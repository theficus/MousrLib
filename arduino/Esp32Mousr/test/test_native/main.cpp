#include <unity.h>
#include "../MousrTests.h"
#include "../UtilityTests.h"

int main(int argc, char** argv)
{
    UNITY_BEGIN();

    //runMousrTests();
    runUtilityTests();

    UNITY_END();

    return 0;
}