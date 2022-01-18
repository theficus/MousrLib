#pragma once
#ifndef MOUSR_CONSOLELOG_H
#define MOUSR_CONSOLELOG_H

#include <string>
#include "Log.h"

using namespace std;

class ConsoleLog_ : public Log
{
public:
    ConsoleLog_(string dest)
    {
        this->destName = dest;
    }

    void WriteLog(const char *str)
    {
        SAVE_LAST_LOG(str);
        printf("[%s] %s", destName.c_str(), str);
    }

private:
    string destName;
};

static ConsoleLog_ SerialLog = ConsoleLog_("Serial");
static ConsoleLog_ OledLog = ConsoleLog_("Oled");
static ConsoleLog_ ConsoleLog = SerialLog; // Default
#endif