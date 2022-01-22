#include <unity.h>
#include "tests.h"

int main(int argc, char** argv)
{
    UNITY_BEGIN();

    // Logging
    test_writeLog();
    test_writeLogF();
    test_writeLogLn();

    // Mousr data
    test_ParseMessage();
    test_toHexString();
    test_mousrAlloc();
    test_getRawData();
    test_convertToBytes();
    test_createFromRaw();

    UNITY_END();

    return 0;
}