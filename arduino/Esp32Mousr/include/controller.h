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
    bool calibrate(int16_t upCorrection = 0, int16_t downCorrection = 0,
                   int16_t leftCorrection = 0, int16_t rightCorrection = 0,
                   uint16_t minStickMovementH = 0, uint16_t minStickMovementV = 0);

private:
    bool giveSemaphore();
    bool takeSemaphore();
    void IRAM_ATTR onButtonPress();
    void buttonPressTask(void *);
    void analogStickTask(void *);
    AnalogStickMovement getStickPosition(int16_t &x, int16_t &y);

    bool isFinalizing;
    bool hasBegun;
    uint8_t irqPin;
    uint8_t addr;
    Adafruit_seesaw ss;
    SemaphoreHandle_t sem;
    QueueHandle_t buttonQueue;
    controller_analog_stick_move_callback stickCallback;
    controller_button_callback buttonCallback;

    // Calibration values
    int16_t x_ctr, y_ctr;
    int16_t upCorrection, downCorrection, leftCorrection, rightCorrection;
    uint16_t minStickMovementH, minStickMovementV;
};

/*
let radian = atan2f(-self.xAxis.value, self.yAxis.value)
        print("y=\(self.yAxis.value) x=\(self.xAxis.value) rad=\(radian) angle=\(radian * Float(180 / Float.pi))")
        let angle = -radian * Float(180 / Float.pi)
*/

#endif // ARDUINO
#endif // MOUSR_CONTROLLER_H