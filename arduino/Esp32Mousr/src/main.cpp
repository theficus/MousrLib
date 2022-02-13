#ifdef ARDUINO

#include "main.h"

void die()
{
    Serial.println("SYSTEM ERROR");
    while (1)
        ;
}

int counter = 0;
void updateOledTask(AnalogStickMovement &stick)
{
    i2cSemTake();
    s_println("Updating OLED...");
    //2*pi * (x / 360).
    
    float angle = stick.angle;
    s_printf("stick angle: %f -> %f\n", stick.angle, angle);
    u8g2.clearBuffer();
    drawBattery(u8g2, 100, 2, 24, 10, 5, counter % 6);
    drawPos(u8g2, 30, 30, 20, angle, stick.isCentered);
    // drawAngle(u8g2, 40, 40, 20, 8, counter % 45);
    drawDetails(u8g2, 55, 40, stick.isCentered ? 0 : angle, (float)(counter % 90), stick.isCentered ? 0 : stick.velocity);
    u8g2.sendBuffer();
    i2cSemGive();
    counter++;
}

void setup()
{
    Serial.begin(115200);

    i2cSemInit();

#ifdef _DO_BLE
    // Set up Bluetooth
    MousrBluetooth *mb = MousrBluetooth::getInstance();
    mb->init();
    mb->setConnectionStatusChangeCallback(onBluetoothStatusChange);
    mb->setMousrNotificationCallback(onBluetoothNotify);
    mb->startScan();
    waitForStatus(MousrConnectionStatus::Discovered);
    mb->connect();
    waitForStatus(MousrConnectionStatus::Connected);
    if (mb->discoverCapabilities() == false)
    {
        die();
    }
#endif // _DO_BLE

#ifdef _DO_OLED
    u8g2.begin();
    u8g2.clear();
    // xTaskCreate(updateOledTask, "oledTask", 10000, NULL, 10, NULL);
#endif // _DO_OLED

#ifdef _DO_SS
    Controller *c = Controller::getInstance();
    c->begin(onButtonPressStateChange, onAnalogStickMovement, JOYSTICK_INT_PIN);
    c->calibrate(DRIFT_U, DRIFT_D, DRIFT_L, DRIFT_R);
#endif // _DO_SS

}

void loop()
{
    ;
}

#ifdef _DO_SS
#define getButtonState(s) s == ButtonPressState::Up ? "UP" : "DOWN"
void onButtonPressStateChange(ButtonStateChange press)
{
    s_printf("[main] Buttons pressed! u=%s->%s d=%s->%s l=%s->%s r=%s->%s sel=%s->%s\n",
             getButtonState(press.oldState.u),
             getButtonState(press.newState.u),
             getButtonState(press.oldState.d),
             getButtonState(press.newState.d),
             getButtonState(press.oldState.l),
             getButtonState(press.newState.l),
             getButtonState(press.oldState.r),
             getButtonState(press.newState.r),
             getButtonState(press.oldState.sel),
             getButtonState(press.newState.sel));
}

void onAnalogStickMovement(AnalogStickMovement move)
{
    s_printf("[main] Stick moved! ctr=%s angle=%f velocity=%f\n",
             move.isCentered ? "TRUE" : "FALSE",
             move.angle,
             move.velocity);
#ifdef _DO_OLED
    updateOledTask(move);
#endif
}
#endif // _DO_SS

#ifdef _DO_BLE
// TODO: Add timeout
void waitForStatus(MousrConnectionStatus status)
{
    std::string expectedStatus = getMousrConnectionStatusString(status);
    s_printf("Waiting for status: %s\n", expectedStatus.c_str());
    MousrConnectionStatus s;
    MousrBluetooth *mb = MousrBluetooth::getInstance();
    while ((s = mb->getConnectionStatus()) != status)
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

#ifdef _DO_OLED

#endif // _DO_OLED

#endif // ARDUINO