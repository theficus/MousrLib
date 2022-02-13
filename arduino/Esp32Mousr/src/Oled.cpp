#include "Oled.h"

/*
void drawPosition()
{
    // u8g2.drawRBox(0, 0, 200, 100, 0);
    u8g2.drawCircle(60, 30, 20);
    u8g2.drawTriangle(20, 5, 27, 50, 5, 32);
}
*/

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
    u8g2.drawFrame(x, y, w, h); // outer battery frame
    u8g2.drawBox(x + w, y + 2, 2, max(h - 4, 2)); // + terminal
    u8g2.drawBox(x + 2, y + 2, dw > 0 ? dw : 0, h - 4); // fill
}