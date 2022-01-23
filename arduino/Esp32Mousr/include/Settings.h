#ifndef _MOUSR_SETTINGS_H
#define _MOUSR_SETTINGS_H

#ifdef ARDUINO
#include <Preferences.h>
#else
#include "fakes.h"
#endif

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

    uint8_t getVolume()
    {
        uint8_t v = (uint8_t)p.getUChar(volumeKey);
        if (v > 100)
        {
            return 100;
        }

        return v;
    }

    void setVolume(uint8_t v)
    {
        if (v > 100)
        {
            v = 100;
        }

        p.putUChar(volumeKey, v);
    }

    uint8_t getMaxSpeed()
    {
        uint8_t v = (uint8_t)p.getUChar(speedKey);
        if (v > 100)
        {
            return 100;
        }

        return v;
    }

    void setMaxSpeed(uint8_t v)
    {
        if (v > 100)
        {
            v = 100;
        }

        p.putUChar(speedKey, v);
    }

private:
    // Preference names
    const char *speedKey = "maxSpeed";
    const char *volumeKey = "volume";

    Preferences p;
};

#endif