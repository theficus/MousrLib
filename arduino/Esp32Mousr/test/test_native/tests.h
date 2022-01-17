#include <unity.h>

// Log tests
void test_writeLogLn();
void test_writeLogF();
void test_writeLog();

void runAll()
{
    UNITY_BEGIN();

    test_writeLog();
    test_writeLogF();
    test_writeLogLn();

    UNITY_END();
}