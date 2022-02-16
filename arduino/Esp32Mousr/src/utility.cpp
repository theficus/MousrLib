#include "utility.h"

#ifdef ARDUINO_ARCH_ESP32

static SemaphoreHandle_t global_i2c_sem;
static StaticSemaphore_t staticSemBuffer;

bool i2cSemInit()
{
    global_i2c_sem = xSemaphoreCreateBinaryStatic(&staticSemBuffer);
    return __i2cSemGive();
}

bool __i2cSemTake(TickType_t timeout)
{
    return semTakeWithTimeout(global_i2c_sem, timeout);
}

bool __i2cSemGive()
{
    return semGive(global_i2c_sem);
}

void logMemory()
{
    uint32_t total = ESP.getHeapSize();
    uint32_t used = ESP.getFreeHeap();
    uint32_t avail = total - used;
    s_printf("Heap usage: %u/%u (%f%%)\n", avail, total, (avail / total) * 100.0);
}

#else

void logMemory()
{
}

#endif // ARDUINO_ARCH_ESP32