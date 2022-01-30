#pragma once
#ifndef MOUSR_CONTROLLER_H
#define MOUSR_CONTROLLER_H

#include "common.h"

#ifdef ARDUINO
#include <Wire.h>

// seesaw includes
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Adafruit_seesaw.h>

// Seesaw
static Adafruit_seesaw ss;
#define BUTTON_RIGHT 6
#define BUTTON_DOWN 7
#define BUTTON_LEFT 9
#define BUTTON_UP 10
#define BUTTON_SEL 14
#define JOYSTICK_H 2
#define JOYSTICK_V 3

// Calibration values
#define JOYSTICK_H_CORRECTION 2
#define JOYSTICK_V_CORRECTION 0

#define JOY_IRQ_PIN 14 // ESP32

bool setupSeesaw();
void onButtonPress();
void readSeesaw();

#endif // ARDUINO
#endif // MOUSR_CONTROLLER_H