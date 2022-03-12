/**
 * @file utility.h
 * @author Adam Meltzer (11638244+theficus@users.noreply.github.com)
 * @brief Various utility functions
 * @version 0.1
 * @date 2022-03-03
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#ifndef MOUSR_UTILITY_H
#define MOUSR_UTILITY_H

#ifndef ARDUINO
#include "fakes.h"
#endif

#include <cstdint>
#include "logging.h"

/**
 * @brief Zero out memory
 *
 */
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
#endif

/**
 * @brief Global i2c semaphore take
 *
 * @param timeout
 * @return true
 * @return false
 */
bool i2cSemTake(TickType_t timeout);

/**
 * @brief Global i2c semaphore give
 *
 * @remark Does nothing when not on ESP32
 * @return true
 * @return false
 */
bool i2cSemGive();

/**
 * @brief Global i2c semaphore init
 * @remark Does nothing when not on ESP32
 * @return true
 * @return false
 */
bool i2cSemInit();

void logMemory();

/**
 * @brief Start Wire library debug task
 * @remark Does nothing when not on ESP32
 * @param delayMs Delay period for the task timer in milliseconds
 */
void startWireDebugTask(uint32_t delayMs);

/**
 * @brief Stop Wire library debug task
 * @remark Does nothing when not on ESP32
 */
void stopWireDebugTask();

/**
 * @brief Print Wire status
 * @remark Does nothing when not on ESP32
 */
void printWireStatus();

/**
 * @brief Takes a semaphore with a timeout
 * @remark Does nothing when not on ESP32
 * @return true Semaphore take successful
 * @return false Semaphore take unsuccessful (timeout)
 */
#define semTakeWithTimeout(__sem, __timeout) xSemaphoreTake(__sem, __timeout)

/**
 * @brief Takes a semaphore with no timeout
 * @remark Does nothing when not on ESP32
 * @returns boolean in theory, but will block so using a return value isn't really beneficial
 */
#define semTake(__sem) semTakeWithTimeout(__sem, portMAX_DELAY)

/**
 * @brief Gives a semaphore
 * @remark Does nothing when not on ESP32
 * @return true if successful
 * @return false if unsuccessful
 */
#define semGive(__sem) xSemaphoreGive(__sem)

/**
 * @brief Waits and immediately gives a semaphore
 * @remark Does nothing when not on ESP32
 */
#define semWait(__sem)  \
    {                   \
        semTake(__sem); \
        semGive(__sem); \
    }

/**
 * @brief Runs a function gated by a semaphore
 * @remark Does nothing when not on ESP32
 * 
 */
#define semCritSec(__sem, __func) \
    {                             \
        semTake(__sem);           \
        __func;                   \
        semGive(__sem);           \
    }

/**
 * @brief Simple macro to run a method and check the result. Requires a function that returns a boolean value.
 * 
 */
#define check(__func, __expr)                                         \
    {                                                                 \
        auto __res = __func;                                          \
        if (__res != __expr)                                          \
        {                                                             \
            s_println("failed check: " #__func " expected " #__expr); \
            return false;                                             \
        }                                                             \
    }

/**
 * @brief Macro to run an expression and log its result
 * 
 */
#define logResult(__expr)     \
    {                         \
        auto __res = __expr;  \
        s_print(#__expr);     \
        s_print(" result: "); \
        s_println(__res);     \
    }

#define checkTrue(__func) check(__func, true);

/**
 * @brief Runs an i2c function within a critical section to block other i2c operations from occurring
 * @remark Does nothing when not on ESP32
 */
#define i2cSemCritSec(__func)      \
    {                              \
        i2cSemTake(portMAX_DELAY); \
        __func;                    \
        i2cSemGive();              \
    }

/**
 * @brief Runs an i2c function within a critical section to block other i2c operations from occurring and assign the function value to a variable
 * @remark Does nothing when not on ESP32
 */
#define i2cSemCritSecGetValue(__func, __val) \
    {                                        \
        i2cSemTake(portMAX_DELAY);           \
        __val = __func;                      \
        i2cSemGive();                        \
    }

/**
 * @brief Debug logging for memory usage
 * 
 */
void logMemory();
#endif // MOUSR_UTILITY_H