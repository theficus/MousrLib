// Fakes for mocking out functionality when not using an embedded device
#ifndef _MOUSR_FAKES_H
#define _MOUSR_FAKES_H

#ifndef ARDUINO
#include <ArduinoFake.h>

class BLERemoteCharacteristic
{
public:
    void writeValue(uint8_t *, size_t)
    {
    }
};

#endif

#endif