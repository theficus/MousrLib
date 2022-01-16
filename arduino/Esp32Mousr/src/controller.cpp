#include "controller.h"

bool setupSeesaw()
{
    if (ss.begin(0x49) == false)
    {
        Serial.println("Could not set up JoyWing!");
        return false;
    }

    Serial.println("seesaw started");
    Serial.print("version: ");
    Serial.println(ss.getVersion(), HEX);
    ss.pinModeBulk(s_button_mask, INPUT_PULLUP);
    ss.setGPIOInterrupts(s_button_mask, 1);
    pinMode(JOY_IRQ_PIN, INPUT);
    attachInterrupt(JOY_IRQ_PIN, onButtonPress, FALLING);
    return true;
}

void onButtonPress()
{
    Serial.println("press!");
}

void readSeesaw()
{
    if (digitalRead(JOY_IRQ_PIN) == true)
    {
        // Serial.println("JOY_IRQ_PIN READ");
        return;
    }

    // Transform from 0...1024 to -128...127
    int x = (ss.analogRead(JOYSTICK_H) / 4 - 128) + JOYSTICK_H_CORRECTION;
    int y = (ss.analogRead(JOYSTICK_V) / 4 - 128) + JOYSTICK_V_CORRECTION;

    uint32_t buttons = ss.digitalReadBulk(s_button_mask);
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