#include "Oled.h"
#include "Log.h"

// OLED views
static U8G2 u8g2 = U8G2_SSD1306_128X64_NONAME_2_HW_I2C(U8G2_R0);           // Graphical mode

void drawPosition()
{
    // u8g2.drawRBox(0, 0, 200, 100, 0);
    u8g2.drawCircle(60, 30, 20);
    u8g2.drawTriangle(20, 5, 27, 50, 5, 32);
}

void writeOled(const char *text)
{
    u8g2.firstPage();
    do
    {
        u8g2.setFont(u8g2_font_ncenB14_tf);
        u8g2.drawStr(0, 20, text);

        u8g2.setFont(u8g2_font_unifont_t_symbols);

        u8g2.drawGlyph(20, 40, ARROW_UP_FILL);
        u8g2.drawGlyph(20, 60, ARROW_DOWN_FILL);
        u8g2.drawGlyph(10, 50, ARROW_LEFT_FILL);
        u8g2.drawGlyph(30, 50, ARROW_RIGHT_FILL);

        drawPosition();
    } while (u8g2.nextPage());
}