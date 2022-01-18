#pragma once
#ifndef MOUSR_MOUSR_BLUETOOTH_H
#define MOUSR_MOUSR_BLUETOOTH_H

#include "Mousr.h"
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

class MousrBluetooth
{
public:
    MousrBluetooth();
    ~MousrBluetooth();

    MousrConnectionStatus getConnectionStatus()
    {
        return this->connectionStatus;
    }

    void ConnectBluetooth(BLEClientCallbacks *clientCallback,
                          void (*errorFunc)(char *),
                          void (*notifyFunc)(BLECharacteristic *, MousrData *));

    void StartScan();

private:
    const BLEUUID serviceUuid = BLEUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
    const BLEUUID uartWriteUuid = BLEUUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
    const BLEUUID uartSubscribeUuid = BLEUUID("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

    MousrConnectionStatus connectionStatus;
    BLEScan *bleScan;
    BLEClient *bleClient;
    BLEClientCallbacks *clientCallback;
    BLEAdvertisedDevice device;
    BLERemoteCharacteristic *uartWriteCharacteristic;
    BLERemoteCharacteristic *uartSubscribeCharacteristic;
};

#endif