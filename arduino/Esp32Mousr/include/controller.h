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
#define BUTTON_RIGHT 6
#define BUTTON_DOWN 7
#define BUTTON_LEFT 9
#define BUTTON_UP 10
#define BUTTON_SEL 14
#define JOYSTICK_H 2
#define JOYSTICK_V 3

enum class ButtonPress
{
    None = 0,
    A = 1,
    B = 1 << 1,
    X = 1 << 2,
    Y = 1 << 3,
    Select = 1 << 4
};

struct AnalogStickMovement
{
    float angle;
    float velocity;
    bool isCentered;
};

typedef std::function<void(ButtonPress press)> controller_button_press_callback;
typedef std::function<void(AnalogStickMovement move)> controller_analog_stick_move_callback;

class Controller
{
public:
    Controller(uint8_t irqPin);
    ~Controller();

    void begin(controller_button_press_callback buttonCallback, controller_analog_stick_move_callback stickCallback);
    void end();
    void calibrate();

private:
    bool giveSemaphore();
    bool takeSemaphore();
    void IRAM_ATTR onButtonPress();
    void buttonPressTask(void *);
    void analogStickTask(void *);

    bool isFinalizing;
    uint8_t irqPin;
    Adafruit_seesaw ss;
    int16_t x_ctr, y_ctr;
    SemaphoreHandle_t sem;
    QueueHandle_t buttonQueue;
};

/*
let radian = atan2f(-self.xAxis.value, self.yAxis.value)
        print("y=\(self.yAxis.value) x=\(self.xAxis.value) rad=\(radian) angle=\(radian * Float(180 / Float.pi))")
        let angle = -radian * Float(180 / Float.pi)
*/

#endif // ARDUINO
#endif // MOUSR_CONTROLLER_H