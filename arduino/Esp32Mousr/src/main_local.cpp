#include <cstdio>
#include "Log.h"
#include "ConsoleLog.h"

// Unit tests will have their own main() method
#ifndef UNIT_TEST

// Local testing only
int main()
{
    printf("Hello world!\n");
    return 0;
}

#endif