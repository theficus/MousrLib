/**
 * @file Mousr.h
 * @author Adam Meltzer (11638244+theficus@users.noreply.github.com)
 * @brief Classes and headers with Mousr-specific implementation details
 * @version 0.1
 * @date 2022-03-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#ifndef MOUSR_MOUSR_H
#define MOUSR_MOUSR_H

#include "logging.h"
#include "utility.h"
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

/**
 * @brief Bluetooth connection status
 *
 */
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

/**
 * @brief Maps a connection status to a friendly string
 *
 */
static std::map<MousrConnectionStatus, std::string> s_MousrConnectionStatusToStringMap = {
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

static std::string getMousrConnectionStatusString(MousrConnectionStatus status)
{
    uint8_t v = (uint8_t)status;
    if (v > (uint8_t)MousrConnectionStatus::Max)
    {
        // Serial.printf("Could not map unknown status value %d. Returning \"Unknown\"\n", v);
        v = (uint8_t)MousrConnectionStatus::Unknown;
    }

    return s_MousrConnectionStatusToStringMap[(MousrConnectionStatus)v];
}

/**
 * @brief Mousr message type received by the Bluetooth listener
 *
 */
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

/**
 * @brief Commands that can be sent when sending a ROBOT_POSE message
 * @see https://github.com/theficus/MousrLib/wiki/Mousr-messages
 */
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

/**
 * @brief Battery message
 * @note 0x6218000000001815000000000000000000000000
 * @see https://github.com/theficus/MousrLib/wiki/BATTERY_VOLTAGE
 */
struct MousrBatteryMsg
{
    uint8_t voltage;
    uint8_t isCharging;
    uint8_t isFullCharge;
    uint8_t isAutoModeOn;
    uint8_t isAutoModeActive;
    uint16_t unknown;
};

/**
 * @brief Movement message (send or receive)
 * @note 0x309f5c10bff3162ec0d71e18c100870000000000
 */
struct MousrMovementMsg
{
    float speed;
    float held;
    float angle;
};

/**
 * @brief Received data message that encompasses all currently supported message types
 * @see https://github.com/theficus/MousrLib/wiki/Mousr-messages
 */
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
    /**
     * @brief Construct a new Mousr Data object with no parameter values (empty)
     *
     * @param msg Message kind
     * @param cmd Message command
     * @param length Message length
     */
    MousrData(const MousrMessage msg, const MousrCommand cmd, const size_t length = 12);

    /**
     * @brief Construct a new Mousr Data object with pre-defined raw data
     *
     * @param data Bytes
     * @param length Message length
     */
    MousrData(const uint8_t *data, size_t length);

    /**
     * @brief Construct a new Mousr Data object with a character string
     *
     * @param data Message text (null terminated)
     */
    MousrData(const char *data);

    /**
     * @brief Construct a new Mousr Data object
     *
     * @param msg Message kind
     * @param cmd Message command
     * @param data Message data (if less than length will be zero filled)
     * @param length Data length (_not_ message length)
     */
    MousrData(const MousrMessage msg, const MousrCommand cmd, const uint8_t *data, const size_t length = 12);

    /**
     * @brief Destroy the Mousr Data object
     *
     */
    ~MousrData();

    /**
     * @brief Get the raw internal message data
     *
     * @return MousrMessageData*
     * @note Caller must not free
     */
    MousrMessageData *getMessageData();

    /**
     * @brief Get a copy of the raw message data
     *
     * @note Must be free'd by the caller
     */
    void getRawMessageData(uint8_t **data, size_t &length);

    /**
     * @brief Get the Message Kind
     * 
     * @return MousrMessage 
     */
    MousrMessage getMessageKind();

    /**
     * @brief Get the Message Length
     * 
     * @return size_t 
     */
    size_t getMessageLength();

    /**
     * @brief Converts the message to a hexadecimal string
     * 
     * @return std::string 
     */
    std::string toString();

    /**
     * @brief Simple conversion from T -> array of bytes
     * 
     * @tparam T type to convert from
     * @param v value value to convert
     * @return uint8_t* value as bytes
     */
    template <typename T>
    static uint8_t *toBytes(T v)
    {
        size_t sz = sizeof(T);
        uint8_t *p = (uint8_t *)malloc(sz);
        memcpy(p, &v, sz);
        return p;
    }

    /**
     * @brief Simple conversion of array of bytes -> T
     * 
     * @tparam T type to convert to
     * @param data source pointer of the data
     * @param v destination of the converted data
     * @param sz source data size
     */
    template <typename T>
    static void fromBytes(const uint8_t *data, T &v, size_t sz = sizeof(T))
    {
        v = *(T *)malloc(sz);
        memcpy(&v, data, sz);
    }

    /**
     * @brief Converts T to bytes and appends a vector
     * 
     * @tparam T Source data
     * @param vec Destination vector
     * @param v Value
     * @param length Data length
     */
    template <typename T>
    static void append(std::vector<uint8_t> &vec, T v, size_t length = sizeof(T))
    {
        uint8_t *raw = toBytes(v);
        // vec.insert(vec.end(), length, raw);
        for (unsigned i = 0; i < length; i++)
        {
            vec.push_back(raw[i]);
        }
    }

    /**
     * @brief Converts a vector of bytes to a hex string
     * 
     * @param data Data to convert
     * @param addSignifier If true, adds 0x
     * @return std::string 
     */
    static std::string toString(const std::vector<uint8_t> data, bool addSignifier = true)
    {
        return toString(data.data(), data.size(), addSignifier);
    }

    // Converts an array of bytes to a hex string
    // TODO: Use C char type instead
    static std::string toString(const uint8_t *data, size_t length, bool addSignifier = true)
    {
        std::stringstream ss;

        if (addSignifier == true)
        {
            ss << "0x";
        }

        ss << std::hex << std::setfill('0');

        for (int i = 0; i < length; i++)
        {
            ss << std::setw(2) << (unsigned)data[i];
        }

        return ss.str();
    }

private:
    void initializeData(const MousrMessage msg, const MousrCommand cmd, const uint8_t* data, const size_t length, const size_t padding = 3);
    uint8_t* raw;
    size_t rawLength;
};

#endif // MOUSR_MOUSR_H
