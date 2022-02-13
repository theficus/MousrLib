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

// Implementation of shortcut for normal semaphore operations
#define __semTakeWithTimeout(__sem, __timeout) xSemaphoreTake(__sem, __timeout);
#define __semTake(__sem) __semTakeWithTimeout(__sem, portMAX_DELAY);
#define __semGive(__sem) xSemaphoreGive(__sem);

void i2cSemInit();
void logMemory();
bool __i2cSemTake(TickType_t timeout = portMAX_DELAY);
bool __i2cSemGive();

#ifdef DEBUG_SEM_OP
#define i2cSemTake() \
    s_printf("[%s] %s: Taking global i2c semaphore...\n", __TIME__, __FUNCTION__); \
    __i2cSemTake();
#define i2cSemGive() \
    s_printf("[%s] %s: Giving global i2c semaphore...\n", __TIME__, __FUNCTION__); \
    __i2cSemGive();

#define semTakeWithTimeout(__sem, __timeout) \
    s_printf("[%s] %s: Taking semaphore %p timeout %d\n", __TIME__, __FUNCTION__, &__sem, __timeout); \
    bool __take_result = __semTakeWithTimeout(__sem, __timeout); \
    s_printf("[%s] %s: Take semaphore %p result: %d\n", __TIME__, __FUNCTION__, &__sem, __take_result);
#define semTake(__sem) semTakeWithTimeout(__sem, portMAX_DELAY);
#define semGive(__sem) \
    s_printf("[%s] %s: Giving semaphore %p", __TIME__, __FUNCTION__, &__sem); \
    bool __give_result = __semGive(__sem); \
    s_printf("[%s] %s: Give semaphore %p result: %d\n", __TIME__, __FUNCTION__, &__sem, __give_result);
#else
#define semTakeWithTimeout(__sem, __timeout) __semTakeWithTimeout(__sem, __timeout);
#define semTake(__sem) __semTake(__sem);
#define semGive(__sem) __semGive(__sem);
#define i2cSemGive() __i2cSemGive();
#define i2cSemTake() __i2cSemTake();
#define i2cSemTakeWithTimeout(timeout) __i2cSemTake(timeout);
#endif

#define semWaitWithTimeout(__sem, __timeout) \
    semTakeWithTimeout(__sem, __timeout); \
    semGive(__sem);

#define semWait(__sem) \
    semTake(__sem);    \
    semGive(__sem);

#else // ARDUINO_ARCH_ESP32
void logMemory();
#endif
#endif // MOUSR_UTILITY_H