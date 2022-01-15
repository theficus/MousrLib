#ifndef MOUSR_H
#define MOUSR_H

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

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
    void *extra;
};

#pragma pack(pop)

#if 0
class Mousr
{
public:
    static BLEUUID serviceUuid;
    Mousr();
    ~Mousr();

private:
    const std::string serviceUuidStr = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
    const std::string uartWriteUuidStr = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
    const std::string uartSubscribeUuid = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";
    static BLEUUID serviceUuid;
    static BLEUUID uartWriteUuid;
    static BLEUUID uartSubscribeUuid;
    static BLEUUID serviceUuid;
};
#endif

#endif