#ifdef ARDUINO

#include <unity.h>
#include "tests.h"

int main(int argc, char** argv)
{
    UNITY_BEGIN();

    /*
    // Mousr data
    test_ParseMessage();
    test_toHexString();
    test_mousrAlloc();
    test_getRawData();
    test_convertToBytes();
    test_createFromRaw();
    test_connectionStatusMap();
    
    // Settings
    test_writeUChar();
    */

    UNITY_END();

    return 0;
}

#endif // ARDUINO