#pragma once
#ifndef MOUSR_MOUSR_H
#define MOUSR_MOUSR_H

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iterator>
#include <algorithm>
#include <map>

#include "Log.h"

using namespace std;

enum class MousrConnectionStatus : uint8_t
{
    Unknown,
    None,
    Scanning,
    ScanStopped,
    Discovered,
    Connecting,
    Connected,
    Ready,
    Disconnected,
    Error,
    Max = Error, // Match whatever the last value is
};

static std::map<MousrConnectionStatus, string> s_MousrConnectionStatusToStringMap = {
    {MousrConnectionStatus::Unknown, "Unknown"},
    {MousrConnectionStatus::None, "None"},
    {MousrConnectionStatus::Scanning, "Scanning"},
    {MousrConnectionStatus::ScanStopped, "ScanStopped"},
    {MousrConnectionStatus::Discovered, "Discovered"},
    {MousrConnectionStatus::Connecting, "Connecting"},
    {MousrConnectionStatus::Connected, "Connected"},
    {MousrConnectionStatus::Ready, "Ready"},
    {MousrConnectionStatus::Disconnected, "Disconnected"},
    {MousrConnectionStatus::Error, "Error"},
};

static string getMousrConnectionStatusString(MousrConnectionStatus status)
{
    uint8_t v = (uint8_t)status;
    if (v > (uint8_t)MousrConnectionStatus::Max)
    {
        s_writeLogF("Could not map unknown status value %d. Returning \"Unknown\"\n", v);
        v = (uint8_t)MousrConnectionStatus::Unknown;
    }

    return s_MousrConnectionStatusToStringMap[(MousrConnectionStatus)v];
}

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

#pragma pack(push, 1) // Very important to make sure we align properly
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
        void *data;
    };
};

#pragma pack(pop)

// Wraps Mousr bytestream data
class MousrData
{
public:
    MousrData(const uint8_t *data, size_t length);
    MousrData(const char *data);
    MousrData(const MousrMessage msg, const MousrCommand cmd, vector<uint8_t> data);
    MousrData(string data);
    ~MousrData();

    MousrMessageData *getMessageData();
    void getRawMessageData(uint8_t **data, size_t &length);
    MousrMessage getMessageKind();
    size_t getMessageLength();
    string toString();

    // Simple conversion from T -> array of bytes
    template <typename T>
    static uint8_t *toBytes(T v)
    {
        size_t sz = sizeof(T);
        uint8_t *p = (uint8_t *)malloc(sz);
        memcpy(p, &v, sz);
        return p;
    }

    // Simple conversion of array of bytes -> T
    template <typename T>
    static void fromBytes(const uint8_t *data, T &v, size_t sz = sizeof(T))
    {
        v = *(T *)malloc(sz);
        memcpy(&v, data, sz);
    }

    // Converts T to bytes and appends to the vector
    template <typename T>
    static void append(vector<uint8_t> &vec, T v, size_t length = sizeof(T))
    {
        uint8_t *raw = toBytes(v);
        // vec.insert(vec.end(), length, raw);
        for (unsigned i = 0; i < length; i++)
        {
            vec.push_back(raw[i]);
        }

        free(raw);
    }

    // Converts a vector of bytes to a hex string
    static string toString(const vector<uint8_t> data, bool addSignifier = true)
    {
        return toString(data.data(), data.size(), addSignifier);
    }

    // Converts an array of bytes to a hex string
    static string toString(const uint8_t *data, size_t length, bool addSignifier = true)
    {
        stringstream ss;

        if (addSignifier == true)
        {
            ss << "0x";
        }

        ss << hex << setfill('0');

        for (int i = 0; i < length; i++)
        {
            ss << setw(2) << (unsigned)data[i];
        }

        return ss.str();
    }
};

#endif