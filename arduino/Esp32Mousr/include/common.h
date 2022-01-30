#pragma once
#ifndef MOUSR_COMMON_H
#define MOUSR_COMMON_H

#ifdef ARDUINO
#include <Arduino.h>
#define s_printf(fmt, ...) Serial.printf(fmt, __VA_ARGS__);
#define s_println(str) Serial.println(str);
#define s_print(str) Serial.print(str);
#define s_printlnb(str, base) Serial.println(str, base);

//#define println(str) HardwareSerial.println(str);
#else
#include <ArduinoFake.h>
#define s_printf(fmt, ...) printf(fmt, __VA_ARGS__);
#define s_println(str) { printf(str); printf("\n"); }
#define s_print(str) printf(str);
#endif // ARDUINO

#endif // MOUSR_COMMON_H