#pragma once
#ifndef MOUSR_OLED_H
#define MOUSR_OLED_H
#ifdef ARDUINO

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <U8g2lib.h>
#include "logging.h"
#include "utility.h"

// Glyphs
#define ARROW_UP_FILL 0x25b2
#define ARROW_UP_EMPTY 0x25b3
#define ARROW_RIGHT_FILL 0x25b6
#define ARROW_RIGHT_EMPTY 0x25b7
#define ARROW_DOWN_FILL 0x25bc
#define ARROW_DOWN_EMPTY 0x25bd
#define ARROW_LEFT_FILL 0x25c0
#define ARROW_LEFT_EMPTY 0x25c1

//U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0);
static U8G2 u8g2 = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0); // Graphical mode

void drawBattery(U8G2 u8g2, int x, int y, int w, int h, int segments, int lvl);
void drawPos(U8G2 u8g2, int x, int y, int rad, int deg);
void drawDetails(U8G2 u8g2, int x, int y, float angle, float tilt, float speed);

enum class OledView
{
    None,
    Robot,
    Log,
    Settings,
    Diagnostic
};

class Oled
{
public:
    OledView getOledView()
    {
        return this->view;
    }

private:
    OledView view;
};

static void registerView(Oled& view);

#endif // ARDUINO
#endif // MOUSR_OLED_H