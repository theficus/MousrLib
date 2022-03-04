#pragma once
#ifndef MOUSR_OLED_H
#define MOUSR_OLED_H

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <math.h>
#include "logging.h"
#include "utility.h"
//#include "controller.h"
#include "analog.h"
#include "buttons.h"
#include "mousr.h"

#ifdef ARDUINO
#include <U8g2lib.h>
#else
#include "fakes.h"
#endif

#include "logging.h"
#include "utility.h"
#include "controller.h"

#ifdef _DO_BLE
#include "MousrBluetooth.h"
#endif

// Glyphs
#define ARROW_UP_FILL 0x25b2
#define ARROW_UP_EMPTY 0x25b3
#define ARROW_RIGHT_FILL 0x25b6
#define ARROW_RIGHT_EMPTY 0x25b7
#define ARROW_DOWN_FILL 0x25bc
#define ARROW_DOWN_EMPTY 0x25bd
#define ARROW_LEFT_FILL 0x25c0
#define ARROW_LEFT_EMPTY 0x25c1
#define SNOWMAN 0x2603
#define UMBRELLA 0x6202

// U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0);
// static U8G2 u8g2 = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0); // Graphical mode

enum class OledMessage
{
    None,
    Robot,
    Log,
    Settings,
    ControllerDiag,
    Diagnostic,
    Connecting,
};

enum class OledView
{
    None,
    Robot,
    Log,
    Settings,
    ControllerDiag,
    Connection
};

enum OledDisplayFlags : uint8_t
{
    None = 0,
    Dump = 1,
    Overlay = 1 << 1
};

void drawBattery(U8G2 u8g2, int x, int y, int w, int h, int segments, int lvl);
void drawPos(U8G2 u8g2, int x, int y, int rad, int deg, bool ctr = false);
void drawDetails(U8G2 u8g2, int x, int y, float angle, float tilt, float speed);
void drawPosPtr(U8G2 u8g2, int x, int y);
void drawMode(U8G2 u8g2, OledView mode);
void drawButtons(U8G2 u8g2, ButtonState buttons);

struct OledDisplayMessage
{
    OledMessage message;
    OledView view;
    union
    {
        AnalogStickMovement stickPos;
        MousrMovementMsg mousrMove;
    };

    union
    {
        ButtonState buttonPress;
        MousrBatteryMsg mousrBattery;
    };

    MousrConnectionStatus status;
    OledDisplayFlags flags;
};

/*
// Message to display OLED data
template<class M>
struct OledDisplayMessage
{
    OledViewKind viewKind;
    M message;
};
*/

class Oled
{
private:
    static Oled *singleton;
    Oled();

public:
    static Oled *getInstance()
    {
        if (singleton == NULL)
        {
            singleton = new Oled;
        }

        return singleton;
    }

    void queueMessage(OledDisplayMessage *message);
    bool begin();
    bool end();
    OledView getView();
    void setView(OledView view);
    void lockView();
    void unlockView();

    U8G2 u8g2; // TODO: Should probably hide this

private:
    static void displayQueueTask(void *);
    bool hasBegun = false;
    bool isFinalizing = false;
    OledView view = OledView::None;
    bool isLocked;
    QueueHandle_t displayQueue;
};

#endif // MOUSR_OLED_H