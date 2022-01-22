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
        this->ble = ble;
    }

    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        s_writeLogF("Discovered device: %s\n", advertisedDevice.toString());
        if (advertisedDevice.haveName() == false ||
            advertisedDevice.haveServiceUUID() == false)
        {
            s_writeLogLn("Skipping device as it doesn't have enough data.");
            return;
        }

        if (advertisedDevice.getName() == "Mousr" &&
            advertisedDevice.getServiceUUID().getNative() == serviceUuid.getNative())
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

    debugLogF("Got notification from characteristic: %s\n", characteristic->getUUID().toString().c_str());
    MousrData d(data, length);
    debugLogF("Message: %s\n", d.toString().c_str());

    if (notifyCallback != nullptr)
    {
        notifyCallback(characteristic, d);
    }
}

MousrBluetooth::MousrBluetooth()
{
    this->setConnectionStatus(MousrConnectionStatus::Disconnected);
    this->bleClient = BLEDevice::createClient();
    this->bleClient->setClientCallbacks(new MousrBluetoothClientCallback(this));
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

void MousrBluetooth::ConnectBluetooth()
{
    bool success = false;

    if (&this->device == nullptr ||
        this->getConnectionStatus() != MousrConnectionStatus::Discovered)
    {
        s_writeLogF("[ERROR] No device was discovered. Cannot connect. Device: %p Status: %d\n",
                    this->device,
                    this->getConnectionStatus());
        return;
    }

    s_writeLogF("Connecting to device: %s", this->device.toString());
    this->setConnectionStatus(MousrConnectionStatus::Connecting);
    this->bleClient->connect(&this->device);
    s_writeLogLn("Device connected");
    s_writeLogLn("Getting characteristics ...");

    BLERemoteService *service = bleClient->getService(serviceUuid);
    if (service == nullptr)
    {
        s_writeLogF("ERROR: Could not find service: %s\n", serviceUuid.toString().c_str());
        goto final;
    }

    uartWriteCharacteristic = service->getCharacteristic(uartWriteUuid);
    if (uartWriteCharacteristic == nullptr)
    {
        s_writeLogF("ERROR: Could not find characteristic: %s\n", uartWriteUuid.toString().c_str());
        goto final;
    }

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

final:
    if (success == false)
    {
        s_writeLogLn("[ERROR] Connection failed.");

        this->setConnectionStatus(MousrConnectionStatus::Error);

        if (bleClient != nullptr)
        {
            bleClient->disconnect();
        }

        return;
    }

    this->setConnectionStatus(MousrConnectionStatus::Connected);
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
    this->setConnectionStatus(MousrConnectionStatus::ScanStopped);
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
    s_writeLogF("Changing connection status from '%d' to '%d'", oldStatus, status);
    this->connectionStatus = status;

    if (statusChangeCallback != nullptr)
    {
        statusChangeCallback(oldStatus, status);
    }
}