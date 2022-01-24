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
    s_writeLogLn("setup()...");
    // Set up Bluetooth
    mb.init();
    mb.setConnectionStatusChangeCallback(onBluetoothStatusChange);
    mb.setMousrNotificationCallback(onBluetoothNotify);
    mb.startScan();
    waitForStatus(MousrConnectionStatus::Discovered);
    mb.connect();
    waitForStatus(MousrConnectionStatus::Connected);
    if (mb.discoverCapabilities() == false)
    {
        die();
    }
}

void loop()
{
    logMemory();
    sleep(1);
}

// TODO: Add timeout
void waitForStatus(MousrConnectionStatus status)
{
    string expectedStatus = getMousrConnectionStatusString(status);
    s_writeLogF("Waiting for status: %s\n", expectedStatus.c_str());
    MousrConnectionStatus s;
    while ((s = mb.getConnectionStatus()) != status)
    {
        string actualStatus = getMousrConnectionStatusString(s);
        s_writeLogF("Status: %s ...\n", actualStatus.c_str());
        sleep(1);
    }

    s_writeLogLn("Done!");
}

static void onBluetoothStatusChange(MousrConnectionStatus oldStatus, MousrConnectionStatus newStatus)
{
    s_writeLogF("[main] Got status change: %d -> %d\n", oldStatus, newStatus);
}

static void onBluetoothNotify(BLERemoteCharacteristic* characteristic, MousrData& data)
{
    s_writeLogF("[main] Got Bluetooth notification for %s: %s\n", characteristic->getUUID().toString().c_str(), data.toString().c_str());
}