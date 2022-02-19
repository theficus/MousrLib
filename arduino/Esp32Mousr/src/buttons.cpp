#include "buttons.h"

/*
ButtonState getPressedButtons(uint32_t mask)
{
    ButtonState bs;
    bs.r = (!(mask & 1 << BUTTON_RIGHT)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.d = (!(mask & 1 << BUTTON_DOWN)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.l = (!(mask & 1 << BUTTON_LEFT)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.u = (!(mask & 1 << BUTTON_UP)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.sel = (!(mask & 1 << BUTTON_SEL)) ? ButtonPressState::Down : ButtonPressState::Up;
    bs.raw = mask;

    s_printf("PRESS: u=%d d=%d l=%d r=%d s=%d bin=" BYTE_TO_BINARY_PATTERN "\n",
             (int)bs.u,
             (int)bs.d,
             (int)bs.l,
             (int)bs.r,
             (int)bs.sel,
             BYTE_TO_BINARY(bs.raw));

    return bs;
}
*/

SemaphoreHandle_t buttonSem;

ControllerButtons::ControllerButtons(Controller* c)
{
    this->controller = c;
    buttonSem = xSemaphoreCreateBinary();
    this->buttonPressQueue = xQueueCreate(3, sizeof(ButtonPressEvent));
}

bool ControllerButtons::begin()
{
    BaseType_t result = xTaskCreate(
        ControllerButtons::buttonPressTask,
        "buttonPressTask",
        10000,
        this,
        1,
        NULL);

    if (result != pdPASS)
    {
        s_printf("ERROR: Could not create button press task. Result: %d\n", result);
        return false;
    }

    pinMode(this->irqPin, INPUT);
    attachInterrupt(this->irqPin, ControllerButtons::onButtonPress, FALLING);

    return true;
}

bool ControllerButtons::end()
{
    detachInterrupt(this->irqPin);
    return true;
}

void ControllerButtons::buttonPressTask(void *p)
{
    s_println(F("buttonPressTask() enter"));
    ControllerButtons *b = static_cast<ControllerButtons *>(p);
    Controller *c = Controller::getInstance();
    s_println(F("buttonPressTask() waiting for begin..."));
    s_println(F("starting button loop"));
    uint32_t pv = 0;
    while (c->isFinalizing == false)
    {
        if (semTakeWithTimeout(buttonSem, 5000) == false)
        {
            s_println(F("Did not receive a queued button press..."));
            continue;
        }

        uint32_t v;
        i2cSemCritSec(v = c->ss.digitalReadBulk(s_button_mask));
        s_printf("press: %u\n", v);

        // Simple debounce. If the value is identical to before we will just discard it.
        if (pv != v)
        {
            ButtonPressEvent evt;
            evt.prev = pv;
            evt.cur = v;
            pv = v;
            s_printf("xqueuesend: %d\n", xQueueSendToFront(b->buttonPressQueue, &evt, 0));
            s_printf("queue space available: %zu\n", uxQueueSpacesAvailable(b->buttonPressQueue));
        }
    }

    s_println(F("buttonPressTask() exit"));
    vTaskDelete(NULL);
}

void IRAM_ATTR ControllerButtons::onButtonPress()
{
    xSemaphoreGiveFromISR(buttonSem, pdFALSE);
}
