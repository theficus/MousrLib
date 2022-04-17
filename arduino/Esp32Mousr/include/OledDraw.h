/**
 * @file OledDraw.h
 * @author Adam Meltzer (11638244+theficus@users.noreply.github.com)
 * @brief Functions for handling drawing well-defined things to the OLED display
 * @version 0.1
 * @date 2022-03-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _MOUSR_OLED_DRAW_H
#define _MOUSR_OLED_DRAW_H

#include "Oled.h"

#define OLED_DEBUG_DUMP_OUTPUT_DESTINATION Serial
//#define OLED_DEBUG_DUMP

// Functions used for drawing known things to the OLED display
void drawBattery(U8G2 u8g2, int x, int y, int w, int h, int segments, int lvl);
void drawPos(U8G2 u8g2, int x, int y, int rad, int deg, bool ctr = false);
void drawDetails(U8G2 u8g2, int x, int y, float angle, float tilt, float speed);
void drawPosPtr(U8G2 u8g2, int x, int y);
void drawMode(U8G2 u8g2, OledMessage message);
void drawButtons(U8G2 u8g2, ButtonState buttons);
void drawAnalogStick(U8G2 u8g2, AnalogStickMovement m);
void dumpBuffer(U8G2 u8g2);
void drawMousrPosition(U8G2 u8g2, MousrMovementMsg m);

#endif // _MOUSR_OLED_DRAW_H
