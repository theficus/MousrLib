#include "Oled.h"

Oled *Oled::singleton = 0;

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

        s_printf("Got message %p is (count: %d)\n", &m, ctr);
        u8g2.clearBuffer();
        ctr++;

        if(m.viewKind == OledView::ControllerDiagStick)
        {
            AnalogStickMovement stick = m.stickPos;
            drawBattery(oled->u8g2, 100, 2, 24, 10, 5, ctr % 6);
            drawPos(oled->u8g2, 30, 30, 20, stick.angle, stick.isCentered);
            drawDetails(oled->u8g2, 55, 40, stick.isCentered ? 0 : stick.angle, (float)(ctr % 90), stick.isCentered ? 0 : stick.velocity);
        }
        else
        {
            u8g2.setFont(u8g2_font_courR08_tf);
            u8g2.drawStr(10, 10, "invalid write mode");
            s_printf("Ignoring unsupported message kind %d", (int)m.viewKind);
            break;
        }

        i2cSemCritSec(u8g2.sendBuffer());
    }

    s_println("Leaving displayQueueTask()");
    vTaskDelete(NULL);
}

Oled::Oled()
{
    // this->displayQueue = xQueueCreate(1, sizeof(OledDisplayMessage));
    this->u8g2 = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0);
    this->displayQueue = xQueueCreate(1, sizeof(OledDisplayMessage));
}

int ctr = 0;
void Oled::queueMessage(OledDisplayMessage *msg)
{
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

void drawPos(U8G2 u8g2, int x, int y, int rad, int deg, bool ctr)
{
    float xx, yy;
    if (ctr == false)
    {
        xx = (rad * cos(deg * PI / 180.0)) + x;
        yy = (rad * sin(deg * PI / 180.0)) + y;
    }
    else
    {
        xx = x;
        yy = y;
    }

    d_printf("deg:%d x:%d y:%d xx:%f yy:%f rad:%f\n", deg, x, y, xx, yy, rad);
    u8g2.drawCircle(x, y, rad);
    u8g2.drawDisc(xx, yy, 4);
    u8g2.drawLine(x, y, xx, yy);
}

void drawDetails(U8G2 u8g2, int x, int y, float angle, float tilt, float speed)
{
    char buf[32] = {0};
    u8g2.setFont(u8g2_font_courR08_tf);
    sprintf(buf, "Angle: %03.2f", angle);
    u8g2.drawStr(x, y, buf);
    sprintf(buf, "Tilt:  %02.2f", tilt);
    u8g2.drawStr(x, y + 9, buf);
    sprintf(buf, "Speed: %02.2f", speed);
    u8g2.drawStr(x, y + 18, buf);
}

void drawBattery(U8G2 u8g2, int x, int y, int w, int h, int segments, int lvl)
{
    int dw = w * lvl / segments - 4;
    d_printf("seg:%d lvl:%d dw:%d\n", segments, lvl, dw);
    u8g2.drawFrame(x, y, w, h);                         // outer battery frame
    u8g2.drawBox(x + w, y + 2, 2, max(h - 4, 2));       // + terminal
    u8g2.drawBox(x + 2, y + 2, dw > 0 ? dw : 0, h - 4); // fill
}