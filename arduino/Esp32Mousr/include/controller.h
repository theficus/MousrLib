#pragma once
#ifndef MOUSR_CONTROLLER_H
#define MOUSR_CONTROLLER_H

#include "logging.h"
#include "utility.h"
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
    ButtonPressState u;
    ButtonPressState d;
    ButtonPressState l;
    ButtonPressState r;
    ButtonPressState sel;
    uint32_t raw;
};

struct ButtonStateChange
{
    ButtonState newState;
    ButtonState oldState;
};

struct AnalogStickMovement
{
    float angle;
    float velocity;
    bool isCentered;
};

typedef std::function<void(ButtonStateChange press)> controller_button_callback;
typedef std::function<void(AnalogStickMovement move)> controller_analog_stick_move_callback;

class Controller
{
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

    bool begin(controller_button_callback buttonCallback,
               controller_analog_stick_move_callback stickCallback,
               uint8_t irqPin,
               uint8_t addr = 0x49);

    bool end();
    bool calibrate(uint16_t upCorrection = 5, uint16_t downCorrection = 5,
                   uint16_t leftCorrection = 5, uint16_t rightCorrection = 5,
                   uint16_t minStickMovementH = MIN_STICK_MOVEMENT, uint16_t minStickMovementV = MIN_STICK_MOVEMENT);

private:
    static void buttonPressTask(void *);
    static void stickMoveTask(void *);
    static void IRAM_ATTR onButtonPress();

    AnalogStickMovement getStickPosition(int16_t &x, int16_t &y);
    Adafruit_seesaw ss;
    SemaphoreHandle_t startSem;
    QueueHandle_t buttonQueue;
    controller_analog_stick_move_callback stickCallback;
    controller_button_callback buttonCallback;

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