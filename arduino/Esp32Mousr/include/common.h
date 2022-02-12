#pragma once
#ifndef MOUSR_COMMON_H
#define MOUSR_COMMON_H

#include <cstring>
#include <cstdio>

#define clearmem(v, sz) memset(v, 0, sz);

#ifdef ARDUINO
#include <Arduino.h>
#define s_printf(fmt, ...) Serial.printf(fmt, __VA_ARGS__);
#define s_println(str) Serial.println(str);
#define s_print(str) Serial.print(str);
#define s_printlnb(str, base) Serial.println(str, base);
#else
//#include <ArduinoFake.h>
#define s_printf(fmt, ...) printf(fmt, __VA_ARGS__);
#define s_println(str) { printf(str); printf("\n"); }
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