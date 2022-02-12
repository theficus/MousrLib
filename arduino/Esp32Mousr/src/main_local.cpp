#ifndef ARDUINO
#ifndef UNIT_TEST

#include <cstdio>

// Only include things we want to verify on native
#include "Mousr.h"
#include "Settings.h"

// Unit tests will have their own main() method

// Local testing only
int main()
{
    printf("Hello world!\n");

    float f1 = 0.2312536;
    float f2 = 0.0;
    float f3 = 93.02859;
    std::vector<uint8_t> data;
    MousrData::append(data, (uint8_t)MousrMessage::ROBOT_POSE);
    MousrData::append(data, f1);
    MousrData::append(data, f2);
    MousrData::append(data, f3);
    MousrData d(data.data(), data.size());
    s_printf("%s\n", d.toString().c_str());
    
    return 0;
}

#endif // !UNIT_TEST
#endif // !ARDUINO