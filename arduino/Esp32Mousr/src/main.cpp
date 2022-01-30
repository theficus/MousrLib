#ifdef ARDUINO

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
    Serial.printf("Waiting for status: %s\n", expectedStatus.c_str());
    MousrConnectionStatus s;
    while ((s = mb.getConnectionStatus()) != status)
    {
        string actualStatus = getMousrConnectionStatusString(s);
        Serial.printf("Status: %s ...\n", actualStatus.c_str());
        sleep(1);
    }
}

static void onBluetoothStatusChange(MousrConnectionStatus oldStatus, MousrConnectionStatus newStatus)
{
    Serial.printf("[main] Got status change: %d -> %d\n", (int)oldStatus, (int)newStatus);
}

static void onBluetoothNotify(BLERemoteCharacteristic* characteristic, MousrData& data)
{
    Serial.printf("[main] Got Bluetooth notification for %s: %s\n", characteristic->getUUID().toString().c_str(), data.toString().c_str());
}

#endif // ARDUINO