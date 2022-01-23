#pragma once
#ifndef MOUSR_UTILITY_H

#include <cstdint>

#ifdef ESP32

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

#else
static void semTake() { }
static void semGive() { }
#endif

#endif