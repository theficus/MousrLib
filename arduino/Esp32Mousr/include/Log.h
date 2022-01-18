#pragma once
#ifndef MOUSR_LOG_H
#define MOUSR_LOG_H

#include <cstdio>
#include <cstdarg>
#include <cinttypes>

class Log
{
public:
    virtual ~Log() {}
    virtual void WriteLog(const char *str) = 0;
};

enum class LogDestination : uint8_t
{
    None = 0,
    Serial = 1,
    Oled = 1 << 1,
    All = Serial | Oled
};

inline LogDestination operator&(LogDestination a, LogDestination b)
{
    return static_cast<LogDestination>(static_cast<int>(a) & static_cast<int>(b));
}

#define g_writeLogF(fmt, ...) writeLogF(LogDestination::All, fmt, __VA_ARGS__);
#define g_writeLogLn(str) writeLogLn(LogDestinaton::All, str);
#define g_writeLog(str) writeLog(LogDestinaton::All, str);

void writeLogF(const LogDestination dest, const char *fmt, ...);
void writeLogLn(const LogDestination dest);
void writeLogLn(const LogDestination dest, const char *str);
void writeLog(const LogDestination dest, const char *str);

#ifdef UNIT_TEST
static char* s_last_log_entry;
#define SAVE_LAST_LOG(str) s_last_log_entry = (char*)str;
#else
#define SAVE_LAST_LOG(str) {}
#endif

#endif