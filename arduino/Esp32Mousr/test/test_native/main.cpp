#include <unity.h>
#include "LogTestsLocal.h"

int main(int argc, char** argv)
{
    LogTests logTests;

    UNITY_BEGIN();
    logTests.RunAll();
    UNITY_END();
}