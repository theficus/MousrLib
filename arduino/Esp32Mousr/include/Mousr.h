#pragma once
#ifndef MOUSR_MOUSR_H
#define MOUSR_MOUSR_H
#include <cstdint>
#include <cstdio>
#include <iostream>

enum class MousrConnectionStatus
{
    Unknown,
    None,
    Scanning,
    Discovered,
    Connecting,
    Connected,
    Disconnected,
    Error
};

enum class MousrMessage : uint8_t
{
    AUTO_ACK = 0x0f,
    FIRMWARE_VERSION = 0x1c,
    HARDWARE_VERSION = 0x1d,
    INIT_DONE = 0x1e,
    ROBOT_POSE = 0x30,
    RESETTAIL_FAILINFO = 0x32,
    STUCK_TOF_MSG = 0x40,
    CRASH_LOG_FINISH = 0x5f,
    CRASH_LOG_STRING = 0x60,
    DEBUG_INFO = 0x61,
    BATTERY_VOLTAGE = 0x62,
    ROBOT_STOPPED = 0x63,
    RC_STUCK = 0x64,
    NACK = 0xff,
};

enum class MousrCommand : uint8_t
{
    STOP = 0x00,
    SPIN = 0x01,
    MOVE = 0x02,
    RESET_HEADING = 0x03,
    GET_DEBUG_LOG = 0x04,
    SPIN_PLAN = 0x05,
    ENTER_DFU_MODE = 0x08,
    TURN_OFF = 0x09,
    SLEEP = 0x0a,
    CONFIG_AUTO_MODE = 0x0f,
    CHIRP = 0x12,
    SOUND_VOLUME = 0x13,
    Flick_SIGNAL = 0x17,
    Reverse_SIGNAL = 0x18,
    Tail_Calib_SIGNAL = 0x19,
    Set_Tail_SIGNAL = 0x1a,
    INITIALIZE_DEVICE = 0x1c,
    FLIP_ROBOT = 0x1f,
    CFG_DRIVER_ASSIST = 0x29,
    TUTORIAL_STEP = 0x2d,
    CMD_SET_TIME = 0x2e,
    INVALID = 0x64,
};

#pragma pack(push, 1)
// 0x6218000000001815000000000000000000000000
struct MousrBatteryMsg
{
    uint8_t voltage;
    uint8_t isCharging;
    uint8_t isFullCharge;
    uint8_t isAutoModeOn;
    uint8_t isAutoModeActive;
    uint16_t unknown;
};

// 0x309f5c10bff3162ec0d71e18c100870000000000
struct MousrMovementMsg
{
    float speed;
    float held;
    float angle;
};

struct MousrMessageData
{
    MousrMessage msg;

    // Data blob
    union
    {
        MousrBatteryMsg battery;
        MousrMovementMsg movement;
        void* data;
    };
};

struct MousrRawMessageData
{
    uint8_t *data;
    size_t length;
};

#pragma pack(pop)

// Wraps Mousr bytestream data
class MousrData
{
public:
    MousrData(const uint8_t *data, size_t length);
    MousrData(const char* data);
    ~MousrData();
    MousrRawMessageData *getRawMessageData();
    MousrMessageData *getMessageData();
    MousrMessage getMessageKind();
};

#endif