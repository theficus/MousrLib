#include "analog.h"
#include "controller.h"

ControllerAnalogStick::ControllerAnalogStick()
{
    this->stickQueue = xQueueCreate(2, sizeof(AnalogStickEvent));
}

bool ControllerAnalogStick::begin()
{
    this->hasFinalized = false;

    BaseType_t result = xTaskCreate(
        ControllerAnalogStick::stickMoveTask,
        "analogStickTask",
        10000,
        this,
        2,
        NULL);

    if (result != pdPASS)
    {
        s_printf("ERROR: Could not create analog stick task. Result: %d\n", result);
        return false;
    }

    this->hasBegun = true;

    return true;
}

bool ControllerAnalogStick::end()
{
    this->hasBegun = false;
    this->hasFinalized = true;
    return true;
}

AnalogStickMovement ControllerAnalogStick::getMovement(const int x, const int y)
{
    AnalogStickMovement as;

    /*
    // Best effort to determine if we're centered or not
    as.isCentered = this->x_ctr >= max(0, evt.cur_x + this->leftCorrection) &&
                    this->x_ctr <= max(0, evt.cur_x + this->rightCorrection) &&
                    this->y_ctr <= max(0, evt.cur_x + this->upCorrection) &&
                    this->y_ctr >= max(0, evt.cur_x + this->downCorrection);
    */

    // If we're out of bounds, recalculate
    int16_t nx = x < 0 ? 0 : x > 1024 ? 1024
                                      : x;
    int16_t ny = y < 0 ? 0 : y > 1024 ? 1024
                                      : y;

    // Convert the position to radian values
    double xr = nx / 4 - 128;
    double yr = ny / 4 - 128;

    // Get the angle and position position from center
    as.angle = -atan2(-yr, xr) * (180.0 / PI);
    as.velocity = sqrt(pow(xr, 2) + pow(yr, 2));
    as.isCentered = as.velocity <= 20; // TODO: Is this better?
    as.x = xr;
    as.y = yr;
    return as;
}

bool ControllerAnalogStick::calibrate(uint16_t upCorrection, uint16_t downCorrection, uint16_t leftCorrection, uint16_t rightCorrection, uint16_t minStickMovementH, uint16_t minStickMovementV)
{
    Controller *c = Controller::getInstance();
    checkTrue(c->hasBegun);
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
    i2cSemCritSec(this->x_ctr = c->ss.analogRead(JOYSTICK_H));
    i2cSemCritSec(this->y_ctr = c->ss.analogRead(JOYSTICK_V));
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

void ControllerAnalogStick::stickMoveTask(void *p)
{
    int16_t x = -1;
    int16_t y = -1;
    s_println(F("stickMoveTask() begin"));
    ControllerAnalogStick *stick = static_cast<ControllerAnalogStick *>(p);
    Controller *c = Controller::getInstance();

    s_println(F("stickMoveTask() waiting for begin() to finish..."));
    while (stick->hasBegun == false)
    {
        delay(100);
    }

    s_println(F("stickMoveTask() starting stick movement loop..."));
    while (stick->hasFinalized == false)
    {
        bool hasRecalibrated;
        int16_t new_x, new_y;
        i2cSemCritSec(new_x = c->ss.analogRead(JOYSTICK_H));
        i2cSemCritSec(new_y = c->ss.analogRead(JOYSTICK_V));
        hasRecalibrated = stick->hasRecalibrated;
        stick->hasRecalibrated = false;

#ifdef JOY_CALIB_DEBUG
        s_printf("x=%d y=%d new_x=%d new_y=%d x_range=%d...%d y_range=%d...%d hasRecal=%d\n", x, y, new_x, new_y, x - c->minStickMovementH, x + c->minStickMovementH, y - c->minStickMovementV, y + c->minStickMovementH, hasRecalibrated);
#endif

        // Ignore minute changes to the analog stick movement as every reading will
        // be slightly different
        if (new_x <= x - stick->minStickMovementH ||
            new_x >= x + stick->minStickMovementH ||
            new_y <= y - stick->minStickMovementV ||
            new_y >= y + stick->minStickMovementV ||
            hasRecalibrated == true)
        {
            AnalogStickEvent evt;
            evt.prev_x = x;
            evt.prev_y = y;
            evt.cur_x = new_x;
            evt.cur_y = new_y;
            xQueueSend(stick->stickQueue, &evt, portMAX_DELAY);

            x = new_x;
            y = new_y;
        }

        delay(200); // Short enough to have quick reads, long enough not to saturate the i2c bus
    }

    s_println(F("stickMoveTask() end"));
    vTaskDelete(NULL);
}
