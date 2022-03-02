#include "buttons.h"
#include "analog.h"
#include "controller.h"
#include "buttons.h"
#include "analog.h"

#ifndef STICK_ENABLED
#define STICK_ENABLED
#endif // STICK_ENABLED

#ifndef BUTTONS_ENABLED
#define BUTTONS_ENABLED
#endif // BUTTONS_ENABLED

Controller *Controller::singleton = 0;

Controller::Controller()
{
}

bool Controller::begin(uint8_t irqPin, uint8_t addr)
{
    if (this->hasBegun == true)
    {
        s_println(F("ERROR: begin() ignored becuase begin() was previously called without end()"));
        return false;
    }

    bool didBegin = false;
    i2cSemCritSecGetValue(ss.begin(addr), didBegin);
    if (didBegin == false)
    {
        s_println(F("ERROR: Seesaw.begin() was unsuccessful."));
        return false;
    }
    
    i2cSemCritSec(this->ss.pinModeBulk(s_button_mask, INPUT_PULLUP));
    i2cSemCritSec(this->ss.setGPIOInterrupts(s_button_mask, 1));

#ifdef STICK_ENABLED
    checkTrue(this->stick.begin());
#endif // STICK_ENABLED

#ifdef BUTTONS_ENABLED
    this->buttons.irqPin = irqPin;
    checkTrue(this->buttons.begin());
#endif // BUTTONS_ENABLED

    this->hasBegun = true;
    return true;
}

ControllerButtons *Controller::getButtons()
{
    return &this->buttons;
}

ControllerAnalogStick *Controller::getStick()
{
    return &this->stick;
}

bool Controller::end()
{
    this->buttons.end();
    this->stick.end();
    this->hasBegun = false;
    return true;
}
