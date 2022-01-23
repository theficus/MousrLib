#include <cstdio>

// Only include things we want to verify on native
#include "Log.h"
#include "ConsoleLog.h"
#include "Mousr.h"
#include "Settings.h"

// Unit tests will have their own main() method
#ifndef UNIT_TEST

// Local testing only
int main()
{
    printf("Hello world!\n");
    return 0;
}

#endif