#ifndef ARDUINO
#include "ConsoleLog.h"
#else
#include "SerialLog.h"
#include "OledLog.h"
#endif

#include <unity.h>

using namespace std;

#define consttxt(txt) const_cast<char*>(txt)
#define constchar(c) const_cast<char*>(c)

void test_writeLogLn()
{
    const char* txt = "writeLogLn()";
    const char* expected = "writeLogLn()";
    writeLogLn(LogDestination::Serial, txt);
    TEST_ASSERT_EQUAL_STRING(expected, SerialLog.getLastLogEntry());

    writeLogLn(LogDestination::Serial);
    TEST_ASSERT_EQUAL_STRING("\n", SerialLog.getLastLogEntry());

    writeLogLn(LogDestination::Oled, txt);
    TEST_ASSERT_EQUAL_STRING(expected, OledLog.getLastLogEntry());

    writeLogLn(LogDestination::Oled);
    TEST_ASSERT_EQUAL_STRING("\n", OledLog.getLastLogEntry());
}

void test_writeLogF()
{
    const char* expected = "writeLogF() text without formatting\n";
    writeLogF(LogDestination::Serial, expected);
    TEST_ASSERT_EQUAL_STRING(expected, SerialLog.getLastLogEntry());
    writeLogF(LogDestination::Oled, expected);
    TEST_ASSERT_EQUAL_STRING(expected, OledLog.getLastLogEntry());

    expected = "writeLogF() text with formatting: test -123 0.123000\n";
    writeLogF(LogDestination::Serial, consttxt("writeLogF() text with formatting: %s %d %f\n"), "test", -123, 0.123);
    TEST_ASSERT_EQUAL_STRING(expected, SerialLog.getLastLogEntry());
    writeLogF(LogDestination::Oled, consttxt("writeLogF() text with formatting: %s %d %f\n"), "test", -123, 0.123);
    TEST_ASSERT_EQUAL_STRING(expected, OledLog.getLastLogEntry());
}

void test_writeLog()
{
    const char* expected = "writeLog()\n";
    writeLog(LogDestination::Serial, expected);
    TEST_ASSERT_EQUAL_STRING(expected, SerialLog.getLastLogEntry());

    writeLog(LogDestination::Oled, expected);
    TEST_ASSERT_EQUAL_STRING(expected, OledLog.getLastLogEntry());
}