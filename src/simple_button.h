#include <Arduino.h>

#ifndef SIMPLE_BUTTON_H
#define SIMPLE_BUTTON_H
class SimpleButton {
    public:
        SimpleButton(uint8_t pin,bool pressed_state_high=true);
        bool is_pressed(void);
        bool toggling_state=false;
        void print_state(void);
        uint32_t toggle_timeout_ms;
    private:
        uint8_t pin;
        uint32_t last_valid_change_time=0;
        bool last_stable_state=false;
        bool pressed_state_high=true;
        const uint32_t DEBOUNCE_TIME_MS=50;
        const uint32_t TOGGLE_TIMEOUT_MS=30*60*60*1000;
        
};

#endif // SIMPLE_BUTTON_H