#include <Arduino.h>


#include "pin_config.h"
#include <ESP32Encoder.h>

#include "simple_time.h"
#include "display.h"

#define ALARM_SETTER_UPDATE_INTERVAL_MS 60000 // periodically refresh from server

// uint8_t minutes_per_encoder_step=1;

class AlarmSetter {
    public:
        AlarmSetter();
        void begin(Time (* alarm_time_fetcher_fn)(void), \
                     bool (* alarm_time_setter_fn)(char * HH_MM_str), \
                     Display * display \
                    );
                     // Fetches the current alarm time and prepares for future changes
        void update(); // checks for updates, run several times per main loop
        Time get_alarm_time(); // returns the current alarm time (refreshing if needed)
        void get_alarm_time_str(char * buffer); // returns the current alarm time (refreshing if needed) but as a str
        bool set_alarm_time(Time alarm_time);
    private:
        uint32_t _last_update_millis;
        Time _alarm_time;
        ESP32Encoder _encoder;
        Display * _display;
        Time (* _alarm_time_fetcher_fn)(void);
        bool (* _alarm_time_setter_fn)(char * HH_MM_str);
        void _show_alarm_time(Time alarm_time);

};