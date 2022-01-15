#include <Log.h>

U8X8LOG u8x8log;
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];

void writeSerial(const char *str)
{
    Serial.print(str);
}

void writeOled(const char *str)
{
    u8x8log.print(str);
}

void setupOledLogDisplay(U8X8 display)
{
    u8x8log.begin(display, U8LOG_HEIGHT, U8LOG_WIDTH, u8log_buffer);
    u8x8log.setRedrawMode(0);
    u8x8log.println("hello world");
}

void writeLogF(const LogDestination dest, const char *fmt, ...)
{
    char *str;
    va_list args;
    va_start(args, fmt);
    vasprintf(&str, fmt, args);
    va_end(args);

    writeLog(dest, str);
}

void writeLogLn(const LogDestination dest, const char *str)
{
    writeLog(dest, str);
    writeLog(dest, "\n");
}

void writeLog(LogDestination dest, const char *str)
{
    if ((LogDestination::Serial & dest) == LogDestination::Serial)
    {
        writeSerial(str);
    }

    if ((LogDestination::Oled & dest) == LogDestination::Oled)
    {
        writeOled(str);
    }
}