#ifndef ARDUINO

#include <cstdio>

// Only include things we want to verify on native
#include "Mousr.h"
#include "Settings.h"

// Unit tests will have their own main() method

// Local testing only
int main()
{
    printf("Hello world!\n");
    return 0;
}

#endif // !ARDUINO