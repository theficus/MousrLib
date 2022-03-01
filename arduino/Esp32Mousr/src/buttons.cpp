#include "buttons.h"
#include "controller.h"

/*
ButtonState getPressedButtons(uint32_t mask)
{
    ButtonState bs;
    bs.r = (!(mask & 1 << BUTTON_RIGHT)) ? ButtonPress::Down : ButtonPress::Up;
    bs.d = (!(mask & 1 << BUTTON_DOWN)) ? ButtonPress::Down : ButtonPress::Up;
    bs.l = (!(mask & 1 << BUTTON_LEFT)) ? ButtonPress::Down : ButtonPress::Up;
    bs.u = (!(mask & 1 << BUTTON_UP)) ? ButtonPress::Down : ButtonPress::Up;
    bs.sel = (!(mask & 1 << BUTTON_SEL)) ? ButtonPress::Down : ButtonPress::Up;
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

QueueHandle_t buttonSem;
ControllerButtons::ControllerButtons()
{
    buttonSem = xSemaphoreCreateBinary();
    //semGive(buttonSem);
    this->buttonPressQueue = xQueueCreate(3, sizeof(ButtonPressEvent));
}

ButtonStateChange ControllerButtons::getStateChange(ButtonPressEvent &event)
{
    ButtonStateChange bs;
    bs.prev.r = (!(event.prev & 1 << BUTTON_RIGHT)) ? ButtonPress::Down : ButtonPress::Up;
    bs.prev.d = (!(event.prev & 1 << BUTTON_DOWN)) ? ButtonPress::Down : ButtonPress::Up;
    bs.prev.l = (!(event.prev & 1 << BUTTON_LEFT)) ? ButtonPress::Down : ButtonPress::Up;
    bs.prev.u = (!(event.prev & 1 << BUTTON_UP)) ? ButtonPress::Down : ButtonPress::Up;
    bs.prev.sel = (!(event.prev & 1 << BUTTON_SEL)) ? ButtonPress::Down : ButtonPress::Up;
    bs.prev.raw = event.prev;

    bs.cur.r = (!(event.cur & 1 << BUTTON_RIGHT)) ? ButtonPress::Down : ButtonPress::Up;
    bs.cur.d = (!(event.cur & 1 << BUTTON_DOWN)) ? ButtonPress::Down : ButtonPress::Up;
    bs.cur.l = (!(event.cur & 1 << BUTTON_LEFT)) ? ButtonPress::Down : ButtonPress::Up;
    bs.cur.u = (!(event.cur & 1 << BUTTON_UP)) ? ButtonPress::Down : ButtonPress::Up;
    bs.cur.sel = (!(event.cur & 1 << BUTTON_SEL)) ? ButtonPress::Down : ButtonPress::Up;
    bs.cur.raw = event.cur;
    return bs;
}

bool ControllerButtons::begin()
{
    this->hasFinalized = false;

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
    this->hasBegun = true;
    return true;
}

bool ControllerButtons::end()
{
    detachInterrupt(this->irqPin);
    this->hasBegun = false;
    this->hasFinalized = true;
    return true;
}

void ControllerButtons::buttonPressTask(void *p)
{
    s_println(F("buttonPressTask() enter"));
    ControllerButtons *b = static_cast<ControllerButtons *>(p);
    Controller *c = Controller::getInstance();
    s_println(F("buttonPressTask() waiting for begin..."));
    s_println(F("buttonPressTask() starting button loop"));
    uint32_t pv = 0;

    s_println(F("buttonPressTask() waiting for begin() to finish..."));
    while (b->hasBegun == false)
    {
        delay(100);
    }

    s_println(F("buttonPressTask() waiting for button press..."));
    while (b->hasFinalized == false)
    {
        if (semTakeWithTimeout(buttonSem, 5000) == false)
        {
            d_println(F("Did not receive a queued button press..."));
            continue;
        }

        uint32_t v = 0;
        i2cSemCritSecGetValue(c->ss.digitalReadBulk(s_button_mask), v);
        d_printf("press: %u\n", v);
        // Simple debounce. If the value is identical to before we will just discard it.
        if (pv != v)
        {
            ButtonPressEvent evt;
            evt.prev = pv;
            evt.cur = v;
            pv = v;
            logResult(xQueueSendToFront(b->buttonPressQueue, &evt, 0));

            d_printf("queue space available: %zu\n", uxQueueSpacesAvailable(b->buttonPressQueue));
        }
    }

    s_println(F("buttonPressTask() exit"));
    vTaskDelete(NULL);
}

void IRAM_ATTR ControllerButtons::onButtonPress()
{
    //s_println("ISR onButtonPress()");
    xSemaphoreGiveFromISR(buttonSem, pdFALSE);
}
