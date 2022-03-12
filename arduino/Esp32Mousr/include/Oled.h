/**
 * @file Oled.h
 * @author Adam Meltzer (11638244+theficus@users.noreply.github.com)
 * @brief OLED operations and classes
 * @version 0.1
 * @date 2022-03-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#ifndef MOUSR_OLED_H
#define MOUSR_OLED_H

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <cmath>
#include <algorithm>
#include "logging.h"
#include "utility.h"
//#include "controller.h"
#include "analog.h"
#include "buttons.h"
#include "Mousr.h"

#ifdef ARDUINO
#include <U8g2lib.h>
// Should match the U8G2 constructor for the OLED display being used
#define U8G2CTOR U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0);

#else
#define U8G2CTOR
#include "fakes.h"
#endif // ARDUINO

#include "logging.h"
#include "utility.h"
#include "controller.h"

#ifdef _DO_BLE
#include "MousrBluetooth.h"
#endif // _DO_BLE

// Frequently used glyphs
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

/**
 * @brief OLED message kind
 * 
 */
enum class OledMessage
{
    /**
     * @brief None (ignored)
     * 
     */
    None,

    /**
     * @brief Robot status message
     * 
     */
    Robot,

    /**
     * @brief Logging output message
     * 
     */
    Log,

    /**
     * @brief Settings message
     * 
     */
    Settings,

    /**
     * @brief Controller diagnostic message
     * 
     */
    ControllerDiag,

    /**
     * @brief General diagnostic message
     * 
     */
    Diagnostic,

    /**
     * @brief Connection status message
     * 
     */
    Connecting,
};

/**
 * @brief Intended OLED view
 * 
 */
enum class OledView
{
    /**
     * @brief None (ignored)
     * 
     */
    None,

    /**
     * @brief Robot status view
     * 
     */
    Robot,

    /**
     * @brief Log view
     * 
     */
    Log,

    /**
     * @brief Settings view
     * 
     */
    Settings,

    /**
     * @brief Controller diagnostic view
     * 
     */
    ControllerDiag,

    /**
     * @brief Connection status view
     * 
     */
    Connection
};

/**
 * @brief Special flags for controlling how the OLED display is written
 * 
 */
enum OledDisplayFlags : uint8_t
{
    /**
     * @brief None (default)
     * 
     */
    None = 0,

    /**
     * @brief Dump buffer to stream as XBM
     * 
     */
    Dump = 1,

    /**
     * @brief Perform dirty writes on display (not sure if this actually works)
     * 
     */
    Overlay = 1 << 1
};

/**
 * @brief OLED display message
 * 
 */
struct OledDisplayMessage
{
    /**
     * @brief Message kind
     * 
     */
    OledMessage message;

    /**
     * @brief Intended view
     * 
     */
    OledView view;

    union
    {
        /**
         * @brief Analog stick movement message
         * 
         */
        AnalogStickMovement stickPos;

        /**
         * @brief Mousr movement message
         * 
         */
        MousrMovementMsg mousrMove;
    };

    union
    {
        /**
         * @brief Button press status
         * 
         */
        ButtonState buttonPress;

        /**
         * @brief Battery level status
         * 
         */
        MousrBatteryMsg mousrBattery;
    };

    /**
     * @brief BLE connection status
     * 
     */
    MousrConnectionStatus status;

    /**
     * @brief Special message flags
     * 
     */
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

/**
 * @brief OLED display output class
 * 
 */
class Oled
{
private:
    static Oled *singleton;
    Oled();

public:
    /**
     * @brief Get the singleton instance
     * 
     * @return Oled* 
     */
    static Oled *getInstance()
    {
        if (singleton == NULL)
        {
            singleton = new Oled;
        }

        return singleton;
    }

    /**
     * @brief Queues a message to write to the OLED. 
     * @remark Message will be written as soon as possible. If the queue is full, oldest messages will be discarded.
     * @param message Message to queue
     */
    void queueMessage(OledDisplayMessage *message);

    /**
     * @brief Begins OLED
     * 
     * @return true OLED successfully initialized
     * @return false OLED could not be initialized
     */
    bool begin();

    /**
     * @brief Ends OLED
     * 
     * @return true 
     * @return false 
     */
    bool end();

    /**
     * @brief Get the current view
     * 
     * @return OledView 
     */
    OledView getView();

    /**
     * @brief Set the current view
     * 
     * @param view 
     */
    void setView(OledView view);

    /**
     * @brief Locks the view from changing. If the view is locked, any messages queued that are for a different view will be discarded
     * 
     */
    void lockView();

    /**
     * @brief Unlocks the view from changing
     * 
     */
    void unlockView();

    /**
     * @brief Raw display controller. Should generally not be used outside the class
     * @remark Should consider hiding this
     */
    U8G2 u8g2;

private:
    static void displayQueueTask(void *);
    bool hasBegun = false;
    bool isFinalizing = false;
    OledView view = OledView::None;
    bool isLocked;
    QueueHandle_t displayQueue;
};

#endif // MOUSR_OLED_H