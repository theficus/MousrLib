#pragma once
#ifndef MOUSR_UTILITY_H
#define MOUSR_UTILITY_H

#include <cstdint>

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
#include "logging.h"

#define semTake(sem) xSemaphoreTake(sem, portMAX_DELAY);
#define semGive(sem) xSemaphoreGive(sem);
#define semWait(sem) \
    semTake(sem);    \
    semGive(sem);

static SemaphoreHandle_t global_i2c_sem;
static StaticSemaphore_t staticSemBuffer;
#define i2cSemTake() semTake(global_i2c_sem);
#define i2cSemGive() semGive(global_i2c_sem);

static void i2cSemInit()
{
    global_i2c_sem = xSemaphoreCreateBinaryStatic(&staticSemBuffer);
    i2cSemGive();
}

static void logMemory()
{
    uint32_t total = ESP.getHeapSize();
    uint32_t used = ESP.getFreeHeap();
    uint32_t avail = total - used;
    s_printf("Heap usage: %u/%u (%f%%)\n", avail, total, (avail / total) * 100.0);
}

/*
static void semTake(SemaphoreHandle_t waitHandle)
{
    if (waitHandle == nullptr)
    {
        return;
    }

    Serial.println("Waiting for semaphore...");
    xSemaphoreTake(waitHandle, portMAX_DELAY);
    Serial.println("Semaphore get!");
}

static void semGive(SemaphoreHandle_t waitHandle)
{
    if (waitHandle == nullptr)
    {
        return;
    }
    Serial.println("Releasing semaphore...");
    xSemaphoreGive(waitHandle);

}
*/

#else // ARDUINO_ARCH_ESP32
static void logMemory() {}
#endif
#endif // MOUSR_UTILITY_H