#include <Arduino.h>

#include "simple_button.h"


SimpleButton::SimpleButton(uint8_t pin,bool pressed_state_high)
{
    this->pin=pin;
    pinMode(pin, INPUT_PULLUP);
    this->last_stable_state=!pressed_state_high; // assume not pressed at start
    this->pressed_state_high=pressed_state_high;
    this->toggling_state=false;
}

bool SimpleButton::is_pressed(void)
{
    // Cancel toggle if timeout exceeded
    if (this->toggling_state && (millis()>this->toggle_timeout_ms))
    {
        Serial.println("Toggle timeout exceeded, resetting toggle state");
        this->toggling_state=false;
    }
    bool recently_changed=millis()-this->last_valid_change_time<DEBOUNCE_TIME_MS;
    bool raw=digitalRead(pin);
    bool read_button_now=(raw==this->pressed_state_high);
    // Serial.printf("Raw: %d\t",raw);
    if (read_button_now!=this->last_stable_state )//
    {
        this->last_valid_change_time=millis();
        if (!this->last_stable_state && read_button_now)
        {
            Serial.println("Button pressed state change detected, toggling");
            this->toggling_state=!this->toggling_state;
            if (this->toggling_state)
            {
                this->toggle_timeout_ms=millis()+TOGGLE_TIMEOUT_MS;
            }
        }
        this->last_stable_state=read_button_now;

        
    }
    return this->last_stable_state;
}

void SimpleButton::print_state(void)
{
    Serial.printf("Button stable state: %d, toggling state: %d, last changed mills: %d\n",this->last_stable_state,this->toggling_state,this->last_valid_change_time);
}