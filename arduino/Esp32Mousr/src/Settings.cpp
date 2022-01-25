#include "Settings.h"

uint8_t Settings::getVolume()
{
    uint8_t v = (uint8_t)this->p.getUChar(volumeKey);
    if (v > 100)
    {
        return 100;
    }

    return v;
}

void Settings::setVolume(uint8_t v)
{
    if (v > 100)
    {
        v = 100;
    }

    this->p.putUChar(volumeKey, v);
}

uint8_t Settings::getMaxSpeed()
{
    uint8_t v = (uint8_t)this->p.getUChar(speedKey);
    if (v > 100)
    {
        return 100;
    }

    return v;
}

void Settings::setMaxSpeed(uint8_t v)
{
    if (v > 100)
    {
        v = 100;
    }

    this->p.putUChar(speedKey, v);
}