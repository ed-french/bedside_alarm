#include <Arduino.h>

#include "alarm_state.h"

#ifndef REMOTE_LIGHT_H
#define REMOTE_LIGHT_H

class Light
{
    public:
        Light(int pin=-1);
        void begin(float starting_level);
        void set(float prop);
        void update(AlarmState *alarm_state);
};


#endif