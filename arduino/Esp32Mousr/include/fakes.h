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

    uchar getUChar(const char* key)
    {
        return getValue<uchar>(key);
    }

    void putUChar(const char* key, unsigned char value)
    {
        valueStore[key] = value;
    }

    void begin(const char* name)
    {
    }

    void end()
    {
    }

    template <typename T>
    T getValue(const char* name)
    {
        std::any v = valueStore[name];
        auto retVal = std::any_cast<T>(v);
        s_printf("Got value '%s' from store.\n", name);
        std::cout << "Actual value: " << retVal << "\n";
        return retVal;
    }

private:
    std::map<const char*, std::any> valueStore;
};

#define Preferences MockPreferences

#endif // !ARDUINO
#endif // MOUSR_FAKES_H