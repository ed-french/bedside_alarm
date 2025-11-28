#include <Arduino.h>
#include "Arduino_GFX_Library.h"


#ifndef DISPLAY_H
#define DISPLAY_H

class Display
{
    private:
        char time_str[20];
        char alarm_str[20];
        uint8_t brightness;
        bool refresh_required;
        Arduino_GFX *gfx;
    public:
        Display();
        void begin();
        void setBrightness(uint8_t bright);
        void setTime(char * time_str);
        void getAlarm(char * alarm_str);
        void setAlarm(char * alarm_str);
        void update_display();

        

};

#endif // DISPLAY_H