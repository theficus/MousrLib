#include <unity.h>
#include "tests.h"

int main(int argc, char** argv)
{
    UNITY_BEGIN();

    test_writeLog();
    test_writeLogF();
    test_writeLogLn();

    test_ParseMessage();
    test_toHexString();
    test_mousrAlloc();

    UNITY_END();

    return 0;
}