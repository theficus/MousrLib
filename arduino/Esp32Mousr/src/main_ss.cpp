// This code is only for use with ESP chips. ESP's RTOS does not play nicely with performing operations against I2C from
// within an ISR. This can cause broken/unpredictable behavior when trying to use an interrupt from the Joy FeatherWing to
// alert the sketch that a controller button has been pressed. This sketch demonstrates one possible workaround for this by
// using Queues (see also: https://www.freertos.org/Embedded-RTOS-Queues.html)

#if !defined(ESP8266) && !defined(ESP32)
#error This sketch only supports ESP32 and ESP8266
#endif // ESP2866 / ESP32

#include "Adafruit_seesaw.h"

// This sketch requires that one of the optional interrupt pins on the Joy Featherwing is soldered closed. This value should match the
// GPIO pin on the ESP device. See: https://learn.adafruit.com/joy-featherwing/pinouts
#define IRQ_PIN 14 // Pin 14 is the pin directly to the left of the SCL pin on an ESP32

Adafruit_seesaw ss;

// GPIO pins on the Joy Featherwing. These should not be changed.
#define BUTTON_RIGHT 6
#define BUTTON_DOWN 7
#define BUTTON_LEFT 9
#define BUTTON_UP 10
#define BUTTON_SEL 14
#define STICK_H 2
#define STICK_V 3

// Optional calibration values to adjust for any potential misalignment when the analog stick is centered
#define STICK_H_CORRECTION 2
#define STICK_V_CORRECTION 0

// Optional drift values to detect movement of the stick
#define STICK_H_DRIFT 5
#define STICK_V_DRIFT 5

uint32_t button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
                       (1 << BUTTON_LEFT)  | (1 << BUTTON_UP)   | (1 << BUTTON_SEL);

QueueHandle_t buttonPressQueue; // Queue for notifying of button press changes

// ISR that gets triggered when a button is pressed.
void IRAM_ATTR onButtonPress()
{
    // The ISR just sends a signal to the queue
    int i = 0;
    if (!xQueueSend(buttonPressQueue, &i, (TickType_t)0))
    {
        Serial.println("WARNING: Could not queue message because queue is full.");
    }
#ifdef JOY_DEBUG
    else
    {
        Serial.println("Sent to queue");
    }
#endif // JOY_DEBUG
}

// Log the pressed buttons to the serial port
void outputPressedButtons(uint32_t mask)
{
#ifdef JOY_DEBUG
    Serial.print("Mask: ");
    Serial.println(mask, BIN);
#endif

    if (!(mask & (1 << BUTTON_RIGHT)))
    {
        Serial.println("Button A pressed");
    }
    if (!(mask & (1 << BUTTON_DOWN)))
    {
        Serial.println("Button B pressed");
    }
    if (!(mask & (1 << BUTTON_LEFT)))
    {
        Serial.println("Button Y pressed");
    }
    if (!(mask & (1 << BUTTON_UP)))
    {
        Serial.println("Button X pressed");
    }
    if (!(mask & (1 << BUTTON_SEL)))
    {
        Serial.println("Button SEL pressed");
    }
}

// Task that watches for analog stick changes
void analogStickTask(void *)
{
    Serial.println("analogStickTask() begin");

    int x = -1;
    int y = -1;

    while (true)
    {
        int new_x = -1;
        int new_y = -1;
        
        new_x = ss.analogRead(STICK_H) + STICK_H_CORRECTION;
        new_y = ss.analogRead(STICK_V) + STICK_V_CORRECTION;

        // Only log something if the position has actually changed
        if (new_x <= x - STICK_H_DRIFT ||
            new_x >= x + STICK_H_DRIFT ||
            new_y <= y - STICK_V_DRIFT ||
            new_y >= y + STICK_V_DRIFT)
        {
            x = new_x;
            y = new_y;
            Serial.printf("Analog stick position change!\n\tPosition: X=%d Y=%d\n\tAngle: X=%f Y=%f\n", 
                x, y, 
                (double)x / 4 - 128, (double)y / 4 - 128);
        }

        yield();
    }

    vTaskDelete(NULL);
}

// Queue consumer for responding to button presses
void buttonPressConsumer(void *)
{
    Serial.println("buttonPressConsumer() begin");
    while (true)
    {
        int i = 0;
        // This will yield until the queue gets signalled
        xQueueReceive(buttonPressQueue, &i, portMAX_DELAY);
        uint32_t v = 0;
        v = ss.digitalReadBulk(button_mask);
        outputPressedButtons(v);
    }

    vTaskDelete(NULL);
}

void setup()
{
    Serial.begin(115200);

    while (!Serial)
    {
        delay(10);
    }

    Serial.println("Joy FeatherWing example!");

    if (!ss.begin(0x49))
    {
        Serial.println("ERROR! seesaw not found");
        while (1)
        {
            delay(1);
        }
    }
    else
    {
        Serial.println("seesaw started");
        Serial.print("version: ");
        Serial.println(ss.getVersion(), HEX);
    }

    ss.pinModeBulk(button_mask, INPUT_PULLUP);
    ss.setGPIOInterrupts(button_mask, 1);
    pinMode(IRQ_PIN, INPUT);

    buttonPressQueue = xQueueCreate(10, sizeof(int));

    // Task for listening to button presses
    xTaskCreate(
        buttonPressConsumer,
        "ButtonPressConsumer",
        1000,
        NULL,
        1,
        NULL);

    // Task for watching the analog stick
    xTaskCreate(
        analogStickTask,
        "AnalogStickTask",
        1000,
        NULL,
        1,
        NULL);
    
    attachInterrupt(IRQ_PIN, onButtonPress, FALLING);
}

void loop()
{
    // Do nothing. Everything we're doing here is in a Task
    //yield();
}