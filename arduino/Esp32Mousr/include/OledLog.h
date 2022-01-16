#pragma once
#ifndef MOUSR_OLED_LOG_H
#define MOUSR_OLED_LOG_H

#include "Log.h"
#include <U8g2lib.h>
#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8

static U8X8 s_u8x8_log = U8X8_SSD1306_128X64_NONAME_HW_I2C(U8X8_PIN_NONE); // Log mode
static U8X8LOG s_u8x8_log_writer;
static uint8_t s_u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];

class OledLog_ : public Log
{
public:
    void WriteLog(const char *str)
    {
        s_u8x8_log_writer.print(str);
    }

    void SetupOledLogDisplay()
    {
        s_u8x8_log_writer.begin(s_u8x8_log, U8LOG_HEIGHT, U8LOG_WIDTH, s_u8log_buffer);
        s_u8x8_log_writer.setRedrawMode(0);
        this->WriteLog("hello world\n");
    }
};

static OledLog_ OledLog = OledLog_();

#endif