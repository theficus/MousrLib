#pragma once
#ifndef MOUSR_MOUSR_BLUETOOTH_H
#define MOUSR_MOUSR_BLUETOOTH_H
#ifdef ARDUINO_ARCH_ESP32

#include "Mousr.h"
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <functional>
#include <atomic>
#include <map>

using namespace std;

static BLEUUID serviceUuid = BLEUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
static BLEUUID uartWriteUuid = BLEUUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
static BLEUUID uartSubscribeUuid = BLEUUID("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

typedef function<void(MousrConnectionStatus oldStatus, MousrConnectionStatus newStatus)> mousr_status_change_callback;
typedef function<void(BLERemoteCharacteristic *characteristic, MousrData &message)> mousr_notify_callback;

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

    void init();
    void connect();
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
    atomic_ulong packetsSent;
    atomic_ulong packetsReceived;
    MousrConnectionStatus connectionStatus = MousrConnectionStatus::Unknown;
    BLEScan *bleScan;
    BLEClient *bleClient;
    BLEClientCallbacks *clientCallback;
    BLEAdvertisedDevice device;
    BLERemoteCharacteristic *uartWriteCharacteristic;
    BLERemoteCharacteristic *uartSubscribeCharacteristic;
};

#endif // ARDUINO_ARCH_ESP32
#endif // MOUSR_MOUSR_BLUETOOTH_H