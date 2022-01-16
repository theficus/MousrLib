#include <stdio.h>
#include <cinttypes>

#include <U8g2lib.h>
#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
void setupOledLogDisplay(U8X8 oled);

enum class LogDestination : uint8_t {
    None = 0,
    Serial = 1,
    Oled = 1 << 1,
    Console = 1 << 2,
    All = Serial | Oled | Console
};

static bool isOledLogEnabled = false;
static bool isSeriaLogEnabled = false;

inline LogDestination operator&(LogDestination a, LogDestination b)
{
    return static_cast<LogDestination>(static_cast<int>(a) & static_cast<int>(b));
}

void writeLogF(const LogDestination dest, const char* fmt, ...);
void writeLogLn(const LogDestination dest, const char* str);
void writeLog(const LogDestination dest, const char* str);
