#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "alarm_state.h"


#ifndef DISPLAY_H
#define DISPLAY_H

class Display
{
    private:
        // char time_str[20];
        // char alarm_str[20];
        // uint8_t brightness;
        // bool refresh_required;
        Arduino_GFX *gfx;
        
    public:
        Display();
        void begin();
        // void _setBrightness(uint8_t bright);
        // void _setTime(char * time_str);
        // void _getAlarm(char * alarm_str);
        // void _setAlarm(char * alarm_str);
        void update_display(AlarmState *alarm_state );

        

};

#endif // DISPLAY_H