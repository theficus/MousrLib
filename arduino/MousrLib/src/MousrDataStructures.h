#include <cstdio>
#include <cstdint>
#include <iostream>

enum MousrMessageType : uint8_t
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

#pragma pack(push, 1)
// 0x6218000000001815000000000000000000000000
struct MousrBattery
{
    uint8_t voltage;
    uint8_t isCharging;
    uint8_t isFullCharge;
    uint8_t isAutoModeOn;
    uint8_t isAutoModeActive;
    uint16_t unknown;
};

// 0x309f5c10bff3162ec0d71e18c100870000000000
struct MousrMovement
{
    float speed;
    float held;
    float angle;
};

struct MousrData
{
    MousrMessageType msg;

    // Data blob
    union
    {
        MousrBattery battery;
        MousrMovement movement; 
    };

    // Remaining data
    void* extra;
};

#pragma pack(pop)