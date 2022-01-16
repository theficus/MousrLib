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
    Console = Serial,
    All = Serial | Oled
};

inline LogDestination operator&(LogDestination a, LogDestination b)
{
    return static_cast<LogDestination>(static_cast<int>(a) & static_cast<int>(b));
}

void writeLogF(const LogDestination dest, const char *fmt, ...);
void writeLogLn(const LogDestination dest, const char *str);
void writeLog(const LogDestination dest, const char *str);
//void writeLog(Log dest, const char *str);

#endif