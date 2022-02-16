#pragma once
#ifdef _MOUSR_ANALOG_H
#else
#define _MOUSR_ANALOG_H

#include <functional>
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

#endif // _MOUSR_ANALOG_H
