#include <unity.h>

// Log tests
void test_writeLogLn();
void test_writeLogF();
void test_writeLog();

// Mousr tests
void test_ParseMessage();

void runAll()
{
    UNITY_BEGIN();

    test_writeLog();
    test_writeLogF();
    test_writeLogLn();

    test_ParseMessage();

    UNITY_END();
}