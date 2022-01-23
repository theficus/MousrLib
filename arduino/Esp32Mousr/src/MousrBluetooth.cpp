#include "MousrBluetooth.h"
#include "Log.h"

mousr_status_change_callback statusChangeCallback;
mousr_notify_callback notifyCallback;

// Scan callback
class MousrBluetoothScanCallback : public BLEAdvertisedDeviceCallbacks
{
public:
    MousrBluetoothScanCallback(MousrBluetooth *ble)
    {
        s_writeLogF("MousrBluetoothScanCallback(): %p", &ble);
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

        s_writeLogF("Device seen: %s\n", advertisedDevice.toString().c_str());

        if (advertisedDevice.getName() == "Mousr" &&
            advertisedDevice.getServiceUUID().toString() == serviceUuid.toString())
        {
            s_writeLogLn("Found device");
            ble->device = advertisedDevice;
            ble->StopScan();
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
        s_writeLogF("Setting callback to %p\n", &ble);
        this->ble = ble;
    }

    void onConnect(BLEClient *client)
    {
        this->ble->setConnectionStatus(MousrConnectionStatus::Connected);
        s_writeLogF("Connected to client: %d\n", client->isConnected());
    }

    void onDisconnect(BLEClient *client)
    {
        this->ble->setConnectionStatus(MousrConnectionStatus::Disconnected);
        s_writeLogF("Disconnected from client: %d\n", client->isConnected());
    }

private:
    MousrBluetooth *ble;
};

void internalNotifyCallback(BLERemoteCharacteristic *characteristic,
                            uint8_t *data,
                            size_t length,
                            bool isNotify)
{
    if (isNotify == false)
    {
        return;
    }

    debugLogF("Got notification from characteristic: %s. Length: %zu\n", characteristic->getUUID().toString().c_str(), length);
    MousrData d(data, length);
    //debugLogF("Message: %s\n", d.toString().c_str());

    if (notifyCallback != nullptr)
    {
        notifyCallback(characteristic, d);
    }
}

MousrBluetooth::MousrBluetooth()
{
    s_writeLogLn("MousrBluetooth::MousrBluetooth()");
    // Do not use the helper function for this since things may not be initialized yet.
    this->connectionStatus = MousrConnectionStatus::Disconnected;
}

MousrBluetooth::~MousrBluetooth()
{
    debugLogLn("~MousrBluetooth: Cleaning up...");

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

void MousrBluetooth::Init()
{
    if (BLEDevice::getInitialized() == false)
    {
        BLEDevice::init("");
    }

    s_writeLogLn("MousrBluetooth::MousrBluetooth(): BLEDevice::createClient()");
    this->bleClient = BLEDevice::createClient();
    this->bleClient->setClientCallbacks(new MousrBluetoothClientCallback(this));

    s_writeLogLn("MousrBluetooth::MousrBluetooth(): BLEDevice::getScan()");
    this->bleScan = BLEDevice::getScan();
    this->bleScan->setAdvertisedDeviceCallbacks(new MousrBluetoothScanCallback(this));
}

bool MousrBluetooth::DiscoverCapabilities()
{
    bool success = false;
    s_writeLogLn("Getting characteristics ...");

    BLERemoteService *service = bleClient->getService(serviceUuid);
    if (service == nullptr)
    {
        s_writeLogF("ERROR: Could not find service: %s\n", serviceUuid.toString().c_str());
        goto final;
    }

    s_writeLogF("Getting characteristic: %s\n", uartWriteUuid.toString().c_str());
    uartWriteCharacteristic = service->getCharacteristic(uartWriteUuid);
    if (uartWriteCharacteristic == nullptr)
    {
        s_writeLogF("ERROR: Could not find characteristic: %s\n", uartWriteUuid.toString().c_str());
        goto final;
    }

    s_writeLogF("Getting characteristic: %s\n", uartWriteUuid.toString().c_str());
    uartSubscribeCharacteristic = service->getCharacteristic(uartSubscribeUuid);
    if (uartSubscribeCharacteristic == nullptr)
    {
        s_writeLogF("ERROR: Could not find characteristic: %s\n", uartSubscribeUuid.toString().c_str());
        goto final;
    }

    if (uartSubscribeCharacteristic->canNotify() == true)
    {
        uartSubscribeCharacteristic->registerForNotify(internalNotifyCallback);
    }
    else
    {
        s_writeLogF("ERROR: Cannot register for notifications from characteristic: %s", uartSubscribeCharacteristic->getUUID().toString().c_str());
        goto final;
    }

    success = true;

final:
    if (success == false)
    {
        s_writeLogLn("[ERROR] Connection failed.");

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

void MousrBluetooth::Connect()
{
    if (&this->device == nullptr ||
        this->getConnectionStatus() != MousrConnectionStatus::Discovered)
    {
        s_writeLogF("[ERROR] No device was discovered. Cannot connect. Device: %p Status: %d\n",
                    this->device,
                    this->getConnectionStatus());
        return;
    }

    s_writeLogF("Connecting to device: %s\n", this->device.toString().c_str());
    this->setConnectionStatus(MousrConnectionStatus::Connecting);
    this->bleClient->connect(&this->device);
}

void MousrBluetooth::SendMessage(MousrData data)
{
    debugLogF("Sending message to %s: %s", this->uartWriteCharacteristic->getUUID().toString().c_str(), data.toString().c_str());
    uint8_t *raw;
    size_t length = 0;
    data.getRawMessageData(&raw, length);
    this->uartWriteCharacteristic->writeValue(raw, length);
    this->incrementPacketsSent();
}

void MousrBluetooth::StartScan()
{
    s_writeLogLn("Starting scan ...");
    this->bleScan->setActiveScan(true);
    this->bleScan->setInterval(100);
    this->bleScan->setWindow(99);
    this->bleScan->start(30, false);
    this->setConnectionStatus(MousrConnectionStatus::Scanning);
}

void MousrBluetooth::StopScan()
{
    s_writeLogLn("Stopping BLE scan");
    this->bleScan->stop();
    this->bleScan->clearResults();
    this->setConnectionStatus(MousrConnectionStatus::ScanStopped);
}

void MousrBluetooth::setMousrNotificationCallback(mousr_notify_callback callback)
{
    notifyCallback = callback;
}

void MousrBluetooth::setConnectionStatusChangeCallback(mousr_status_change_callback callback)
{
    statusChangeCallback = callback;
}

void MousrBluetooth::setConnectionStatus(MousrConnectionStatus status)
{
    MousrConnectionStatus oldStatus = this->connectionStatus;
    if (oldStatus > MousrConnectionStatus::Max)
    {
        s_writeLogF("Invalid connection status %d\n", oldStatus);
        oldStatus = MousrConnectionStatus::Unknown;
    }

    this->connectionStatus = status;

    string oldStatusStr = getMousrConnectionStatusString(oldStatus);
    string newStatusStr = getMousrConnectionStatusString(status);
    s_writeLogF("Changing connection status from '%s' to '%s'\n", oldStatusStr.c_str(), newStatusStr.c_str());

    if (statusChangeCallback != nullptr)
    {
        statusChangeCallback(oldStatus, status);
    }
}