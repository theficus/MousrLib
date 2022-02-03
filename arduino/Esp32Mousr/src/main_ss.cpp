#include "Adafruit_seesaw.h"

#define BUTTON_RIGHT 6
#define BUTTON_DOWN 7
#define BUTTON_LEFT 9
#define BUTTON_UP 10
#define BUTTON_SEL 14
#define JOYSTICK_H 2
#define JOYSTICK_V 3
#define JOY_IRQ_PIN 14

static portMUX_TYPE joyReadMutex = portMUX_INITIALIZER_UNLOCKED;
static uint32_t s_button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_UP) | (1 << BUTTON_SEL);
Adafruit_seesaw ss;
QueueHandle_t joyQueue;
size_t queueSize = 10;

static void onButtonPress()
{
    Serial.println("queueing...");
    int i = 0;
    xQueueSend(joyQueue, &i, portMAX_DELAY);
    /*
    //ss.begin(0x49, -1, false);
    //Serial.printf("onButtonPress() ss: %p\n", ss);
    Serial.println("onButtonPress() Read:");
    Serial.println(ss.digitalReadBulk(s_button_mask), BIN);
    Serial.printf("onButtonPress() ss: %p\n", ss);
    Serial.println(ss.digitalReadBulk(s_button_mask), BIN);
    */
}

void joyTaskConsumer(void* p)
{
    Serial.println("In queue!");
    int element;
    size_t i = 0;
    while(true)
    {
        Serial.printf("q=%zu\n", i);
        xQueueReceive(joyQueue, &element, portMAX_DELAY);
        uint32_t v = 0;
        
        portENTER_CRITICAL(&joyReadMutex);
        v = ss.digitalReadBulk(s_button_mask);
        portEXIT_CRITICAL(&joyReadMutex);
        Serial.println(v, BIN);
        i++;
    }
    vTaskDelete(NULL);
}

void setup()
{
    Serial.begin(115200);

    joyQueue = xQueueCreate(queueSize, sizeof(int));

    ss.begin(0x49);
    Serial.printf("setup() ss: %p\n", ss);
    ss.pinModeBulk(s_button_mask, INPUT_PULLUP);
    ss.setGPIOInterrupts(s_button_mask, true);
    pinMode(JOY_IRQ_PIN, INPUT);
    Serial.println("Initial read:");
    //onButtonPress(); // Log initial state
    
    xTaskCreate(
        joyTaskConsumer,
        "JoyTaskConsumer",
        10000,
        NULL,
        1,
        NULL);

    attachInterrupt(JOY_IRQ_PIN, onButtonPress, FALLING);
    Serial.println("setup() done");
}

void loop()
{
    ;
}