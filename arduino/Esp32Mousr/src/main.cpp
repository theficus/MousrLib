#include "main.h"

// BLE characteristics
BLEUUID serviceUuid("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
BLEUUID uartWriteUuid("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
BLEUUID uartSubscribeUuid("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

BLEScan *bleScan;
BLEClient *bleClient;
BLEAdvertisedDevice device;
BLERemoteCharacteristic *uartWriteCharacteristic;
BLERemoteCharacteristic *uartSubscribeCharacteristic;
bool isConnected = false;

enum class displayMode
{
    unknown,
    mousr,
    logging,
    settings
};

displayMode mode = displayMode::unknown;

// Print the data as a hex string
void printData(uint8_t *data, size_t length)
{
    MousrMessageData *mousr = (MousrMessageData *)data;
    Serial.printf("Got message type: %2x\n", (uint8_t)mousr->msg);

    switch (mousr->msg)
    {
    case MousrMessage::BATTERY_VOLTAGE:
        writeLogF(LogDestination::All, "Battery voltage: %d%%\n", mousr->battery.voltage);
        // u8x8log.printf("BATT:%d%%\n", mousr->battery.voltage);
        break;
    default:
        break;
    }
}

// Callback for discovering BLE devices
class BleScanCallback : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        Serial.printf("Discovered device: %s\n", advertisedDevice.toString().c_str());
        if (advertisedDevice.getName() == "Mousr")
        {
            Serial.printf("Device ptr: %p\n", &advertisedDevice);
            device = advertisedDevice;
            Serial.println("Stopping BLE scan");
            // writeOled(advertisedDevice.getAddress().toString().c_str());
            bleScan->stop();
            bleScan->clearResults();
            semGive();
        }
    }
};

class BleClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *client)
    {
        Serial.printf("Connected to client: %d\n", client->isConnected());
    }

    void onDisconnect(BLEClient *client)
    {
        Serial.printf("Disconnected from client: %d\n", client->isConnected());
    }
};

void bleNotifyCallback(
    BLERemoteCharacteristic *characteristic,
    uint8_t *data,
    size_t length,
    bool isNotify)
{
    Serial.printf("Got notification from characteristic %s\n", characteristic->getUUID().toString().c_str());
    Serial.printf("Length: %zu Data: ", length);
    printData(data, length);
}

void die()
{
    Serial.println("SYSTEM ERROR");
    while (1)
        ;
}

void reset()
{
    // TODO: Clean up and reset. Also restart scan?
}

void discoverDevice()
{
    Serial.println("In discoverDevice()");
    Serial.printf("Got device: Name: %s Address: %s RSSI: %d\n",
                  device.getName().c_str(),
                  device.getAddress().toString().c_str(),
                  device.getRSSI());
    Serial.println("Leaving discoverDevice()");
}

bool setupDevice()
{
    bool success = false;
    Serial.println("Connecting to device ...");
    bleClient = BLEDevice::createClient();
    bleClient->setClientCallbacks(new BleClientCallback());
    bleClient->connect(&device);
    Serial.println("Device connected!");
    Serial.println("Getting characteristics ...");
    BLERemoteService *remoteService = bleClient->getService(serviceUuid);
    if (remoteService == nullptr)
    {
        Serial.printf("ERROR: Could not find service UUID: %s\n", serviceUuid.toString().c_str());
        goto final;
    }

    // TX characteristic
    uartWriteCharacteristic = remoteService->getCharacteristic(uartWriteUuid);
    if (uartWriteCharacteristic == nullptr)
    {
        Serial.printf("ERROR: Could not find characteristic: %s\n", uartWriteUuid.toString().c_str());
        goto final;
    }

    uartSubscribeCharacteristic = remoteService->getCharacteristic(uartSubscribeUuid);
    if (uartSubscribeCharacteristic == nullptr)
    {
        Serial.printf("ERROR: Could not find characteristic: %s\n", uartSubscribeUuid.toString().c_str());
        goto final;
    }

    if (uartSubscribeCharacteristic->canNotify() == true)
    {
        uartSubscribeCharacteristic->registerForNotify(bleNotifyCallback);
    }
    else
    {
        Serial.printf("ERROR: Could not subscribe to notifications from characteristic: %s\n", uartSubscribeCharacteristic->toString().c_str());
        goto final;
    }

    success = true;

final:
    if (success == false &&
        bleClient != nullptr)
    {
        Serial.println("Disconnecting device because of a setup failure.");
        bleClient->disconnect();
    }

    return success;
}

void startBleScan()
{
    Serial.println("Setting up BLE for scan ...");
    BLEDevice::init("");
    bleScan = BLEDevice::getScan();
    bleScan->setAdvertisedDeviceCallbacks(new BleScanCallback());
    bleScan->setActiveScan(true);
    bleScan->setInterval(100);
    bleScan->setWindow(99);
    bleScan->start(30, false);
    semTake();
    Serial.println("startBleScan() end ...");
}


void setup()
{
    Serial.begin(115200);
    semGive();
    // setupOled();
    //setupOledLogView();
    OledLog.SetupOledLogDisplay();
    setupSeesaw();
    startBleScan();
    Serial.printf("Device: %p\n", device);
    discoverDevice();
    Serial.println("Setting up...");
    setupDevice();
}

void loop()
{
    delay(100);
    // delay(10);
    // readSeeSaw();
}