#pragma once
#ifndef MOUSR_CONTROLLER_H
#define MOUSR_CONTROLLER_H

#include "common.h"
#include <functional>

#ifdef ARDUINO
#include <Wire.h>

// seesaw includes
#include <Adafruit_seesaw.h>

// Seesaw
#define BUTTON_RIGHT 6 // A
#define BUTTON_DOWN 7  // B
#define BUTTON_LEFT 9  // X
#define BUTTON_UP 10   // Y
#define BUTTON_SEL 14  // SELECT
#define JOYSTICK_H 3
#define JOYSTICK_V 2

#define MIN_STICK_MOVEMENT 5

enum ButtonPressState
{
    Up,
    Down,
};

struct ButtonState
{
    ButtonPressState a;
    ButtonPressState b;
    ButtonPressState x;
    ButtonPressState y;
    ButtonPressState sel;
};

struct AnalogStickMovement
{
    float angle;
    float velocity;
    bool isCentered;
};

typedef std::function<void(ButtonState press)> controller_button_callback;
typedef std::function<void(AnalogStickMovement move)> controller_analog_stick_move_callback;
class Controller
{
public:
    Controller(uint8_t irqPin, uint8_t addr = 0x49);
    ~Controller();

    bool begin(controller_button_callback buttonCallback, controller_analog_stick_move_callback stickCallback);
    bool end();
    bool calibrate(uint16_t upCorrection = 5, uint16_t downCorrection = 5,
                   uint16_t leftCorrection = 5, uint16_t rightCorrection = 5,
                   uint16_t minStickMovementH = MIN_STICK_MOVEMENT, uint16_t minStickMovementV = MIN_STICK_MOVEMENT);

private:
    bool giveSemaphore();
    bool takeSemaphore();
    void IRAM_ATTR onButtonPress();
    static void buttonPressTask(void *);
    static void stickMoveTask(void *);
    AnalogStickMovement getStickPosition(int16_t &x, int16_t &y);

    bool isFinalizing;
    bool hasRecalibrated;
    bool hasBegun;

    uint8_t irqPin;
    uint8_t addr;

    Adafruit_seesaw ss;
    SemaphoreHandle_t sem;
    QueueHandle_t buttonQueue;

    // Callbacks
    controller_analog_stick_move_callback stickCallback;
    controller_button_callback buttonCallback;
    //std::function<void()> isr = [&]{ onButtonPress(); };

    // Calibration values
    int16_t x_ctr, y_ctr;
    int16_t upCorrection, downCorrection, leftCorrection, rightCorrection;
    uint16_t minStickMovementH, minStickMovementV;
};

#endif // ARDUINO
#endif // MOUSR_CONTROLLER_H