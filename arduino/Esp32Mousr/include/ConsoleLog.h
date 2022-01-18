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
        setLastLogEntry(str);
        printf("[%s] %s", destName.c_str(), str);
    }

    char* getLastLogEntry();

private:
    string destName;
    void setLastLogEntry(const char *str);
};

static ConsoleLog_ SerialLog = ConsoleLog_("Serial");
static ConsoleLog_ OledLog = ConsoleLog_("Oled");
static ConsoleLog_ ConsoleLog = SerialLog; // Default
#endif