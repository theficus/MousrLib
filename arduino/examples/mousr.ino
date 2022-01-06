#include <Print.h>

// OLED
#include <U8g2lib.h>
U8X8_SSD1306_64X32_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 

// BLE
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// BLE characteristics
BLEUUID serviceUuid("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
BLEUUID txUuid("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
BLEUUID rxUuid("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

BLEScan* bleScan;
BLEClient* bleClient;
BLEAdvertisedDevice* device;
BLERemoteCharacteristic* txCharacteristic;
BLERemoteCharacteristic* rxCharacteristic;
bool isConnected = false;

// Seesaw
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;
#define BUTTON_RIGHT 6
#define BUTTON_DOWN 7
#define BUTTON_LEFT 9
#define BUTTON_UP 10
#define BUTTON_SEL 14
uint32_t button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) | 
                       (1 << BUTTON_LEFT) | (1 << BUTTON_UP) | 
                       (1 << BUTTON_SEL);
                
#define IRQ_PIN   14 // ESP32

// Other
SemaphoreHandle_t waitHandle = xSemaphoreCreateMutex();

// Callback for discovering BLE devices
class BleScanCallback : public BLEAdvertisedDeviceCallbacks 
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        Serial.printf("Discovered device: %s\n", advertisedDevice.toString().c_str());
        if (advertisedDevice.getName() == "Mousr") {
            device = &advertisedDevice;
            Serial.println("Stopping BLE scan");
            bleScan->stop();
            bleScan->clearResults();
            
            xSemaphoreGive(waitHandle);
        }
    }
};

class BleClientCallback : public BLEClientCallbacks 
{
    void onConnect(BLEClient* client)
    {
    }

    void onDisconnect(BLEClient* client) 
    {
        isConnected = false;
    }
};

void txNotifyCallback(
    BLERemoteCharacteristic* characteristic,
    uint8_t* data,
    size_t length,
    bool isNotify)
{
    Serial.printf("Got notification from characteristic %s\n", characteristic->getUUID().toString().c_str());
    Serial.printf("Length: %zu Data: ", length);
    printData(data, length);
}

void setup() 
{
    Serial.begin(115200);

    setupOled();
    setupSeeSaw();

    startBleScan();
    discoverDevice();
    isConnected = setupDevice();
}

void loop() 
{
    readSeeSaw();
}

void die()
{
    Serial.println("SYSTEM ERROR");
    while(1);
}

void reset()
{
    // TODO: Clean up and reset. Also restart scan?
}

void readSeeSaw()
{
    if (digitalRead(IRQ_PIN) == true) 
    {
        return;
    }

    int x = ss.analogRead(2);
    int y = ss.analogRead(3);
    uint32_t buttons = ss.digitalReadBulk(button_mask);
    Serial.printf("Analog: X: %d Y: %d\n", x, y);
    Serial.printf("Button: A: %d B: %d Y: %d X: %d SEL: %d\n", 
        buttons & (1 << BUTTON_RIGHT),
        buttons & (1 << BUTTON_DOWN),
        buttons & (1 << BUTTON_LEFT),
        buttons & (1 << BUTTON_UP),
        buttons & (1 << BUTTON_SEL));

    Serial.print("Button (mask):");
    Serial.println(buttons, BIN);
}

bool setupSeeSaw() 
{
    if (ss.begin(0x49) == false)
    {
        Serial.println("Could not set up JoyWing!");
        return false;
    }
   
    Serial.println("seesaw started");
    Serial.print("version: ");
    Serial.println(ss.getVersion(), HEX);
    ss.pinModeBulk(button_mask, INPUT_PULLUP);
    ss.setGPIOInterrupts(button_mask, 1);
    pinMode(IRQ_PIN, INPUT);
    return true;
}

void discoverDevice() 
{
    Serial.println("Waiting for device to be discovered...");
    xSemaphoreTake(waitHandle, portMAX_DELAY);
    Serial.printf("Got device: Name: %s Address: %s RSSI: %d", 
        device->getName(), 
        device->getAddress().toString().c_str(), 
        device->getRSSI());
}

bool setupDevice() 
{
    bool success = false;
    Serial.println("Connecting to device ...");
    bleClient = BLEDevice::createClient();
    bleClient->setClientCallbacks(new BleClientCallback());
    bleClient->connect(device);

    Serial.println("Waiting for device to connect...");
    xSemaphoreTake(waitHandle, portMAX_DELAY);
    Serial.println("Device connected!");
    Serial.println("Getting characteristics ...");
    BLERemoteService* remoteService = bleClient->getService(serviceUuid);
    if (remoteService == nullptr)
    {
        Serial.printf("ERROR: Could not find service UUID: %s\n", serviceUuid.toString().c_str());
        goto final;
    }

    // TX characteristic
    txCharacteristic = remoteService->getCharacteristic(txUuid);
    if (txCharacteristic == nullptr)
    {
        Serial.printf("ERROR: Could not find characteristic: %s\n", txCharacteristic->toString().c_str());
        goto final;
    }

    if (txCharacteristic->canNotify() == false) 
    {
        Serial.printf("ERROR: Characteristic %s does not have notify capabilities!\n", txCharacteristic->toString().c_str());
        goto final;
    }

    txCharacteristic->registerForNotify(txNotifyCallback);

    // RX characteristic
    rxCharacteristic = remoteService->getCharacteristic(rxUuid);
    if (rxCharacteristic == nullptr)
    {
        Serial.printf("ERROR: Could not find characteristic: %s\n", rxCharacteristic->toString().c_str());
        goto final;
    }

    final:

    if (success == false &&
        bleClient != nullptr)
    {
        Serial.println("Disconnecting device because of a setup failure.");
        bleClient->disconnect();
    }

    return success;
}

void startBleScan() {
    Serial.println("Setting up BLE for scan ...");

    BLEDevice::init("");
    bleScan = BLEDevice::getScan();
    bleScan->setAdvertisedDeviceCallbacks(new BleScanCallback());
    bleScan->setActiveScan(true);
    bleScan->setInterval(100);
    bleScan->setWindow(99);
}

void setupOled() 
{
    Serial.println("Setting up OLED display");
    u8x8.begin();
    u8x8.setPowerSave(0);
}

// Print the data as a hex string
void printData(uint8_t* data, size_t length)
{
    Serial.print("0x");
    for (int i = 0; i < length; i++) 
    {
        Serial.printf("%x", (&data + i));
    }

    Serial.println();
}
