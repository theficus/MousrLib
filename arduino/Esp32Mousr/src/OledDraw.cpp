/**
 * @file OledDraw.cpp
 * @author Adam Meltzer (11638244+theficus@users.noreply.github.com)
 * @brief Helper methods for drawing things on the OLED display
 * @version 0.1
 * @date 2022-03-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "OledDraw.h"

void dumpBuffer(U8G2 u8g2)
{
#ifndef OLED_DEBUG_DUMP
    return;
#endif

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

void drawAnalogStick(U8G2 u8g2, AnalogStickMovement m)
{
    drawPos(u8g2, 30, 30, 20, m.angle, m.isCentered);
    drawDetails(u8g2, 55, 40, m.isCentered ? 0 : m.angle, 0, m.isCentered ? 0 : m.velocity);
}

void drawMousrPosition(U8G2 u8g2, MousrMovementMsg m)
{
    drawPos(u8g2, 30, 30, 20, m.angle, false);
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