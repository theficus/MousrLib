/**
 * @file logging.h
 * @author Adam Meltzer (11638244+theficus@users.noreply.github.com)
 * @brief Simple logging helpers
 * @version 0.1
 * @date 2022-03-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#ifndef MOUSR_COMMON_H
#define MOUSR_COMMON_H

#include <cstring>
#include <cstdio>

#ifdef ARDUINO
#include <Arduino.h>
/**
 * @brief printf to Serial
 * 
 */
#define s_printf(fmt, ...) Serial.printf(fmt, __VA_ARGS__);

/**
 * @brief print to Serial with carriage return
 * 
 */
#define s_println(str) Serial.println(str);

/**
 * @brief print to Serial
 * 
 */
#define s_print(str) Serial.print(str);

/**
 * @brief print to Serial with base value and carriage return
 * 
 */
#define s_printlnb(str, base) Serial.println(str, base);
#else
#include <ArduinoFake.h>
/**
 * @brief printf to stdout
 * 
 */
#define s_printf(fmt, ...) printf(fmt, __VA_ARGS__);

/**
 * @brief print to stdout with carriage return
 * 
 */
#define s_println(str) { printf(str); printf("\n"); }

/**
 * @brief print to stdout
 * 
 */
#define s_print(str) printf(str);
#endif // ARDUINO

// Debug only macros
#if DEBUG
#define d_print(str) s_printf(str);
#define d_printf(fmt, ...) s_printf(fmt, __VA_ARGS__);
#define d_println(str) s_println(str);
#else
#define d_print(str)
#define d_printf(fmt, ...)
#define d_println(str)
#endif // DEBUG

#endif // MOUSR_COMMON_H