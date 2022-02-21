#pragma once
#ifndef _MOUSR_ANALOG_H
#define _MOUSR_ANALOG_H

#include <functional>
#include <algorithm>
#include "logging.h"
#include "utility.h"

#ifndef ARDUINO_ARCH_ESP32
#include "fakes.h"
#endif // ARDUINO_ARCH_ESP32

// Seesaw
#define JOYSTICK_H 3
#define JOYSTICK_V 2

#define MIN_STICK_MOVEMENT 5

struct AnalogStickEvent
{
    int16_t prev_x;
    int16_t prev_y;
    int16_t cur_x;
    int16_t cur_y;
};

struct AnalogStickMovement
{
    float x;
    float y;
    float angle;
    float velocity;
    bool isCentered;
};

class Controller;

class ControllerAnalogStick
{
    friend class Controller;

public:
    QueueHandle_t stickQueue;

    bool calibrate(uint16_t upCorrection = 5, uint16_t downCorrection = 5,
                   uint16_t leftCorrection = 5, uint16_t rightCorrection = 5,
                   uint16_t minStickMovementH = MIN_STICK_MOVEMENT, uint16_t minStickMovementV = MIN_STICK_MOVEMENT);

    AnalogStickMovement getMovement(const int x, const int y);

private:
    static void stickMoveTask(void *);
    ControllerAnalogStick();
    bool begin();
    bool end();

    bool hasRecalibrated = false;
    bool hasBegun = false;
    bool hasFinalized = false;
    int16_t x_ctr = 0;
    int16_t y_ctr = 0;
    int16_t upCorrection = 0;
    int16_t downCorrection = 0;
    int16_t leftCorrection = 0;
    int16_t rightCorrection = 0;
    int16_t minStickMovementH = 0;
    int16_t minStickMovementV = 0;
};

#endif // _MOUSR_ANALOG_H
