#pragma once
#ifndef MOUSR_MAIN_H
#define MOUSR_MAIN_H

#include "Mousr.h"
#include "MousrBluetooth.h"
#include "controller.h"
#include "Oled.h"
#include "Log.h"
#include "OledLog.h"
#include "SerialLog.h"
#include "utility.h"

MousrBluetooth mb;

static void onBluetoothStatusChange(MousrConnectionStatus oldStatus, MousrConnectionStatus newStatus);
static void onBluetoothNotify(BLERemoteCharacteristic* characteristic, MousrData data);

#endif