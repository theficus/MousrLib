#pragma once
#ifndef MOUSR_SETTINGS_H
#define MOUSR_SETTINGS_H

#ifdef ARDUINO_ARCH_ESP32
#include <Preferences.h>
#else
#include "fakes.h"
#endif // ARDUINO_ARCH_ESP32

#include "Log.h"

class Settings
{
public:
    Settings()
    {
        p.begin("MousrSettings");
    }

    ~Settings()
    {
        p.end();
    }

    uint8_t getVolume();
    void setVolume(uint8_t v);
    uint8_t getMaxSpeed();
    void setMaxSpeed(uint8_t v);

private:
    // Preference names
    const char *speedKey = "maxSpeed";
    const char *volumeKey = "volume";

    Preferences p;
};

#endif // MOUSR_SETTINGS_H