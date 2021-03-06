#pragma once
#ifdef _MOUSR_BUTTONS_H
#else
#define _MOUSR_BUTTONS_H

#include <functional>
#include "logging.h"
#include "utility.h"

#define BUTTON_RIGHT 6 // A
#define BUTTON_DOWN 7  // B
#define BUTTON_LEFT 9  // X
#define BUTTON_UP 10   // Y
#define BUTTON_SEL 14  // SELECT

const uint32_t s_button_mask =
    (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
    (1 << BUTTON_LEFT) | (1 << BUTTON_UP) |
    (1 << BUTTON_SEL);

enum ButtonPress
{
    Up,
    Down,
};

struct ButtonState
{
    ButtonPress u;
    ButtonPress d;
    ButtonPress l;
    ButtonPress r;
    ButtonPress sel;
    uint32_t raw;
};

struct ButtonStateChange
{
    ButtonState prev;
    ButtonState cur;
};

struct ButtonPressEvent
{
    uint32_t prev;
    uint32_t cur;
};

/**
 * @brief Controller buttons
 *
 */
class ControllerButtons
{
    friend class Controller;

public:
    /**
     * @brief Queued button presses
     *
     */
    QueueHandle_t buttonPressQueue;

    /**
     * @brief Get the State Change object
     *
     * @param event
     * @return ButtonStateChange
     */
    static ButtonStateChange getStateChange(ButtonPressEvent &event);

private:
    /**
     * @brief Construct a new Controller Buttons object
     *
     */
    ControllerButtons();

    /**
     * @brief Starts buttons
     * 
     * @return true Start successful
     * @return false Start unsuccessful
     */
    bool begin();

    /**
     * @brief Ends buttons
     * 
     * @return true End successful
     * @return false End unsucessful
     */
    bool end();
    uint8_t irqPin = 0;
    bool hasBegun = false;
    bool hasFinalized = false;
    static void buttonPressTask(void *);
    static void IRAM_ATTR onButtonPress();
};

#endif // _MOUSR_BUTTONS_H