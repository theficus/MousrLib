#pragma once
#ifndef MOUSR_CONTROLLER_H
#define MOUSR_CONTROLLER_H

#include <functional>
#include "logging.h"
#include "utility.h"
#include "analog.h"
#include "buttons.h"

#ifndef ARDUINO
#include "fakes.h"
#else
#include <Wire.h>
// seesaw includes
#include <Adafruit_seesaw.h>
#endif

class ControllerButtons;
class ControllerAnalogStick;

/**
 * @brief Controller device class singleton
 *
 */
class Controller
{
    friend class ControllerButtons;
    friend class ControllerAnalogStick;

private:
    static Controller *singleton;
    Controller();

public:
    static Controller *getInstance()
    {
        if (singleton == NULL)
        {
            singleton = new Controller;
        }

        return singleton;
    };

    bool begin(uint8_t irqPin,
               uint8_t addr = 0x49);

    bool end();

    ControllerAnalogStick *getStick();
    ControllerButtons *getButtons();

private:
    ControllerButtons buttons;
    ControllerAnalogStick stick;
    Adafruit_seesaw ss;
    bool hasBegun = false;
    uint8_t addr = 0;
};

#endif // MOUSR_CONTROLLER_H