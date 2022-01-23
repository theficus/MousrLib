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

    All = Serial | Oled,
    Console = Serial
};

// Allow binary or comparison
inline LogDestination operator&(LogDestination a, LogDestination b)
{
    return static_cast<LogDestination>(static_cast<int>(a) & static_cast<int>(b));
}

void writeLogF(const LogDestination dest, const char *fmt, ...);
void writeLogLn(const LogDestination dest);
void writeLogLn(const LogDestination dest, const char *str);
void writeLog(const LogDestination dest, const char *str);

// Shortcuts to log to all destinations
#define g_writeLogF(fmt, ...) writeLogF(LogDestination::All, fmt, __VA_ARGS__)
#define g_writeLogLn(str) writeLogLn(LogDestination::All, str)
#define g_writeLog(str) writeLog(LogDestination::All, str)

// Shortcuts to write serial log
#define s_writeLogF(fmt, ...) writeLogF(LogDestination::Serial, fmt, __VA_ARGS__)
#define s_writeLogLn(str) writeLogLn(LogDestination::Serial, str)
#define s_writeLog(str) writeLog(LogDestination::Serial, str)

// Shortcuts to write OLED log
#define o_writeLogF(fmt, ...) writeLogF(LogDestination::Oled, fmt, __VA_ARGS__)
#define o_writeLogLn(str) writeLogLn(LogDestination::Oled, str)
#define o_writeLog(str) writeLog(LogDestination::Oled, str)

//#if DEBUG
#define debugLogF(fmt, ...) s_writeLogF(fmt, __VA_ARGS__)
#define debugLogLn(str) s_writeLogLn(str)
#define debugLog(str) s_writeLog(str)
//#else
//#define debugLogF(fmt, ...)
//#define debugLogLn(str)
//#define debugLog(str)
//#endif

// Hooks for unit testing to validate proper logging API behavior
#ifdef UNIT_TEST
static char* s_last_log_entry;
#define SAVE_LAST_LOG(str) s_last_log_entry = (char*)str;
#else
#define SAVE_LAST_LOG(str) {}
#endif

#endif