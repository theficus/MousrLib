#include "main.h"
// U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8_settings = U8X8_SSD1306_128X64_NONAME_HW_I2C(U8X8_PIN_NONE); // Settings mode

// Glyphs
#define ARROW_UP_FILL 0x25b2
#define ARROW_UP_EMPTY 0x25b3
#define ARROW_RIGHT_FILL 0x25b6
#define ARROW_RIGHT_EMPTY 0x25b7
#define ARROW_DOWN_FILL 0x25bc
#define ARROW_DOWN_EMPTY 0x25bd
#define ARROW_LEFT_FILL 0x25c0
#define ARROW_LEFT_EMPTY 0x25c1

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

// Seesaw
Adafruit_seesaw ss;
#define BUTTON_RIGHT 6
#define BUTTON_DOWN 7
#define BUTTON_LEFT 9
#define BUTTON_UP 10
#define BUTTON_SEL 14
#define JOYSTICK_H 2
#define JOYSTICK_V 3

// Calibration values
#define JOYSTICK_H_CORRECTION 2
#define JOYSTICK_V_CORRECTION 0

uint32_t button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
                       (1 << BUTTON_LEFT) | (1 << BUTTON_UP) |
                       (1 << BUTTON_SEL);

#define JOY_IRQ_PIN 14 // ESP32

// Other

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

void readSeeSaw()
{
    if (digitalRead(JOY_IRQ_PIN) == true)
    {
        // Serial.println("JOY_IRQ_PIN READ");
        return;
    }

    // Transform from 0...1024 to -128...127
    int x = (ss.analogRead(JOYSTICK_H) / 4 - 128) + JOYSTICK_H_CORRECTION;
    int y = (ss.analogRead(JOYSTICK_V) / 4 - 128) + JOYSTICK_V_CORRECTION;

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

void onButtonPress()
{
    Serial.println("press!");
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
    pinMode(JOY_IRQ_PIN, INPUT);
    attachInterrupt(JOY_IRQ_PIN, onButtonPress, FALLING);
    return true;
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

void setupOledLogView()
{
    u8x8_status.begin();
    u8x8_status.setFont(u8x8_font_chroma48medium8_r);
    setupOledLogDisplay(u8x8_status);
}

void setupOledMousrView()
{
    isOledLogEnabled = true;
    u8g2.begin();
    // writeOled("hello world");
}

void drawPosition()
{
    // u8g2.drawRBox(0, 0, 200, 100, 0);
    u8g2.drawCircle(60, 30, 20);
    u8g2.drawTriangle(20, 5, 27, 50, 5, 32);
}

void writeOled(const char *text)
{
    u8g2.firstPage();
    do
    {
        u8g2.setFont(u8g2_font_ncenB14_tf);
        u8g2.drawStr(0, 20, text);

        u8g2.setFont(u8g2_font_unifont_t_symbols);

        u8g2.drawGlyph(20, 40, ARROW_UP_FILL);
        u8g2.drawGlyph(20, 60, ARROW_DOWN_FILL);
        u8g2.drawGlyph(10, 50, ARROW_LEFT_FILL);
        u8g2.drawGlyph(30, 50, ARROW_RIGHT_FILL);

        drawPosition();
    } while (u8g2.nextPage());
}

void setup()
{
    isSeriaLogEnabled = true;
    Serial.begin(115200);
    semGive();
    // setupOled();
    setupOledLogView();
    // writeOled("hello world");
    setupSeeSaw();
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