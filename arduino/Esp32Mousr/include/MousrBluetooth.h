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

typedef function<void(BLERemoteCharacteristic *characteristic, MousrData *message)> mousr_notify_callback;

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

    void ConnectBluetooth(BLEClientCallbacks *clientCallback,
                          mousr_notify_callback);

    void StartScan();
    void StopScan();
    void SendMessage(MousrData* msg);

private:
    void setConnectionStatus(MousrConnectionStatus status)
    {
        s_writeLogF("Changing connection status from '%d' to '%d'", this->connectionStatus, status);
        this->connectionStatus = status;
    }

    void incrementPacketsSent()
    {
        this->packetsSent.fetch_add(1);
    }

    void incrementPacketsReceived()
    {
        this->packetsReceived.fetch_add(1);
    }

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