#include "Oled.h"

//#define OLED_DEBUG_DUMP
#define OLED_DEBUG_DUMP_OUTPUT_DESTINATION Serial

Oled *Oled::singleton = 0;

bool dumpEnable = false;
OledView mode;

void dumpBuffer(U8G2 u8g2)
{
    s_println(F("Dumping OLED buffer:"));
    s_println(F("-----------------------------------------------------------"));
    u8g2.writeBufferXBM(OLED_DEBUG_DUMP_OUTPUT_DESTINATION);
    s_println(F("-----------------------------------------------------------"));
}

void drawMode(U8G2 u8g2, OledMessage view)
{
    u8g2.setFont(u8g2_font_profont10_mf);
    char c[30];
    sprintf(c, "MODE: %d\n", (int)view);
    u8g2.drawStr(2, 60, c);
}

void drawAnalogStick(U8G2 u8g2, AnalogStickMovement m)
{
    drawPos(u8g2, 30, 30, 20, m.angle, m.isCentered);
    drawDetails(u8g2, 55, 40, m.isCentered ? 0 : m.angle, 0, m.isCentered ? 0 : m.velocity);
}

void drawMousrPosition(U8G2 u8g2, MousrMovementMsg m)
{
    drawPos(u8g2, 30, 30, 20, m.angle);
    drawDetails(u8g2, 55, 40, m.angle, m.held, m.speed);
}

void drawButtons(U8G2 u8g2, ButtonState buttons)
{
    const int y_base = 20;
    const int step = 10;
    int x = 52;
    u8g2.setFont(u8g2_font_9x15_t_symbols);
    if (buttons.u == ButtonPress::Down)
    {
        u8g2.drawGlyph(x, y_base, ARROW_UP_FILL);
    }
    else
    {
        u8g2.drawGlyph(x, y_base, ARROW_UP_EMPTY);
    }

    x += step;
    if (buttons.d == ButtonPress::Down)
    {
        u8g2.drawGlyph(x, y_base, ARROW_DOWN_FILL);
    }
    else
    {
        u8g2.drawGlyph(x, y_base, ARROW_DOWN_EMPTY);
    }

    x += step;
    if (buttons.l == ButtonPress::Down)
    {
        u8g2.drawGlyph(x, y_base, ARROW_LEFT_FILL);
    }
    else
    {
        u8g2.drawGlyph(x, y_base, ARROW_LEFT_EMPTY);
    }

    x += step;
    if (buttons.r == ButtonPress::Down)
    {
        u8g2.drawGlyph(x, y_base, ARROW_RIGHT_FILL);
    }
    else
    {
        u8g2.drawGlyph(x, y_base, ARROW_RIGHT_EMPTY);
    }

    x += step;
    if (buttons.sel == ButtonPress::Down)
    {
        // dumpEnable = true;
        u8g2.drawGlyph(x, y_base, SNOWMAN);
    }
    else
    {
        // dumpEnable = false;
        u8g2.drawGlyph(x, y_base, UMBRELLA);
    }
}

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
    // this->displayQueue = xQueueCreate(1, sizeof(OledDisplayMessage));
    //this->u8g2 = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0);
    this->u8g2 = U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0);

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

    d_printf("deg:%d x:%d y:%d x(cir):%f y(cir):%f rad:%d\n", deg, x, y, xx, yy, rad);
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