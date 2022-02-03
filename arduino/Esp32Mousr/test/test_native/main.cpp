#include <unity.h>
#include "../MousrTests.h"

int main(int argc, char** argv)
{
    UNITY_BEGIN();

    runMousrTests();

    UNITY_END();

    return 0;
}