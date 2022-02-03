#include "MousrBluetooth.h"

// Synchronize status changes
static portMUX_TYPE bleStatusMutex = portMUX_INITIALIZER_UNLOCKED;

// Scan callback
class MousrBluetoothScanCallback : public BLEAdvertisedDeviceCallbacks
{
public:
    MousrBluetoothScanCallback(MousrBluetooth *ble)
    {
        s_printf("MousrBluetoothScanCallback(): %p\n", &ble);
        this->ble = ble;
    }

    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        if (advertisedDevice.haveName() == false ||
            advertisedDevice.haveServiceUUID() == false)
        {
            // s_writeLogLn("Skipping device as it doesn't have enough data.");
            return;
        }

        s_printf("Device seen: %s\n", advertisedDevice.toString().c_str());

        if (advertisedDevice.getName() == "Mousr" &&
            advertisedDevice.getServiceUUID().toString() == serviceUuid.toString())
        {
            s_println("Found device");
            ble->device = advertisedDevice;
            //advertisedDevice.getScan()->stop();
            ble->stopScan();
            ble->setConnectionStatus(MousrConnectionStatus::Discovered);
        }
    }

private:
    MousrBluetooth *ble;
};

// Client callback
class MousrBluetoothClientCallback : public BLEClientCallbacks
{
public:
    MousrBluetoothClientCallback(MousrBluetooth *ble)
    {
        this->ble = ble;
    }

    void onConnect(BLEClient *client)
    {
        s_println("MousrBluetoothClientCallback::onConnect()");
        this->ble->setConnectionStatus(MousrConnectionStatus::Connected);
    }

    void onDisconnect(BLEClient *client)
    {
        s_println("MousrBluetoothClientCallback::onDisconnect()");
        this->ble->setConnectionStatus(MousrConnectionStatus::Disconnected);
    }

private:
    MousrBluetooth *ble;
};

MousrBluetooth::MousrBluetooth()
{
    // Do not use the helper function for this since things may not be initialized yet.
    this->connectionStatus = MousrConnectionStatus::Disconnected;
}

MousrBluetooth::~MousrBluetooth()
{
    if (this->bleScan != nullptr)
    {
        this->bleScan->stop();
        this->bleScan->clearResults();
        delete this->bleScan;
    }

    if (this->bleClient != nullptr)
    {
        this->bleClient->disconnect();
        delete this->bleClient;
    }

    this->setConnectionStatus(MousrConnectionStatus::Unknown);
}

void MousrBluetooth::init()
{
    if (BLEDevice::getInitialized() == false)
    {
        BLEDevice::init("");
    }

    this->bleClient = BLEDevice::createClient();
    this->bleClient->setClientCallbacks(new MousrBluetoothClientCallback(this));
    this->bleScan = BLEDevice::getScan();
    this->bleScan->setAdvertisedDeviceCallbacks(new MousrBluetoothScanCallback(this));
}

bool MousrBluetooth::discoverCapabilities()
{
    bool success = false;
    BLERemoteService *service = bleClient->getService(serviceUuid);
    if (service == nullptr)
    {
        Serial.printf("ERROR: Could not find service: %s\n", serviceUuid.toString().c_str());
        goto final;
    }

    s_printf("Getting characteristic: %s\n", uartWriteUuid.toString().c_str());
    uartWriteCharacteristic = service->getCharacteristic(uartWriteUuid);
    if (uartWriteCharacteristic == nullptr)
    {
        Serial.printf("ERROR: Could not find characteristic: %s\n", uartWriteUuid.toString().c_str());
        goto final;
    }

    s_printf("Getting characteristic: %s\n", uartWriteUuid.toString().c_str());
    uartSubscribeCharacteristic = service->getCharacteristic(uartSubscribeUuid);
    if (uartSubscribeCharacteristic == nullptr)
    {
        s_printf("ERROR: Could not find characteristic: %s\n", uartSubscribeUuid.toString().c_str());
        goto final;
    }

    if (uartSubscribeCharacteristic->canNotify() == true)
    {
        uartSubscribeCharacteristic->registerForNotify(this->bleNotificationCallback);
    }
    else
    {
        s_printf("ERROR: Cannot register for notifications from characteristic: %s\n", uartSubscribeCharacteristic->getUUID().toString().c_str());
        goto final;
    }

    success = true;

final:
    if (success == false)
    {
        Serial.println("[ERROR] Connection failed.");

        this->setConnectionStatus(MousrConnectionStatus::Error);

        if (bleClient != nullptr)
        {
            bleClient->disconnect();
        }

        return false;
    }

    this->setConnectionStatus(MousrConnectionStatus::Ready);
    return true;
}

void MousrBluetooth::connect()
{
    if (&this->device == nullptr ||
        this->getConnectionStatus() != MousrConnectionStatus::Discovered)
    {
        s_printf("[ERROR] No device was discovered. Cannot connect. Device: %p Status: %d\n",
                 this->device,
                 (int)this->getConnectionStatus());
        return;
    }

    s_printf("Connecting to device: %s\n", this->device.toString().c_str());
    this->setConnectionStatus(MousrConnectionStatus::Connecting);
    this->bleClient->connect(&this->device);
}

void MousrBluetooth::sendMessage(MousrData &data)
{
    // Serial.printf("Sending message to %s: %s", this->uartWriteCharacteristic->getUUID().toString().c_str(), data.toString().c_str());
    uint8_t *raw;
    size_t length = 0;
    data.getRawMessageData(&raw, length);
    this->uartWriteCharacteristic->writeValue(raw, length);
    this->incrementPacketsSent();
    free(raw);
}

void MousrBluetooth::startScan()
{
    this->bleScan->setActiveScan(true);
    this->bleScan->setInterval(100);
    this->bleScan->setWindow(99);
    this->bleScan->start(30);
    this->setConnectionStatus(MousrConnectionStatus::Scanning);
}

void MousrBluetooth::stopScan()
{
    this->bleScan->stop();
    this->bleScan->clearResults();
    this->setConnectionStatus(MousrConnectionStatus::ScanStopped);
}

void MousrBluetooth::setMousrNotificationCallback(mousr_notify_callback callback)
{
    this->mousrNotificationCallback = callback;
}

void MousrBluetooth::setConnectionStatusChangeCallback(mousr_status_change_callback callback)
{
    this->statusChangeCallback = callback;
}

void MousrBluetooth::setConnectionStatus(MousrConnectionStatus status)
{
    portENTER_CRITICAL(&bleStatusMutex);
    MousrConnectionStatus oldStatus = this->connectionStatus;
    if (oldStatus > MousrConnectionStatus::Max)
    {
        Serial.printf("Invalid connection status %d\n", (int)oldStatus);
        oldStatus = MousrConnectionStatus::Unknown;
    }

    this->connectionStatus = status;
    portEXIT_CRITICAL(&bleStatusMutex);

    std::string oldStatusStr = getMousrConnectionStatusString(oldStatus);
    std::string newStatusStr = getMousrConnectionStatusString(status);
    s_printf("Changing connection status from '%s' (0x%x) to '%s' (0x%x)\n",
             oldStatusStr.c_str(),
             (uint8_t)oldStatus,
             newStatusStr.c_str(),
             (uint8_t)status);

    if (statusChangeCallback != nullptr)
    {
        statusChangeCallback(oldStatus, status);
    }
}

void MousrBluetooth::incrementPacketsSent()
{
    this->packetsSent.fetch_add(1);
}

void MousrBluetooth::incrementPacketsReceived()
{
    this->packetsReceived.fetch_add(1);
}

void MousrBluetooth::onNotify(BLERemoteCharacteristic *characteristic,
                              uint8_t *data,
                              size_t length,
                              bool isNotify)
{
    if (isNotify == false)
    {
        return;
    }

    // debugLogF("Got notification from characteristic: %s. Length: %zu\n", characteristic->getUUID().toString().c_str(), length);
    MousrData d(data, length);
    if (this->mousrNotificationCallback != nullptr)
    {
        this->mousrNotificationCallback(characteristic, d);
    }
}