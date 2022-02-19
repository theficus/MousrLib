#include "buttons.h"
#include "analog.h"
#include "controller.h"

#ifndef STICK_ENABLED
#define STICK_ENABLED
#endif // STICK_ENABLED

#ifndef BUTTONS_ENABLED
#define BUTTONS_ENABLED
#endif // BUTTONS_ENABLED

Controller *Controller::singleton = 0;
ControllerAnalogStick* stick;
ControllerButtons* buttons;

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
    *buttons = ControllerButtons(this);
    *stick = ControllerAnalogStick(this);
}

ControllerAnalogStick *Controller::getStick()
{
    return stick;
}

ControllerButtons *Controller::getButtons()
{
    return buttons;
}

bool Controller::begin(uint8_t irqPin, uint8_t addr)
{
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

    i2cSemCritSec(this->ss.pinModeBulk(s_button_mask, INPUT_PULLUP));
    i2cSemCritSec(this->ss.setGPIOInterrupts(s_button_mask, 1));

#ifdef STICK_ENABLED
    if (stick->begin() == false)
    {
        goto finalize;
    }
#endif // STICK_ENABLED

#ifdef BUTTONS_ENABLED
    if (buttons->begin() == false)
    {
        goto finalize;
    }
#endif // BUTTONS_ENABLED

    this->hasBegun = true;

finalize:
    return this->hasBegun;
}

bool Controller::end()
{
    buttons->end();
    stick->end();
    this->hasBegun = false;
    this->isFinalizing = false;
    return true;
}
