#include <Arduino.h>
#include "simple_time.h"


#ifndef ALARM_STATE_H
#define ALARM_STATE_H

class AlarmState {
    public:
        AlarmState(void) {};
        Time alarm_time;
        bool override_active = false;
        Time now;
        float light_level = 0.0f;
        uint8_t backlight_brightness = 100;
        bool display_needs_update = true;

};




#endif // ALARM_STATE_H
