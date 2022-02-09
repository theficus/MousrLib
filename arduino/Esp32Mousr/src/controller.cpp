#include "controller.h"

uint32_t s_button_mask =
    (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
    (1 << BUTTON_LEFT) | (1 << BUTTON_UP) |
    (1 << BUTTON_SEL);

Controller::Controller(uint8_t irqPin)
{
    this->irqPin = irqPin;
    this->sem = xSemaphoreCreateBinary();
    this->giveSemaphore();
}

void IRAM_ATTR Controller::onButtonPress()
{
    uint8_t v = 0;
    xQueueSendFromISR(buttonQueue, &v, NULL);
}

void Controller::buttonPressTask(void *)
{
    s_println("buttonPressTask() enter");
    uint32_t lastValue = 0;
    while (this->isFinalizing == false)
    {
        void *p;
        if (xQueueReceive(buttonQueue, &p, 10000000) == false)
        {
            continue;
        }

        xSemaphoreTake(this->sem, portMAX_DELAY);
        uint32_t v = this->ss.digitalReadBulk(s_button_mask);
        xSemaphoreGive(this->sem);

        if (lastValue != v)
        {
            lastValue = v;
            ButtonPress bp = {};

        }
    }

    vTaskDelete(NULL);
}

void Controller::analogStickTask(void *)
{
}

bool Controller::giveSemaphore()
{
    return xSemaphoreGive(this->sem);
}

bool Controller::takeSemaphore()
{
    return xSemaphoreTake(this->sem, portMAX_DELAY);
}

ButtonPress getPressedButtons(uint32_t mask)
{
    ButtonPress bp = {};
    if (!(mask & (1 << BUTTON_RIGHT)))
    {
        bp |= (uint32_t)ButtonPress::A;
    }

    if (!(mask & (1 << BUTTON_DOWN)))
    {

    }
}

void calculatePosition(int16_t x, int16_t y, float &angle, float &velocity)
{
}