#include <cstdint>

static SemaphoreHandle_t waitHandle = xSemaphoreCreateRecursiveMutex();

static void semTake()
{
    Serial.println("Waiting for semaphore...");
    xSemaphoreTake(waitHandle, portMAX_DELAY);
    Serial.println("Semaphore get!");
}

static void semGive()
{
    Serial.println("Releasing semaphore...");
    xSemaphoreGive(waitHandle);
}