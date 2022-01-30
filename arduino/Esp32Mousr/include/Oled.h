#pragma once
#ifndef MOUSR_OLED_H
#define MOUSR_OLED_H
#ifdef ARDUINO

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <U8g2lib.h>

// Glyphs
#define ARROW_UP_FILL 0x25b2
#define ARROW_UP_EMPTY 0x25b3
#define ARROW_RIGHT_FILL 0x25b6
#define ARROW_RIGHT_EMPTY 0x25b7
#define ARROW_DOWN_FILL 0x25bc
#define ARROW_DOWN_EMPTY 0x25bd
#define ARROW_LEFT_FILL 0x25c0
#define ARROW_LEFT_EMPTY 0x25c1

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