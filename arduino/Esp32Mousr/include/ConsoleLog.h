#pragma once
#ifndef MOUSR_CONSOLELOG_H
#define MOUSR_CONSOLELOG_H
#ifndef ARDUINO

#include <string>
#include "Log.h"

using namespace std;

// Used for testing or for native (non-embedded) use
class ConsoleLog_ : public Log
{
public:
    ConsoleLog_(string dest)
    {
        this->destName = dest;
    }

    void writeLog(const char *str)
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
#endif // ARDUINO
#endif // MOUSR_CONSOLELOG_H