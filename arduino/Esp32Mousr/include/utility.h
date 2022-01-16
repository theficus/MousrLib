#pragma once
#ifndef MOUSR_UTILITY_H

#include <cstdint>

#ifdef ESP32

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

#else
static void semTake() { }
static void semGive() { }
#endif

#endif