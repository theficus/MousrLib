#include "controller.h"

Controller *Controller::singleton = 0;

constexpr uint32_t s_button_mask =
    (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
    (1 << BUTTON_LEFT) | (1 << BUTTON_UP) |
    (1 << BUTTON_SEL);

// Make sure someone has called Controller::begin() before running functions that
// rely on this.
#define checkIsBegun()                                                                      \
    if (this->hasBegun == false)                                                            \
    {                                                                                       \
        s_printf("Cannot run command %s because begin has not been called.", __FUNCTION__); \
        return false;                                                                       \
    }

ButtonState getPressedButtons(uint32_t mask)
{
    ButtonState bs;
    bs.r = (!(mask & 1 << BUTTON_RIGHT)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.d = (!(mask & 1 << BUTTON_DOWN)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.l = (!(mask & 1 << BUTTON_LEFT)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.u = (!(mask & 1 << BUTTON_UP)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.sel = (!(mask & 1 << BUTTON_SEL)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.raw = mask;

    d_printf("PRESS: u=%d d=%d l=%d r=%d s=%d bin=" BYTE_TO_BINARY_PATTERN "\n",
             (int)bs.u,
             (int)bs.d,
             (int)bs.l,
             (int)bs.r,
             (int)bs.sel,
             BYTE_TO_BINARY(bs.mask));

    return bs;
}

Controller::Controller()
{
    this->opSem = xSemaphoreCreateBinary();
    this->startSem = xSemaphoreCreateBinary();
    this->buttonQueue = xQueueCreate(10, sizeof(uint8_t));

    semGive(this->opSem);
    semGive(this->startSem);
    semTake(this->startSem);
}

bool Controller::begin(controller_button_callback buttonCallback, controller_analog_stick_move_callback stickCallback, uint8_t irqPin, uint8_t addr)
{
    BaseType_t result;

    if (buttonCallback == nullptr ||
        stickCallback == nullptr)
    {
        s_println(F("ERROR: Missing button or stick callback parameter value."));
        return false;
    }

    s_printf("has begun=%d\n", this->hasBegun);

    if (this->hasBegun == true)
    {
        s_println(F("Controller is already set up. Cannot continue."));
        goto finalize;
    }

    if (ss.begin(addr) == false)
    {
        s_println(F("ERROR: Could not start Seesaw"));
        goto finalize;
    }

    this->irqPin = irqPin;
    this->ss.pinModeBulk(s_button_mask, INPUT_PULLUP);
    this->ss.setGPIOInterrupts(s_button_mask, 1);
    this->buttonCallback = buttonCallback;
    this->stickCallback = stickCallback;

    result = xTaskCreate(
        Controller::buttonPressTask,
        "buttonPressTask",
        10000,
        this,
        1,
        NULL);

    if (result != pdPASS)
    {
        s_printf("ERROR: Could not create button press task. Result: %d\n", result);
        goto finalize;
    }

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

    pinMode(this->irqPin, INPUT);
    attachInterrupt(this->irqPin, Controller::onButtonPress, FALLING);
    this->hasBegun = true;

finalize:
    semGive(this->startSem);
    return this->hasBegun;
}

bool Controller::calibrate(uint16_t upCorrection, uint16_t downCorrection, uint16_t leftCorrection, uint16_t rightCorrection, uint16_t minStickMovementH, uint16_t minStickMovementV)
{
    checkIsBegun();

    s_println(F("Calibrating analog stick"));

    // Hold on to the semaphore for the entirety of the function to make sure
    // we aren't reading the stick while calibration is happening and messing up
    // positional calcluations
    semTake(this->opSem);

    this->upCorrection = upCorrection;
    this->downCorrection = -downCorrection;
    this->leftCorrection = -leftCorrection;
    this->rightCorrection = rightCorrection;

    this->minStickMovementH = minStickMovementH;
    this->minStickMovementV = minStickMovementV;
    this->x_ctr = ss.analogRead(JOYSTICK_H);
    this->y_ctr = ss.analogRead(JOYSTICK_V);
    s_printf("Calibrated analog stick. Center X=%d Y=%d. Correction values: L=%d R=%d U=%d D=%d\n",
             this->x_ctr,
             this->y_ctr,
             this->leftCorrection,
             this->rightCorrection,
             this->upCorrection,
             this->downCorrection);
    this->hasRecalibrated = true;
    semGive(this->opSem);
    return true;
}

bool Controller::end()
{
    detachInterrupt(this->irqPin);
    this->hasBegun = false;
    this->isFinalizing = false;
    semTake(startSem);
    return true;
}

void IRAM_ATTR Controller::onButtonPress()
{
    if (singleton == NULL)
    {
        return;
    }

    uint8_t v = 0;
    xQueueSend(singleton->buttonQueue, &v, portMAX_DELAY);
}

void Controller::buttonPressTask(void *p)
{
    s_println(F("buttonPressTask() enter"));
    uint32_t lastValue = 0;
    Controller *c = static_cast<Controller *>(p);

    s_println(F("buttonPressTask() waiting for begin..."));
    semWait(c->startSem);
    s_println(F("starting button loop"));
    while (c->hasBegun == true && c->isFinalizing == false)
    {
        // Don't care about the queue value, just that we get something queued
        void *x;
        if (xQueueReceive(c->buttonQueue, &x, 10000000) == false)
        {
            continue;
        }

        semTake(c->opSem);
        uint32_t v = c->ss.digitalReadBulk(s_button_mask);
        ButtonState prev{};
        semGive(c->opSem);

        // Simple debounce. If the value is identical to before we will just discard it.
        if (lastValue != v)
        {
            lastValue = v;
            ButtonStateChange bc;
            bc.oldState.u = prev.u;
            bc.oldState.d = prev.d;
            bc.oldState.l = prev.l;
            bc.oldState.r = prev.r;
            bc.oldState.sel = prev.sel;

            bc.newState = getPressedButtons(v);
            prev.u = bc.newState.u;
            prev.d = bc.newState.d;
            prev.l = bc.newState.l;
            prev.r = bc.newState.r;
            prev.sel = bc.newState.sel;
            c->buttonCallback(bc);
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
    semWait(c->startSem);
    s_println(F("starting stick move loop"));
    while (c->hasBegun == true &&
           c->isFinalizing == false)
    {
        bool hasRecalibrated;
        semTake(c->opSem);
        int16_t new_x = c->ss.analogRead(JOYSTICK_H);
        int16_t new_y = c->ss.analogRead(JOYSTICK_V);
        hasRecalibrated = c->hasRecalibrated;
        c->hasRecalibrated = false;
        semGive(c->opSem);

        // Ignore minute changes to the analog stick movement as every reading will
        // be slightly different
        if (new_x <= x - c->minStickMovementH ||
            new_x >= x + c->minStickMovementH ||
            new_y <= y - c->minStickMovementV ||
            new_y >= y + c->minStickMovementV ||
            hasRecalibrated == true)
        {
            AnalogStickMovement as = c->getStickPosition(new_x, new_y);
            c->stickCallback(as);
            x = new_x;
            y = new_y;
        }

        delay(100); // Short enough to have quick reads, long enough not to saturate the i2c bus
    }

    s_println(F("stickMoveTask() end"));
    vTaskDelete(NULL);
}

AnalogStickMovement Controller::getStickPosition(int16_t &x, int16_t &y)
{
    AnalogStickMovement as;

    // Best effort to determine if we're centered or not
    as.isCentered = this->x_ctr >= max(0, x + this->leftCorrection) &&
                    this->x_ctr <= max(0, x + this->rightCorrection) &&
                    this->y_ctr <= max(0, y + this->upCorrection) &&
                    this->y_ctr >= max(0, y + downCorrection);

    // If we're out of bounds, recalculate
    x = x < 0 ? 0 : x > 1024 ? 1024
                             : x;
    y = y < 0 ? 0 : y > 1024 ? 1024
                             : y;

    // Convert the position to radian values
    double xr = x / 4 - 128;
    double yr = y / 4 - 128;

    // Get the angle and position position from center
    as.angle = -atan2(-xr, yr) * (180.0 / PI);
    as.velocity = sqrt(pow(xr, 2) + pow(yr, 2));
    return as;
}
