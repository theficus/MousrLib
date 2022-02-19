#pragma once
#ifdef _MOUSR_BUTTONS_H
#else
#define _MOUSR_BUTTONS_H

#include <functional>
#include "controller.h"

#define BUTTON_RIGHT 6 // A
#define BUTTON_DOWN 7  // B
#define BUTTON_LEFT 9  // X
#define BUTTON_UP 10   // Y
#define BUTTON_SEL 14  // SELECT

const uint32_t s_button_mask =
    (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
    (1 << BUTTON_LEFT) | (1 << BUTTON_UP) |
    (1 << BUTTON_SEL);

/*
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
};

class ButtonPress
{
private:
    uint32_t prev;
    uint32_t cur;

public:
    ButtonPress(uint32_t prev, uint32_t cur);
};
*/

struct ButtonPressEvent
{
    uint32_t prev;
    uint32_t cur;
};

class ControllerButtons
{
    friend class Controller;

public:
    QueueHandle_t buttonPressQueue;
    
private:
    Controller *controller;
    ControllerButtons(Controller *c);
    bool begin();
    bool end();
    uint8_t irqPin = 0;
    static void buttonPressTask(void *);
    static void IRAM_ATTR onButtonPress();
};

#endif // _MOUSR_BUTTONS_H