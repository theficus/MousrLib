#include "Oled.h"
#include "OledDraw.h"

Oled *Oled::singleton = 0;

bool dumpEnable = false;
OledView mode;

void Oled::setView(OledView view)
{
    if (this->isLocked)
    {
        s_println("ERROR: Cannot change view because it is locked.");
    }

    s_printf("Changing view from %d -> %d\n", (int)this->view, (int)view);
    this->view = view;
}

OledView Oled::getView()
{
    return this->view;
}

void Oled::lockView()
{
    this->isLocked = true;
}

void Oled::unlockView()
{
    this->isLocked = false;
}

void Oled::displayQueueTask(void *p)
{
    s_println("Starting displayQueueTask()");
    Oled *oled = Oled::getInstance();
    U8G2 u8g2 = oled->u8g2;
    int ctr = 0;
    while (oled->isFinalizing == false)
    {
        OledDisplayMessage m;
        if (xQueueReceive(oled->displayQueue, &m, 100000) == false)
        {
            continue;
        }

        d_printf("Got OLED message %p. Kind=%d Flags=%u (count=%d) Current view=%d Locked=%d\n", &m, (uint8_t)m.message, (uint8_t)m.flags, ctr, (int)oled->getView(), oled->isLocked);

        if (m.view != oled->view)
        {
            d_println("Ignoring message because it's not for the active view.");
            continue;
        }

        if ((m.flags & OledDisplayFlags::Overlay) == 0)
        {
            u8g2.clearBuffer();
        }
        ctr++;

        drawMode(u8g2, m.message);
        if (m.message == OledMessage::ControllerDiag)
        {
            AnalogStickMovement stick = m.stickPos;
            drawBattery(u8g2, 100, 0, 24, 10, 10, ctr % 10);
            drawAnalogStick(u8g2, stick);
            drawButtons(u8g2, m.buttonPress);
        }
        else if (m.message == OledMessage::Robot)
        {
            u8g2.setFont(u8g2_font_courR12_tf);
            u8g2.drawStr(0, 10, getMousrConnectionStatusString(m.status).c_str());
            drawBattery(u8g2, 100, 0, 24, 10, 10, m.mousrBattery.voltage / 10);
            drawMousrPosition(u8g2, m.mousrMove);
        }
        else if (m.message == OledMessage::Connecting)
        {
            u8g2.setFont(u8g2_font_courR12_tf);
            u8g2.drawStr(0, 10, getMousrConnectionStatusString(m.status).c_str());   
        }
        else
        {
            u8g2.setFont(u8g2_font_courR12_tf);
            u8g2.drawStr(10, 30, "invalid write mode");
            s_printf("Ignoring unsupported message kind %d\n", (int)m.message);
            break;
        }

#ifndef OLED_DEBUG_DUMP
        if (m.flags & OledDisplayFlags::Dump || dumpEnable == true)
#endif // OLED_DEBUG_DUMP
        {
            dumpBuffer(u8g2);
        }

        i2cSemCritSec(u8g2.sendBuffer());
    }

    s_println("Leaving displayQueueTask()");
    vTaskDelete(NULL);
}

Oled::Oled()
{
    this->u8g2 = U8G2CTOR;
    this->displayQueue = xQueueCreate(1, sizeof(OledDisplayMessage));
}

void Oled::queueMessage(OledDisplayMessage *msg)
{
    if (this->hasBegun == false)
    {
        s_println(F("Ignoring queued message because Oled::begin has not been called"));
        return;
    }

    xQueueSendToFront(this->displayQueue, msg, 0);
}

bool Oled::begin()
{
    if (this->hasBegun == true)
    {
        s_println(F("ERROR: Cannot call Oled::begin() because it has already begun."));
    }

    this->isFinalizing = false;
    checkTrue(this->u8g2.begin());
    s_printf("OLED bus clock: %u Wire bus clock: %zu\n", this->u8g2.getBusClock(), Wire.getClock());
    xTaskCreate(displayQueueTask,
                "OledDisplay",
                10000,
                NULL,
                100,
                NULL);
    this->hasBegun = true;
    return true;
}

bool Oled::end()
{
    if (this->isFinalizing == true)
    {
        s_println(F("ERROR: Cannot call Oled::end() because it has already ended."));
    }

    this->isFinalizing = true;
    this->hasBegun = false;
    return true;
}