#pragma once
#ifndef MOUSR_MAIN_H
#define MOUSR_MAIN_H

#include "common.h"

#ifdef ARDUINO
#ifdef ARDUINO_ARCH_ESP32
#include "esp32-hal-log.h"
#endif // ARDUINO_ARCH_ESP32

#include "Mousr.h"
#include "MousrBluetooth.h"
#include "controller.h"
#include "Oled.h"
#include "utility.h"

MousrBluetooth mb;
void waitForStatus(MousrConnectionStatus);
static void onBluetoothStatusChange(MousrConnectionStatus oldStatus, MousrConnectionStatus newStatus);
static void onBluetoothNotify(BLERemoteCharacteristic* characteristic, MousrData& data);

#endif // ARDUINO
#endif // MOUSR_MAIN_H