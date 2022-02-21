#pragma once
#ifndef MOUSR_UTILITY_H
#define MOUSR_UTILITY_H

#include <cstdint>
#include "logging.h"

#define clearmem(v, sz) memset(v, 0, sz);
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)             \
    (byte & 0x80000000 ? '1' : '0'),     \
        (byte & 0x40000000 ? '1' : '0'), \
        (byte & 0x20000000 ? '1' : '0'), \
        (byte & 0x10000000 ? '1' : '0'), \
        (byte & 0x8000000 ? '1' : '0'),  \
        (byte & 0x4000000 ? '1' : '0'),  \
        (byte & 0x2000000 ? '1' : '0'),  \
        (byte & 0x1000000 ? '1' : '0'),  \
        (byte & 0x800000 ? '1' : '0'),   \
        (byte & 0x400000 ? '1' : '0'),   \
        (byte & 0x200000 ? '1' : '0'),   \
        (byte & 0x100000 ? '1' : '0'),   \
        (byte & 0x80000 ? '1' : '0'),    \
        (byte & 0x40000 ? '1' : '0'),    \
        (byte & 0x20000 ? '1' : '0'),    \
        (byte & 0x10000 ? '1' : '0'),    \
        (byte & 0x8000 ? '1' : '0'),     \
        (byte & 0x4000 ? '1' : '0'),     \
        (byte & 0x2000 ? '1' : '0'),     \
        (byte & 0x1000 ? '1' : '0'),     \
        (byte & 0x800 ? '1' : '0'),      \
        (byte & 0x400 ? '1' : '0'),      \
        (byte & 0x200 ? '1' : '0'),      \
        (byte & 0x100 ? '1' : '0'),      \
        (byte & 0x80 ? '1' : '0'),       \
        (byte & 0x40 ? '1' : '0'),       \
        (byte & 0x20 ? '1' : '0'),       \
        (byte & 0x10 ? '1' : '0'),       \
        (byte & 0x08 ? '1' : '0'),       \
        (byte & 0x04 ? '1' : '0'),       \
        (byte & 0x02 ? '1' : '0'),       \
        (byte & 0x01 ? '1' : '0')

#ifdef ARDUINO_ARCH_ESP32
#include "Wire.h"

bool __i2cSemTake(TickType_t timeout);
bool __i2cSemGive();
bool i2cSemInit();
void logMemory();
void startWireDebugTask(uint32_t delayMs);
void stopWireDebugTask();
void printWireStatus();

// Implementation of shortcut for normal semaphore operations
#define semTakeWithTimeout(__sem, __timeout) xSemaphoreTake(__sem, __timeout)
#define semTake(__sem) semTakeWithTimeout(__sem, portMAX_DELAY)
#define semGive(__sem) xSemaphoreGive(__sem)
#define semWait(__sem) \
    semTake(__sem);    \
    semGive(__sem);

#define semCritSec(__sem, __func) \
    semTake(__sem);               \
    func;                         \
    semGive(__sem);

#define check(__func, __expr)                                         \
    {                                                                 \
        auto __res = __func;                                          \
        if (__res != __expr)                                          \
        {                                                             \
            s_println("failed check: " #__func " expected " #__expr); \
            return false;                                             \
        }                                                             \
    }

#define checkTrue(__func) check(__func, true);

#define i2cSemCritSec(__func)    \
    __i2cSemTake(portMAX_DELAY); \
    __func;                      \
    __i2cSemGive();

#else // ARDUINO_ARCH_ESP32
void logMemory();
#endif
#endif // MOUSR_UTILITY_H