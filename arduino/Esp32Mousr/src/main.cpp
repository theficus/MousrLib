#ifdef ARDUINO

#include "main.h"

void die()
{
    Serial.println("SYSTEM ERROR");
    while (1)
        ;
}

#ifdef _DO_OLED
void setupMessages()
{
    controllerMsg.message = OledMessage::ControllerDiag;
    controllerMsg.view = OledView::ControllerDiag;
    mousrMsg.message = OledMessage::Robot;
    mousrMsg.view = OledView::Robot;
    connectionMsg.message = OledMessage::Connecting;
    connectionMsg.view = OledView::Connection;
    connectionMsg.status = MousrConnectionStatus::Unknown;
}
#endif

void setup()
{
    Serial.begin(115200);
    i2cSemInit();

#ifdef _DO_OLED
    setupMessages();
    Oled *oled = Oled::getInstance();
    oled->u8g2.setBusClock(100000);
    oled->begin();
#endif // _DO_OLED

#ifdef _DO_SS
    Controller *c = Controller::getInstance();
    c->begin(JOYSTICK_INT_PIN);
    c->getStick()->calibrate(DRIFT_U, DRIFT_D, DRIFT_L, DRIFT_R);
    xTaskCreate(onButtonPressChangeTask, "mainButtonPress", 10000, NULL, 3, NULL);
    xTaskCreate(onAnalogStickChangeTask, "mainAnalogStick", 10000, NULL, 4, NULL);
#endif // _DO_SS

#ifdef WIRE_DEBUG
    startWireDebugTask(5000);
#endif // WIRE_DEBUG

#if _DO_OLED
    oled->setView(OledView::Connection);
    oled->lockView();
#endif

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

#if _DO_OLED
    oled->unlockView();

#if _DO_BLE
    oled->setView(OledView::Robot);
#else
    oled->setView(OledView::ControllerDiag);
#endif
#endif
}

void loop()
{
    // logMemory();
    // delay(5000);
    ;
}

#ifdef _DO_OLED
void displayButtonState(Oled *oled, const ButtonState &state)
{
    controllerMsg.buttonPress = state;
    oled->queueMessage(&controllerMsg);
}
#endif

#ifdef _DO_SS

#if _DO_OLED
OledView getNextView(OledView currentView)
{
    switch (currentView)
    {
    case OledView::ControllerDiag:
        return OledView::Robot;
    case OledView::Robot:
        return OledView::ControllerDiag;
    default:
        return currentView;
    }
}
#endif

void onButtonPressChangeTask(void *)
{
    s_println("main() starting onButtonPressChangeTask()");

    QueueHandle_t q = Controller::getInstance()->getButtons()->buttonPressQueue;
#ifdef _DO_OLED
    Oled *oled = Oled::getInstance();
#endif

#ifdef _DO_BLE
    MousrBluetooth *mb = MousrBluetooth::getInstance();
#endif

    while (true)
    {
        ButtonPressEvent e;
        if (xQueueReceive(q, &e, 5000) == false)
        {
            d_println(F("main() No button press triggered ..."));
            continue;
        }

        d_println(F("main() Got button press!"));
        ButtonStateChange state = ControllerButtons::getStateChange(e);
        onButtonPressStateChange(state);
#ifdef _DO_OLED
        displayButtonState(oled, state.cur);

        if (state.cur.sel == ButtonPress::Down)
        {
            oled->setView(getNextView(oled->getView()));
        }
#endif

#ifdef _DO_BLE
        if (mb->getConnectionStatus() == MousrConnectionStatus::Ready)
        {
            if (state.cur.d == ButtonPress::Down)
            {
                std::vector<uint8_t> data;
                MousrData::append(data, (uint8_t)MousrMessage::ROBOT_POSE);
                MousrData::append(data, (uint16_t)0);
                MousrData::append(data, (uint16_t)0);
                MousrData::append(data, (uint16_t)0);
                MousrData::append(data, (uint8_t)MousrCommand::RESET_HEADING);
                MousrData::append(data, false);
                MousrData msg = MousrData(data.data(), data.size());
                mb->sendMessage(msg);
            }

            if (state.cur.u == ButtonPress::Down)
            {
                std::vector<uint8_t> data;
                MousrData::append(data, (uint8_t)MousrMessage::ROBOT_POSE);
                MousrData::append(data, (uint16_t)0);
                MousrData::append(data, (uint16_t)0);
                MousrData::append(data, (uint16_t)0);
                MousrData::append(data, (uint8_t)MousrCommand::RESET_HEADING);
                MousrData::append(data, false);
                MousrData msg = MousrData(data.data(), data.size());
            }
        }
#endif
    }

    vTaskDelete(NULL);
}

void onAnalogStickChangeTask(void *)
{
    s_println("main() starting onAnalogStickChangeTask()");
#ifdef _DO_OLED
    Oled *oled = Oled::getInstance();
#endif
    Controller *c = Controller::getInstance();
    QueueHandle_t q = Controller::getInstance()->getStick()->stickQueue;
    ButtonState bs;

#ifdef _DO_OLED
    displayButtonState(oled, bs);
#endif

#ifdef _DO_BLE
    // MousrBluetooth *mb = MousrBluetooth::getInstance();
#endif

    int ctr = 0;
    while (true)
    {
        AnalogStickEvent e;
        if (xQueueReceive(q, &e, 5000) == false)
        {
            d_println("main() No analog stick movement ...");
            continue;
        }

        d_printf("main() Got analog stick movement! x=%d->%d y=%d->%d\n", e.prev_x, e.cur_x, e.prev_y, e.cur_y);

        AnalogStickMovement mov = c->getStick()->getMovement(e.cur_x, e.cur_y);
        AnalogStickMovement prev = c->getStick()->getMovement(e.prev_x, e.prev_y);
        onAnalogStickChange(prev, mov);

#ifdef _DO_OLED
        controllerMsg.stickPos = mov;
        oled->queueMessage(&controllerMsg);
#endif
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
#if _DO_OLED
    Oled *oled = Oled::getInstance();
#endif

    while ((s = mb->getConnectionStatus()) != status)
    {
        std::string actualStatus = getMousrConnectionStatusString(s);
        s_printf("Current status: %s ...\n", actualStatus.c_str());
#if _DO_OLED
        connectionMsg.status = status;
        oled->queueMessage(&connectionMsg);
#endif
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
        if (msg->movement.angle <= mousrMsg.mousrMove.angle - 1 ||
            msg->movement.angle >= mousrMsg.mousrMove.angle + 1)
        {
            mousrMsg.mousrMove = msg->movement;
            d_printf("[main] ROBOT_POSE: Speed=%f Angle=%f Tilt=%f\n", msg->movement.speed, msg->movement.angle, msg->movement.held);
            break;
        }
        d_printf("ignoring movement... %f -> %f", mousrMsg.mousrMove.angle, msg->movement.angle);
        return;
    case MousrMessage::BATTERY_VOLTAGE:
        mousrMsg.mousrBattery = msg->battery;
        d_printf("[main] BATTERY: Percent: %d%%\n", msg->battery.voltage);
        break;
    default:
        s_printf("[main] Got unknown Bluetooth notification for %s: %s\n", characteristic->getUUID().toString().c_str(), data.toString().c_str());
        return;
    }

    Oled::getInstance()->queueMessage(&mousrMsg);
}

#endif // _DO_BLE

#ifdef _DO_OLED

#endif // _DO_OLED

#endif // ARDUINO