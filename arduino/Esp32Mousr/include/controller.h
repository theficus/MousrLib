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
    /**
     * @brief Get the singleton controller instance
     * 
     * @return Controller* 
     */
    static Controller *getInstance()
    {
        if (singleton == NULL)
        {
            singleton = new Controller;
        }

        return singleton;
    };

    /**
     * @brief Starts the controller
     * 
     * @param irqPin Interrupt pin
     * @param addr i2c address
     * @return true Controller successfully started
     * @return false Controller could not start
     */
    bool begin(uint8_t irqPin,
               uint8_t addr = 0x49);

    bool end();

    /**
     * @brief Get the analog stick peripheral
     * 
     * @return ControllerAnalogStick* 
     */
    ControllerAnalogStick *getStick();

    /**
     * @brief Get the controller buttons peripheral
     * 
     * @return ControllerButtons* 
     */
    ControllerButtons *getButtons();

private:
    ControllerButtons buttons;
    ControllerAnalogStick stick;
    Adafruit_seesaw ss;
    bool hasBegun = false;
    uint8_t addr = 0;
};

#endif // MOUSR_CONTROLLER_H