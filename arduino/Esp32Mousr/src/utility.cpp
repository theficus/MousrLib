#include "utility.h"

#ifdef ARDUINO_ARCH_ESP32

static SemaphoreHandle_t global_i2c_sem;
static StaticSemaphore_t staticSemBuffer;

bool runWireTask;
bool runOledDumpTask;

void pollWireTask(void *p)
{
    uint32_t delayMs = (uint32_t)&p;
    s_printf("Starting i2c debug diag task ... (delay: %ums)", delayMs);
    while (runWireTask == true)
    {
        printWireStatus();
        delay(delayMs);
    }

    s_println(F("Stopping i2c debug diag task"));
    vTaskDelete(NULL);
}

void printWireStatus()
{
    s_println(F("Wire status:"));
    s_println(F("--------------------------------"));
    s_printf("      Bus clock: %zu\n", Wire.getClock());
    s_printf("    Write error: %d\n", Wire.getWriteError());
    s_printf("Last error code: %d\n", Wire.lastError());
    s_printf("     Last error: %s\n", Wire.getErrorText(Wire.lastError()));
}

void startWireDebugTask(uint32_t delayMs)
{
    if (runWireTask == true)
    {
        return;
    }

    runWireTask = xTaskCreate(pollWireTask, "__debugPollWireTask", 10000, &delayMs, 100, NULL);
}

void stopWireDebugTask()
{
    runWireTask = false;
}

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