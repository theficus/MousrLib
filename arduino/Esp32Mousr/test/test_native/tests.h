#include <unity.h>

// Log tests
void test_writeLogLn();
void test_writeLogF();
void test_writeLog();

// Mousr tests
void test_ParseMessage();
void test_toHexString();
void test_mousrAlloc();

void runAll()
{
    UNITY_BEGIN();

    test_writeLog();
    test_writeLogF();
    test_writeLogLn();

    test_ParseMessage();
    test_toHexString();
    test_mousrAlloc();

    UNITY_END();
}