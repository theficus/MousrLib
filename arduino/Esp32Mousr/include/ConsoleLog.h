#pragma once
#ifndef MOUSR_CONSOLELOG_H
#define MOUSR_CONSOLELOG_H

#include "Log.h"

class ConsoleLog_ : virtual public Log
{
public:
    ConsoleLog_(char *dest)
    {
        this->destName = dest;
    }

    void WriteLog(const char* str)
    {
        printf("[%s] %s", destName, str);
    }

private:
    char *destName;
};

static ConsoleLog_ SerialLog = ConsoleLog_("Serial");
static ConsoleLog_ OledLog = ConsoleLog_("Oled");

#endif