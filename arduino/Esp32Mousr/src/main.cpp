#ifdef ARDUINO

#include "main.h"

void die()
{
    Serial.println("SYSTEM ERROR");
    while (1)
        ;
}

void setup()
{
    Serial.begin(115200);

#ifdef _DO_BLE
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
#endif // _DO_BLE

#ifdef _DO_SS
    c.begin(onButtonPressStateChange, onAnalogStickMovement);
    c.calibrate(DRIFT_U, DRIFT_D, DRIFT_L, DRIFT_R);
#endif // _DO_SS
}

void loop()
{
    // onButtonPress();
    ;
    logMemory();
    sleep(1);
}

#ifdef _DO_SS
#define getButtonState(s) s == ButtonPressState::Up ? "UP" : "DOWN"
void onButtonPressStateChange(ButtonState press)
{
    s_printf("[main] Buttons pressed! x=%s y=%s a=%s b=%s sel=%s\n",
             getButtonState(press.x),
             getButtonState(press.y),
             getButtonState(press.a),
             getButtonState(press.b),
             getButtonState(press.sel));
}

void onAnalogStickMovement(AnalogStickMovement move)
{
    s_printf("[main] Stick moved! ctr=%s angle=%f velocity=%f\n",
             move.isCentered ? "TRUE" : "FALSE",
             move.angle,
             move.velocity);
}
#endif // _DO_SS

#ifdef _DO_BLE
// TODO: Add timeout
void waitForStatus(MousrConnectionStatus status)
{
    std::string expectedStatus = getMousrConnectionStatusString(status);
    s_printf("Waiting for status: %s\n", expectedStatus.c_str());
    MousrConnectionStatus s;
    while ((s = mb.getConnectionStatus()) != status)
    {
        std::string actualStatus = getMousrConnectionStatusString(s);
        s_printf("Current status: %s ...\n", actualStatus.c_str());
        sleep(1);
    }
}

static void onBluetoothStatusChange(MousrConnectionStatus oldStatus, MousrConnectionStatus newStatus)
{
    s_printf("[main] Got status change: %d -> %d\n", (int)oldStatus, (int)newStatus);
}

static void onBluetoothNotify(BLERemoteCharacteristic *characteristic, MousrData &data)
{
    // s_printf("[main] Got Bluetooth notification for %s: %s\n", characteristic->getUUID().toString().c_str(), data.toString().c_str());
    MousrMessageData *msg = data.getMessageData();
    switch (data.getMessageKind())
    {
    case MousrMessage::ROBOT_POSE:
        s_printf("[main] ROBOT_POSE: Speed=%f Angle=%f Tilt=%f\n", msg->movement.speed, msg->movement.angle, msg->movement.held);
        break;
    case MousrMessage::BATTERY_VOLTAGE:
        s_printf("[main] BATTERY: Percent: %d%%\n", msg->battery.voltage);
        break;
    default:
        s_printf("[main] Got unknown Bluetooth notification for %s: %s\n", characteristic->getUUID().toString().c_str(), data.toString().c_str());
    }
}

#endif // _DO_BLE

#endif // ARDUINO