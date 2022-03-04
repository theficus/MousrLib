// Fakes for mocking out functionality when not using an embedded device
#pragma once
#ifndef MOUSR_FAKES_H
#define MOUSR_FAKES_H

#include "logging.h"

#ifndef ARDUINO

#include <iostream>
#include <map>
#include <any>

#define uchar unsigned char
#define BaseType_t int
#define xTaskCreate(...) 1
#define pdPASS 1
#define QueueHandle_t int
#define SemaphoreHandle_t int
#define F(str) #str
#define FALLING 0
#define INPUT 0
#define IRAM_ATTR
#define vTaskDelete(...)
#define xSemaphoreGiveFromISR(...) 1
#define uxQueueSpacesAvailable(...) (size_t)1
#define xQueueSendToFront(...) 1
#define xSemaphoreCreateBinary() 1
#define TickType_t uint32_t
#define u8g2_font_courR08_tf 0
#define u8g2_font_courR12_tf 0
#define u8g2_font_profont10_mf 0
#define u8g2_font_9x15_t_symbols 0
#define xQueueReceive(...) true
#define portMAX_DELAY 0
#define U8G2_SH1106_128X64_NONAME_F_HW_I2C U8G2
#define U8G2_R0 0
#define xQueueCreate(...) 1
#define OLED_DEBUG_DUMP_OUTPUT_DESTINATION

#define Wire = Wire_()
class Wire_
{
public:
    Wire_(...) {}
    int getClock() { return 100000; }
};

class U8G2
{
public:
    U8G2(...) {}
    void drawGlyph(int x, int y, int glyph)
    {
        s_printf("[OLED->drawGlyph] x=%d y=%d glyph=%d (%x)\n", x, y, glyph, glyph);
    }

    void writeBufferXBM(...)
    {
        s_println("OLED->writeBufferXBM]");
    }

    bool begin()
    {
        s_println("[OLED->begin]");
        return true;
    }

    void clearBuffer() { s_println("[OLED->clearBuffer]"); }
    void setFont(...) { s_println("[OLED->setFont]"); }
    void sendBuffer() { s_println("[OLED->sendBuffer]"); }
    int getBusClock() { return 100000; }

    void drawLine(int x, int y, int w, int h)
    {
        s_printf("[OLED->drawLine] x=%d y=%d w=%d h=%d\n", x, y, w, h);
    }

    void drawDisc(int x, int y, int rad)
    {
        s_printf("[OLED->drawDisc] x=%d y=%d rad=%d\n", x, y, rad);
    }

    void drawCircle(int x, int y, float rad)
    {
        s_printf("[OLED->drawCircle] x=%d y=%d rad=%f\n", x, y, rad);
    }

    void drawFrame(int x, int y, int w, int h)
    {
        s_printf("[OLED->drawFrame] x=%d y=%d w=%d h=%d\n", x, y, w, h);
    }

    void drawBox(int x, int y, int w, int h)
    {
        s_printf("[OLED->drawBox] x=%d y=%d w=%d h=%d\n", x, y, w, h);
    }

    void drawStr(int x, int y, char *buf)
    {
        s_printf("[OLED->drawStr] x=%d y=%d str=%s\n", x, y, buf);
    }
};

class Adafruit_seesaw
{
public:
    Adafruit_seesaw() {}
    uint32_t digitalReadBulk(...)
    {
        return 0xffffffff;
    }
};

class BLERemoteCharacteristic
{
public:
    void writeValue(uint8_t *, size_t)
    {
    }
};

// Mock out the ESP32 Preferences library to just store things in memory
class MockPreferences
{
public:
    MockPreferences()
    {
    }

    ~MockPreferences()
    {
    }

    uchar getUChar(const char *key)
    {
        return getValue<uchar>(key);
    }

    void putUChar(const char *key, unsigned char value)
    {
        valueStore[key] = value;
    }

    void begin(const char *name)
    {
    }

    void end()
    {
    }

    template <typename T>
    T getValue(const char *name)
    {
        std::any v = valueStore[name];
        auto retVal = std::any_cast<T>(v);
        s_printf("Got value '%s' from store.\n", name);
        std::cout << "Actual value: " << retVal << "\n";
        return retVal;
    }

private:
    std::map<const char *, std::any> valueStore;
};

#define Preferences MockPreferences

#endif // !ARDUINO
#endif // MOUSR_FAKES_H