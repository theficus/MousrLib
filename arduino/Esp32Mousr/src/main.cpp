#include "main.h"

enum class displayMode
{
    unknown,
    mousr,
    logging,
    settings
};

void die()
{
    Serial.println("SYSTEM ERROR");
    while (1)
        ;
}

void setup()
{
    Serial.begin(115200);

    // Set up Bluetooth
    MousrBluetooth::setConnectionStatusChangeCallback(onBluetoothStatusChange);
    MousrBluetooth::setMousrNotificationCallback(onBluetoothNotify);
    mb.ConnectBluetooth();
}

void loop()
{
}

static void onBluetoothStatusChange(MousrConnectionStatus oldStatus, MousrConnectionStatus newStatus)
{
    s_writeLogF("Got status change: %d -> %d\n", oldStatus, newStatus);
}

static void onBluetoothNotify(BLERemoteCharacteristic* characteristic, MousrData data)
{
    s_writeLogF("Got Bluetooth notification for %s: %s", characteristic->getUUID().toString().c_str(), data.toString().c_str());
}