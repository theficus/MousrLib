#include "Log.h"

#ifdef ARDUINO
#include "SerialLog.h"
#include "OledLog.h"
#else
#include "ConsoleLog.h"
#endif

void writeLogF(const LogDestination dest, const char *fmt, ...)
{
    char *str;
    va_list args;
    va_start(args, fmt);
    vasprintf(&str, fmt, args);
    va_end(args);

    writeLog(dest, str);
}

void writeLogLn(const LogDestination dest)
{
    writeLog(dest, "\n");
}

void writeLogLn(const LogDestination dest, const char *str)
{
    writeLog(dest, str);
}

void writeLog(LogDestination dest, const char *str)
{
    if ((LogDestination::Serial & dest) == LogDestination::Serial)
    {
        SerialLog.WriteLog(str);
    }

    if ((LogDestination::Oled & dest) == LogDestination::Oled)
    {
        OledLog.WriteLog(str);
    }
}

#ifndef ARDUINO
static char* s_lastLogEntry;

char* ConsoleLog_::getLastLogEntry()
{
    return s_lastLogEntry;
}

void ConsoleLog_::setLastLogEntry(const char* str)
{
    s_lastLogEntry = (char*)malloc(sizeof(str));
    strcpy(s_lastLogEntry, str);
}
#endif