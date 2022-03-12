/**
 * @file Settings.h
 * @author Adam Meltzer (11638244+theficus@users.noreply.github.com)
 * @brief Storage and retrieval of persisted settings
 * @version 0.1
 * @date 2022-03-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#ifndef MOUSR_SETTINGS_H
#define MOUSR_SETTINGS_H

#ifdef ARDUINO_ARCH_ESP32
#include <Preferences.h>
#else
#include "fakes.h"
#endif // ARDUINO_ARCH_ESP32

/**
 * @brief Settings class
 * 
 */
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

    /**
     * @brief Get the current volume level
     * 
     * @return uint8_t 
     */
    uint8_t getVolume();

    /**
     * @brief Set the desired volume level
     * 
     * @param v Value between 0-100 (percent)
     */
    void setVolume(uint8_t v);

    /**
     * @brief Get the current maximum speed
     * 
     * @return uint8_t 
     */
    uint8_t getMaxSpeed();

    /**
     * @brief Set the maximum speed
     * 
     * @param v Value between 0-100 (percent)
     */
    void setMaxSpeed(uint8_t v);

private:
    // Preference names
    const char *speedKey = "maxSpeed";
    const char *volumeKey = "volume";

    Preferences p;
};

#endif // MOUSR_SETTINGS_H