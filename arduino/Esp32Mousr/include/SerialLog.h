#pragma once
#ifndef MOUSR_SERIAL_LOG_H
#define MOUSR_SERIAL_LOG_H

#include <Arduino.h>
#include "Log.h"

class SerialLog_ : public Log
{
public:
    void WriteLog(const char *str)
    {
        SAVE_LAST_LOG(str);
        Serial.print(str);
    }
};

static SerialLog_ SerialLog = SerialLog_();

#endif