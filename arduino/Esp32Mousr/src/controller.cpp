#include "controller.h"

#ifndef STICK_ENABLED
#define STICK_ENABLED
#endif // STICK_ENABLED

#ifndef BUTTONS_ENABLED
#define BUTTONS_ENABLED
#endif // BUTTONS_ENABLED

// 27 = sda
// 33 = scl
Controller *Controller::singleton = 0;
SemaphoreHandle_t buttonSem;

// Make sure someone has called Controller::begin() before running functions that
// rely on this.
#define checkIsBegun()                                                                      \
    if (this->hasBegun == false)                                                            \
    {                                                                                       \
        s_printf("Cannot run command %s because begin has not been called.", __FUNCTION__); \
        return false;                                                                       \
    }

Controller::Controller()
{
    // Wire1.setPins(27, 33);
    // this->ss = Adafruit_seesaw(&Wire1);
    buttonSem = xSemaphoreCreateBinary();
    // semGive(buttonSem);

    this->buttonPressQueue = xQueueCreate(3, sizeof(ButtonPressEvent));
    this->stickQueue = xQueueCreate(2, sizeof(AnalogStickEvent));
}

bool Controller::begin(uint8_t irqPin, uint8_t addr)
{
    BaseType_t result;
    s_printf("has begun=%d\n", this->hasBegun);

    if (this->hasBegun == true)
    {
        s_println(F("Controller is already set up. Cannot continue."));
        goto finalize;
    }

    bool ssDidBegin;
    i2cSemCritSec(ssDidBegin = ss.begin(addr));
    if (ssDidBegin == false)
    {
        s_println(F("ERROR: Could not start Seesaw"));
        goto finalize;
    }

    this->irqPin = irqPin;
    i2cSemCritSec(this->ss.pinModeBulk(s_button_mask, INPUT_PULLUP));
    i2cSemCritSec(this->ss.setGPIOInterrupts(s_button_mask, 1));

#ifdef STICK_ENABLED
    result = xTaskCreate(
        Controller::stickMoveTask,
        "analogStickTask",
        10000,
        this,
        2,
        NULL);

    if (result != pdPASS)
    {
        s_printf("ERROR: Could not create analog stick task. Result: %d\n", result);
        goto finalize;
    }
#endif // STICK_ENABLED

#ifdef BUTTONS_ENABLED
    result = xTaskCreate(
        Controller::buttonPressTask,
        "buttonPressTask",
        10000,
        NULL,
        1,
        NULL);

    if (result != pdPASS)
    {
        s_printf("ERROR: Could not create button press task. Result: %d\n", result);
        goto finalize;
    }

    pinMode(this->irqPin, INPUT);
    attachInterrupt(this->irqPin, Controller::onButtonPress, FALLING);

#endif // BUTTONS_ENABLED
    this->hasBegun = true;

finalize:
    return this->hasBegun;
}

bool Controller::calibrate(uint16_t upCorrection, uint16_t downCorrection, uint16_t leftCorrection, uint16_t rightCorrection, uint16_t minStickMovementH, uint16_t minStickMovementV)
{
    checkIsBegun();

    s_println(F("Calibrating analog stick"));

    // Hold on to the semaphore for the entirety of the function to make sure
    // we aren't reading the stick while calibration is happening and messing up
    // positional calcluations
    this->upCorrection = upCorrection;
    this->downCorrection = -downCorrection;
    this->leftCorrection = -leftCorrection;
    this->rightCorrection = rightCorrection;

    this->minStickMovementH = minStickMovementH;
    this->minStickMovementV = minStickMovementV;
    i2cSemCritSec(this->x_ctr = ss.analogRead(JOYSTICK_H));
    i2cSemCritSec(this->y_ctr = ss.analogRead(JOYSTICK_V));
    s_printf("Calibrated analog stick. Center X=%d Y=%d. Correction values: L=%d R=%d U=%d D=%d\n",
             this->x_ctr,
             this->y_ctr,
             this->leftCorrection,
             this->rightCorrection,
             this->upCorrection,
             this->downCorrection);
    this->hasRecalibrated = true;
    return true;
}

bool Controller::end()
{
    detachInterrupt(this->irqPin);
    this->hasBegun = false;
    this->isFinalizing = false;
    return true;
}

void IRAM_ATTR Controller::onButtonPress()
{
    if (singleton == NULL)
    {
        return;
    }

    xSemaphoreGiveFromISR(buttonSem, pdFALSE);
}

void Controller::buttonPressTask(void *p)
{
    s_println(F("buttonPressTask() enter"));
    Controller *c = Controller::getInstance();
    s_println(F("buttonPressTask() waiting for begin..."));
    s_println(F("starting button loop"));
    uint32_t pv = 0;
    while (c->isFinalizing == false)
    {
        if (semTakeWithTimeout(buttonSem, 5000) == false)
        {
            s_println("Did not receive a queued button press...");
            continue;
        }

        uint32_t v;
        i2cSemCritSec(v = c->ss.digitalReadBulk(s_button_mask));
        s_printf("press: %u\n", v);

        // Simple debounce. If the value is identical to before we will just discard it.
        if (pv != v)
        {
            ButtonPressEvent evt;
            evt.prev = pv;
            evt.cur = v;
            pv = v;
            s_printf("xqueuesend: %d\n", xQueueSendToFront(c->buttonPressQueue, &evt, 0));
            s_printf("queue space available: %zu\n", uxQueueSpacesAvailable(c->buttonPressQueue));
        }
    }

    s_println(F("buttonPressTask() exit"));
    vTaskDelete(NULL);
}

void Controller::stickMoveTask(void *p)
{
    int16_t x = -1;
    int16_t y = -1;
    s_println(F("stickMoveTask() begin"));
    Controller *c = static_cast<Controller *>(p);

    s_println(F("stickMoveTask() waiting to begin..."));
    s_println(F("starting stick move loop"));
    while (c->isFinalizing == false)
    {
        bool hasRecalibrated;
        int16_t new_x, new_y;
        i2cSemCritSec(new_x = c->ss.analogRead(JOYSTICK_H));
        i2cSemCritSec(new_y = c->ss.analogRead(JOYSTICK_V));
        hasRecalibrated = c->hasRecalibrated;
        c->hasRecalibrated = false;

#ifdef JOY_CALIB_DEBUG
        s_printf("x=%d y=%d new_x=%d new_y=%d x_range=%d...%d y_range=%d...%d hasRecal=%d\n", x, y, new_x, new_y, x - c->minStickMovementH, x + c->minStickMovementH, y - c->minStickMovementV, y + c->minStickMovementH, hasRecalibrated);
#endif

        // Ignore minute changes to the analog stick movement as every reading will
        // be slightly different
        if (new_x <= x - c->minStickMovementH ||
            new_x >= x + c->minStickMovementH ||
            new_y <= y - c->minStickMovementV ||
            new_y >= y + c->minStickMovementV ||
            hasRecalibrated == true)
        {
            /*
            AnalogStickMovement as = c->getStickPosition(new_x, new_y);
            c->stickCallback(as);
            */

            AnalogStickEvent evt;
            evt.prev_x = x;
            evt.prev_y = y;
            evt.cur_x = new_x;
            evt.cur_y = new_y;
            xQueueSend(c->stickQueue, &evt, portMAX_DELAY);

            x = new_x;
            y = new_y;
        }

        delay(250); // Short enough to have quick reads, long enough not to saturate the i2c bus
    }

    s_println(F("stickMoveTask() end"));
    vTaskDelete(NULL);
}

AnalogStickMovement Controller::getStickPosition(AnalogStickEvent evt)
{
    AnalogStickMovement as;

    // Best effort to determine if we're centered or not
    as.isCentered = this->x_ctr >= max(0, evt.cur_x + this->leftCorrection) &&
                    this->x_ctr <= max(0, evt.cur_x + this->rightCorrection) &&
                    this->y_ctr <= max(0, evt.cur_x + this->upCorrection) &&
                    this->y_ctr >= max(0, evt.cur_x + this->downCorrection);

    // If we're out of bounds, recalculate
    evt.cur_x = evt.cur_x < 0 ? 0 : evt.cur_x > 1024 ? 1024
                                                     : evt.cur_x;
    evt.cur_y = evt.cur_y < 0 ? 0 : evt.cur_y > 1024 ? 1024
                                                     : evt.cur_y;

    // Convert the position to radian values
    double xr = evt.cur_x / 4 - 128;
    double yr = evt.cur_y / 4 - 128;

    // Get the angle and position position from center
    as.angle = -atan2(-yr, xr) * (180.0 / PI);
    as.velocity = sqrt(pow(xr, 2) + pow(yr, 2));
    as.x = xr;
    as.y = yr;
    return as;
}
