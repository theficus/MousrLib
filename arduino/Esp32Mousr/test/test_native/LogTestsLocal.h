#include "../LogTests.h"

void test_writeLogLn()
{
    writeLogLn(LogDestination::Console, "writeLogLn()");
}

void test_writeLogF()
{
    writeLogF(LogDestination::Console, "writeLogF() text without formatting\n");
    writeLogF(LogDestination::Console, "writeLogF() text with formatting: %s %d %f\n", "test", -123, 0.123);
}

void test_writeLog()
{
    writeLog(LogDestination::Console, "writeLog()\n");
}