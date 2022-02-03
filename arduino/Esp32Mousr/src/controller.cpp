#include "controller.h"

static uint32_t s_button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
                       (1 << BUTTON_LEFT) | (1 << BUTTON_UP) |
                       (1 << BUTTON_SEL);

static Adafruit_seesaw ss;

bool setupSeesaw()
{
    if (ss.begin(0x49) == false)
    {
        s_println("Could not set up JoyWing!");
        return false;
    }

    s_println("seesaw started");
    s_print("version: ");
    s_printlnb(ss.getVersion(), HEX);
    s_print("options: ");
    s_printlnb(ss.getOptions(), HEX);
    
    ss.pinModeBulk(s_button_mask, INPUT_PULLUP);
    ss.setGPIOInterrupts(s_button_mask, true);
    pinMode(JOY_IRQ_PIN, INPUT);
    attachInterrupt(JOY_IRQ_PIN, onButtonPress, FALLING);
    return true;
}

void onButtonPress()
{
    s_print("Read: ");
    s_printlnb(ss.digitalReadBulk(s_button_mask), BIN);
    //readSeesaw();
}

void readSeesaw()
{
    /*
    if (digitalRead(JOY_IRQ_PIN) == true)
    {
        // Serial.println("JOY_IRQ_PIN READ");
        return;
    }
    */

    // Transform from 0...1024 to -128...127
    int x = (ss.analogRead(JOYSTICK_H) / 4 - 128) + JOYSTICK_H_CORRECTION;
    int y = (ss.analogRead(JOYSTICK_V) / 4 - 128) + JOYSTICK_V_CORRECTION;

    uint32_t buttons = ss.digitalReadBulk(s_button_mask);
    s_printf("Analog: X: %d Y: %d\n", x, y);
    s_printf("Button: A: %d B: %d Y: %d X: %d SEL: %d\n",
                  buttons & (1 << BUTTON_RIGHT),
                  buttons & (1 << BUTTON_DOWN),
                  buttons & (1 << BUTTON_LEFT),
                  buttons & (1 << BUTTON_UP),
                  buttons & (1 << BUTTON_SEL));

    s_print("Button (mask):");
    s_printlnb(buttons, BIN);
}