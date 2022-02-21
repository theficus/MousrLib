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

#ifdef WIRE_DEBUG
    startWireDebugTask();
#endif // WIRE_DEBUG

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
    Oled::getInstance()->u8g2.setBusClock(100000);
    Oled::getInstance()->begin();
#endif // _DO_OLED

#ifdef _DO_SS
    Controller *c = Controller::getInstance();
    c->begin(JOYSTICK_INT_PIN);
    c->getStick()->calibrate(DRIFT_U, DRIFT_D, DRIFT_L, DRIFT_R);
    xTaskCreate(onButtonPressChangeTask, "mainButtonPress", 10000, NULL, 3, NULL);
    xTaskCreate(onAnalogStickChangeTask, "mainAnalogStick", 10000, NULL, 4, NULL);
#endif // _DO_SS
}

void loop()
{
    // logMemory();
    // delay(5000);
    ;
}

#ifdef _DO_SS
void displayButtonState(Oled* oled, const ButtonState &state)
{
    OledDisplayMessage m;
    m.message = OledMessage::ControllerDiagButton;
    //m.flags = OledDisplayFlags::Overlay;
    m.buttonPress = state;
    oled->queueMessage(&m);
}

void onButtonPressChangeTask(void *)
{
    s_println("main() starting onButtonPressChangeTask()");

    QueueHandle_t q = Controller::getInstance()->getButtons()->buttonPressQueue;
    Oled *oled = Oled::getInstance();

    while (true)
    {
        ButtonPressEvent e;
        if (xQueueReceive(q, &e, 5000) == false)
        {
            s_println(F("main() No button press triggered ..."));
            continue;
        }

        s_println(F("main() Got button press!"));
        ButtonStateChange state = ControllerButtons::getStateChange(e);
        onButtonPressStateChange(state);
        displayButtonState(oled, state.cur);
    }

    vTaskDelete(NULL);
}

void onAnalogStickChangeTask(void *)
{
    s_println("main() starting onAnalogStickChangeTask()");
    Oled *oled = Oled::getInstance();
    Controller *c = Controller::getInstance();
    QueueHandle_t q = Controller::getInstance()->getStick()->stickQueue;
    ButtonState bs;
    displayButtonState(oled, bs);
    int ctr = 0;
    while (true)
    {
        AnalogStickEvent e;
        if (xQueueReceive(q, &e, 5000) == false)
        {
            s_println("main() No analog stick movement ...");
            continue;
        }

        s_printf("main() Got analog stick movement! x=%d->%d y=%d->%d\n", e.prev_x, e.cur_x, e.prev_y, e.cur_y);

        AnalogStickMovement mov = c->getStick()->getMovement(e.cur_x, e.cur_y);
        AnalogStickMovement prev = c->getStick()->getMovement(e.prev_x, e.prev_y);
        onAnalogStickChange(prev, mov);
        OledDisplayMessage m;
        m.message = OledMessage::ControllerDiagStick;
        m.stickPos = mov;
        oled->queueMessage(&m);
        ctr += 10;
    }

    vTaskDelete(NULL);
}

#define getButtonState(s) s == ButtonPress::Up ? "UP" : "DOWN"
void onButtonPressStateChange(ButtonStateChange state)
{
    s_printf("[main] Buttons pressed! u=%s->%s d=%s->%s l=%s->%s r=%s->%s sel=%s->%s\n",
             getButtonState(state.prev.u),
             getButtonState(state.cur.u),
             getButtonState(state.prev.d),
             getButtonState(state.cur.d),
             getButtonState(state.prev.l),
             getButtonState(state.cur.l),
             getButtonState(state.prev.r),
             getButtonState(state.cur.r),
             getButtonState(state.prev.sel),
             getButtonState(state.cur.sel));
}

void onAnalogStickChange(AnalogStickMovement prev, AnalogStickMovement cur)
{
    s_printf("[main] Analog stick moved! x=%f->%f y=%f->%f angle=%f->%f velocity=%f->%f isCentered=%d->%d\n",
             prev.x,
             cur.x,
             prev.y,
             cur.y,
             prev.angle,
             cur.angle,
             prev.velocity,
             cur.velocity,
             prev.isCentered,
             cur.isCentered);
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