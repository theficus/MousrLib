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
    TEST_ASSERT_EQUAL_STRING(expected, s_last_log_entry);

    writeLogLn(LogDestination::Serial);
    TEST_ASSERT_EQUAL_STRING("\n", s_last_log_entry);

    writeLogLn(LogDestination::Oled, txt);
    TEST_ASSERT_EQUAL_STRING(expected, s_last_log_entry);

    writeLogLn(LogDestination::Oled);
    TEST_ASSERT_EQUAL_STRING("\n", s_last_log_entry);
}

void test_writeLogF()
{
    const char* expected = "[serial] writeLogF() text without formatting\n";
    writeLogF(LogDestination::Serial, expected);
    TEST_ASSERT_EQUAL_STRING(expected, s_last_log_entry);

    expected = "[oled] writeLogF() text without formatting\n";
    writeLogF(LogDestination::Oled, expected);
    TEST_ASSERT_EQUAL_STRING(expected, s_last_log_entry);

    /*
    expected = "[any] writeLogF() text without formatting\n";
    g_writeLogF(expected);
    TEST_ASSERT_EQUAL_STRING(expected, s_last_log_entry);
    */
    
    expected = "[serial] writeLogF() text with formatting: test -123 0.123000\n";
    writeLogF(LogDestination::Serial, consttxt("[serial] writeLogF() text with formatting: %s %d %f\n"), "test", -123, 0.123);
    TEST_ASSERT_EQUAL_STRING(expected, s_last_log_entry);

    expected = "[oled] writeLogF() text with formatting: test -123 0.123000\n";
    writeLogF(LogDestination::Oled, consttxt("[oled] writeLogF() text with formatting: %s %d %f\n"), "test", -123, 0.123);
    TEST_ASSERT_EQUAL_STRING(expected, s_last_log_entry);
}

void test_writeLog()
{
    const char* expected = "writeLog()\n";
    writeLog(LogDestination::Serial, expected);
    TEST_ASSERT_EQUAL_STRING(expected, s_last_log_entry);

    writeLog(LogDestination::Oled, expected);
    TEST_ASSERT_EQUAL_STRING(expected, s_last_log_entry);
}