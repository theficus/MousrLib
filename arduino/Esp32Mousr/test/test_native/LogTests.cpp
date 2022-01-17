#include <ConsoleLog.h>
#include <unity.h>

#define ConsoleLog
using namespace std;

void test_writeLogLn()
{
    const char* txt = "writeLogLn()";
    string expected = string(txt) + "\n";
    writeLogLn(LogDestination::Console, txt);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), SerialLog.lastLogEntry.c_str());
}

void test_writeLogF()
{
    string expected = "writeLogF() text without formatting\n";
    writeLogF(LogDestination::Console, expected.c_str());
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), SerialLog.lastLogEntry.c_str());
    writeLogF(LogDestination::Console, "writeLogF() text with formatting: %s %d %f\n", "test", -123, 0.123);
}

void test_writeLog()
{
    writeLog(LogDestination::Console, "writeLog()\n");
}