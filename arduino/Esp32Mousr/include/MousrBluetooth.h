/**
 * @file MousrBluetooth.h
 * @author Adam Meltzer (11638244+theficus@users.noreply.github.com)
 * @brief BLE functions and classes for working with Mousr
 * @version 0.1
 * @date 2022-03-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#ifndef MOUSR_MOUSR_BLUETOOTH_H
#define MOUSR_MOUSR_BLUETOOTH_H
#ifdef ARDUINO_ARCH_ESP32

#include "Mousr.h"
#include "utility.h"
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <functional>
#include <atomic>
#include <map>

static BLEUUID serviceUuid = BLEUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
static BLEUUID uartWriteUuid = BLEUUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
static BLEUUID uartSubscribeUuid = BLEUUID("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

typedef std::function<void(MousrConnectionStatus oldStatus, MousrConnectionStatus newStatus)> mousr_status_change_callback;
typedef std::function<void(BLERemoteCharacteristic *characteristic, MousrData &message)> mousr_notify_callback;

class MousrBluetooth
{
private:
    static MousrBluetooth *singleton;
    MousrBluetooth();

    // Allow for updating connection status from callbacks
    friend class MousrBluetoothClientCallback;
    friend class MousrBluetoothScanCallback;

public:
    static MousrBluetooth* getInstance()
    {   
        if (singleton == NULL)
        {
            singleton = new MousrBluetooth;
        }

        return singleton;
    }

    MousrConnectionStatus getConnectionStatus()
    {
        return this->connectionStatus;
    }

    unsigned long getPacketsSent()
    {
        return this->packetsSent;
    }

    unsigned long getPacketsReceived()
    {
        return this->packetsReceived;
    }

    void init();
    void connect();
    void disconnect();
    bool discoverCapabilities();
    void startScan();
    void stopScan();
    void sendMessage(MousrData &msg);
    void setMousrNotificationCallback(mousr_notify_callback callback);
    void setConnectionStatusChangeCallback(mousr_status_change_callback callback);

private:
    void setConnectionStatus(MousrConnectionStatus status);
    void incrementPacketsSent();
    void incrementPacketsReceived();
    void onNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

    notify_callback bleNotificationCallback = [=](BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
    {
        this->onNotify(pBLERemoteCharacteristic, pData, length, isNotify);
    };

    mousr_status_change_callback statusChangeCallback;
    mousr_notify_callback mousrNotificationCallback;
    std::atomic_ulong packetsSent;
    std::atomic_ulong packetsReceived;
    MousrConnectionStatus connectionStatus = MousrConnectionStatus::Unknown;
    SemaphoreHandle_t sem;
    BLEScan *bleScan;
    BLEClient *bleClient;
    BLEClientCallbacks *clientCallback;
    BLEAdvertisedDevice device;
    BLERemoteCharacteristic *uartWriteCharacteristic;
    BLERemoteCharacteristic *uartSubscribeCharacteristic;
};

#endif // ARDUINO_ARCH_ESP32
#endif // MOUSR_MOUSR_BLUETOOTH_H