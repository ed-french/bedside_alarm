#include <Arduino.h>
#include "simple_time.h"


#include "alarm_setter.h"




AlarmSetter::AlarmSetter() {}

#define SECONDS_PER_ENCODER_COUNT 75
#define TWIRL_IS_PAUSED_THRESHOLD_MS 2000

void AlarmSetter::begin(Time (* alarm_time_fetcher_fn)(void), \
                     bool (* alarm_time_setter_fn)(char * HH_MM_str), \
                     Display * display)
{
    this->_alarm_time_fetcher_fn=alarm_time_fetcher_fn;
    this->_alarm_time_setter_fn=alarm_time_setter_fn;
    this->_display=display;
    _encoder.attachFullQuad(PIN_QUAD_A,PIN_QUAD_B);
    _encoder.setCount(0);
    _alarm_time=_alarm_time_fetcher_fn();
    _last_update_millis=millis();
    _show_alarm_time(_alarm_time);
}

void AlarmSetter::_show_alarm_time(Time alarm_time)
{
    char alarm_buffer[20];
    alarm_time.HH_MM_str(alarm_buffer);
    _display->setAlarm(alarm_buffer);

}

void AlarmSetter::update()
{
    static int32_t last_encoder_count=0;
    static uint32_t last_twirl_ms=millis();

    // Check if the knob has been turned
    // if it has then we need to update the display

    if (last_encoder_count!=_encoder.getCount())
    {
        Serial.printf("Encoder position >>>>>>>>>>>>>>>>: %d\n",_encoder.getCount());
        Serial.println("************ TWIRL DETECTED **************");
        _show_alarm_time(_alarm_time);
        last_encoder_count=_encoder.getCount();
        last_twirl_ms=millis();
        _alarm_time.set_offset_seconds(_encoder.getCount()*SECONDS_PER_ENCODER_COUNT);
        _display->update_display();
    }
    
    // Periodically we want to fetch the alarm time from the server
    // but only if the knob hasn't been touched for a while
    int32_t time_since_last_twirl_ms=millis()-last_twirl_ms;
    if(millis()-_last_update_millis>ALARM_SETTER_UPDATE_INTERVAL_MS && time_since_last_twirl_ms>(2*TWIRL_IS_PAUSED_THRESHOLD_MS))
    {
        Serial.println("FETCHING ALARM TIME FROM THE SERVER WHILST IT'S ALL QUIET");
        _alarm_time=_alarm_time_fetcher_fn();
        _last_update_millis=millis();
        _show_alarm_time(_alarm_time);
        _encoder.setCount(0); // reset the encoder
    }
    
    

    // If it's been a while since the last twirl,
    //but we have twirled, 
    //then we can send the new position to the server
    if (time_since_last_twirl_ms>TWIRL_IS_PAUSED_THRESHOLD_MS && last_encoder_count!=0)
    {
        // Send the new alarm time
        char temp_alarm_time_buf[20];
        _alarm_time.HH_MM_str(temp_alarm_time_buf);
        Serial.printf(".///// About to send a twirled alarm time to server of :\n\t\t %s\n",temp_alarm_time_buf);
        _alarm_time_setter_fn(temp_alarm_time_buf);
        _alarm_time.consolidate_offset();
        _encoder.setCount(0);


    }

}

Time AlarmSetter::get_alarm_time()
{
    this->update();
    return _alarm_time;
}

void AlarmSetter::get_alarm_time_str(char * buffer)
{
    this->update();
    _alarm_time.c_str(buffer);
}


bool AlarmSetter::set_alarm_time(Time alarm_time)
{
    char alarm_buffer[20];
    alarm_time.HH_MM_str(alarm_buffer);
    bool success=_alarm_time_setter_fn(alarm_buffer);
    _display->setAlarm(alarm_buffer);
    return success;
}