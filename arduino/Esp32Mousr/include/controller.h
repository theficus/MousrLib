#pragma once
#ifndef MOUSR_CONTROLLER_H
#define MOUSR_CONTROLLER_H

#include <functional>
#include "logging.h"
#include "utility.h"
#include "buttons.h"
#include "analog.h"

#ifdef ARDUINO
#include <Wire.h>

// seesaw includes
#include <Adafruit_seesaw.h>

class Controller
{
private:
    static Controller *singleton;
    QueueHandle_t buttonPressQueue;
    QueueHandle_t stickQueue;
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
    bool calibrate(uint16_t upCorrection = 5, uint16_t downCorrection = 5,
                   uint16_t leftCorrection = 5, uint16_t rightCorrection = 5,
                   uint16_t minStickMovementH = MIN_STICK_MOVEMENT, uint16_t minStickMovementV = MIN_STICK_MOVEMENT);

    QueueHandle_t getButtonPressQueueHandle() { return this->buttonPressQueue; }
    QueueHandle_t getStickQueueHandle() { return this->stickQueue; }

private:
    static void buttonPressTask(void *);
    static void stickMoveTask(void *);
    static void IRAM_ATTR onButtonPress();

    AnalogStickMovement getStickPosition(AnalogStickEvent evt);
    Adafruit_seesaw ss;

    bool isFinalizing = false;
    bool hasRecalibrated = false;
    bool hasBegun = false;

    uint8_t irqPin = 0;
    uint8_t addr = 0;
    int16_t x_ctr = 0;
    int16_t y_ctr = 0;
    int16_t upCorrection = 0;
    int16_t downCorrection = 0;
    int16_t leftCorrection = 0;
    int16_t rightCorrection = 0;
    int16_t minStickMovementH = 0;
    int16_t minStickMovementV = 0;
};

#endif // ARDUINO
#endif // MOUSR_CONTROLLER_H