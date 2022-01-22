#pragma once
#ifndef MOUSR_MOUSR_BLUETOOTH_H
#define MOUSR_MOUSR_BLUETOOTH_H

#include "Mousr.h"
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <functional>
#include <atomic>

using namespace std;

static BLEUUID serviceUuid = BLEUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
static BLEUUID uartWriteUuid = BLEUUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
static BLEUUID uartSubscribeUuid = BLEUUID("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

enum class MousrConnectionStatus
{
    Unknown,
    None,
    Scanning,
    ScanStopped,
    Discovered,
    Connecting,
    Connected,
    Disconnected,
    Error
};

typedef function<void(MousrConnectionStatus oldStatus, MousrConnectionStatus newStatus)> mousr_status_change_callback;
typedef function<void(BLERemoteCharacteristic *characteristic, MousrData message)> mousr_notify_callback;

class MousrBluetooth
{
    // Allow for updating connection status from callbacks
    friend class MousrBluetoothClientCallback;
    friend class MousrBluetoothScanCallback;

public:
    MousrBluetooth();
    ~MousrBluetooth();

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

    void ConnectBluetooth();
    void StartScan();
    void StopScan();
    void SendMessage(MousrData msg);

    static void setMousrNotificationCallback(mousr_notify_callback callback)
    {
        MousrBluetooth::notifyCallback = callback;
    }

    static void setConnectionStatusChangeCallback(mousr_status_change_callback callback)
    {
        MousrBluetooth::statusChangeCallback = callback;
    }

private:
    void setConnectionStatus(MousrConnectionStatus status)
    {
        MousrConnectionStatus oldStatus = this->connectionStatus;
        s_writeLogF("Changing connection status from '%d' to '%d'", oldStatus, status);
        this->connectionStatus = status;

        if (MousrBluetooth::statusChangeCallback != nullptr)
        {
            MousrBluetooth::statusChangeCallback(oldStatus, status);
        }
    }

    void incrementPacketsSent()
    {
        this->packetsSent.fetch_add(1);
    }

    void incrementPacketsReceived()
    {
        this->packetsReceived.fetch_add(1);
    }

    static void internalNotifyCallback(BLERemoteCharacteristic *characteristic,
                                       uint8_t *data,
                                       size_t length,
                                       bool isNotify)
    {
        if (isNotify == false)
        {
            return;
        }

        debugLogF("Got notification from characteristic: %s\n", characteristic->getUUID().toString().c_str());
        MousrData d(data, length);
        debugLogF("Message: %s\n", d.toString().c_str());

        if (MousrBluetooth::notifyCallback != nullptr)
        {
            MousrBluetooth::notifyCallback(characteristic, d);
        }
    }

    static mousr_status_change_callback statusChangeCallback;
    static mousr_notify_callback notifyCallback;

    atomic_ulong packetsSent;
    atomic_ulong packetsReceived;
    MousrConnectionStatus connectionStatus;

    BLEScan *bleScan;
    BLEClient *bleClient;
    BLEClientCallbacks *clientCallback;
    BLEAdvertisedDevice device;
    BLERemoteCharacteristic *uartWriteCharacteristic;
    BLERemoteCharacteristic *uartSubscribeCharacteristic;
};

#endif