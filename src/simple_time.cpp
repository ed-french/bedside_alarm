#include <Arduino.h>

#include "simple_time.h"


Time::Time(void) {};

void Time::set_from_HHMM_str(char * hhmm_str)
        {
            uint8_t hours=atoi(hhmm_str);
            uint8_t minutes=atoi(hhmm_str+3);
            this->hours=hours;
            this->minutes=minutes;
            this->seconds=0;
            this->encoder_offset_seconds=0;
            this->good_time=true;
            this->_last_changed_ms=millis();
        };
    
void Time::set_from_hms(uint8_t hours, uint8_t minutes, uint8_t seconds)
        {
            this->hours=hours;
            this->minutes=minutes;
            this->seconds=seconds;
            
            this->encoder_offset_seconds=0;
            this->good_time=true;
            this->_last_changed_ms=millis();
        };

void Time::set_from_hms_str(char * hms_str)
        {
            uint8_t hours=atoi(hms_str);
            uint8_t minutes=atoi(hms_str+3);
            uint8_t seconds=atoi(hms_str+6);
            this->hours=hours;
            this->minutes=minutes;
            this->seconds=seconds;
            this->encoder_offset_seconds=0;
            this->good_time=true;
            this->_last_changed_ms=millis();
        };

void Time::set_offset_seconds(int32_t encoder_offset_seconds)
        {
            this->encoder_offset_seconds=encoder_offset_seconds;
            this->_last_changed_ms=millis();
        }
int32_t Time::get_offset_seconds(void)
        {
            return this->encoder_offset_seconds;    
        }
void Time::consolidate_offset(void)
{
    Time temp_time=this->_self_with_offset();
    this->hours=temp_time.hours;
    this->minutes=temp_time.minutes;
    this->seconds=temp_time.seconds;
    this->encoder_offset_seconds=0;
}
Time Time::_self_with_offset(void)
        // Returns a time with the offset merged
        {
            Time new_time;
            new_time=this->_add_seconds_no_offset(this->encoder_offset_seconds);
            return new_time;
    
        }

Time Time::_add_seconds_no_offset(int32_t seconds)
        {
            Time new_time;
            uint32_t old_seconds=this->hours*3600+this->minutes*60+this->seconds;
            int32_t new_total_seconds=old_seconds+seconds;
            // If it's negative we have to wrap around
            while (new_total_seconds<0)
            {
                new_total_seconds=new_total_seconds+24*3600;
            }
            // Now split new_total_seconds into hours, minutes and seconds
            uint32_t new_hours=new_total_seconds/3600;
            uint32_t residual=new_total_seconds-3600*new_hours;
            uint32_t new_minutes=residual/60;
            uint32_t new_seconds=residual-new_minutes*60;
            new_hours%=24;
            new_time.set_from_hms(new_hours, new_minutes, new_seconds);
            return new_time;
        }

Time Time::add_seconds(int32_t seconds)
        {
            Time merged_offset=_self_with_offset();
            Time new_time=merged_offset._add_seconds_no_offset(seconds) ;
            return new_time;    
        }
void Time::set_bad(void)
        {
            this->good_time=false;
        }
uint32_t Time::HHMMSSacd(void)
        // Returns a 32 bit number representing HHMMSS
        {
            Time merged_offset=_self_with_offset();
            return merged_offset.hours*10000+merged_offset.minutes*100+merged_offset.seconds;
        }
uint32_t Time::total_seconds(void)
        {
            Time merged_offset=_self_with_offset();
            return merged_offset.hours*3600+merged_offset.minutes*60+merged_offset.seconds;
        }
void Time::c_str(char * buffer)
        {
            Time merged_offset=_self_with_offset();
            sprintf(buffer, "%02d:%02d:%02d", merged_offset.hours, merged_offset.minutes, merged_offset.seconds);
        }

void Time::HH_MM_str(char * buffer)
        {
            Time merged_offset=_self_with_offset();
            sprintf(buffer, "%02d:%02d", merged_offset.hours, merged_offset.minutes);
        }

// class Time
// {
//     private:
//         uint8_t hours;
//         uint8_t minutes;
//         uint8_t seconds;    
//         bool good_time;
//     public:
//         Time(void)
//         {
            
//         }
//         void set_from_HHMM_str(char * hhmm_str)
//         {
//             uint8_t hours=atoi(hhmm_str);
//             uint8_t minutes=atoi(hhmm_str+3);
//             this->hours=hours;
//             this->minutes=minutes;
//             this->seconds=0;
//             this->good_time=true;
//         }
//         void set_from_hms(uint8_t hours, uint8_t minutes, uint8_t seconds)
//         {
//             this->hours=hours;
//             this->minutes=minutes;
//             this->seconds=seconds;
//             this->good_time=true;
//         }
//         void set_from_hms_str(char * hms_str)
//         {
//             uint8_t hours=atoi(hms_str);
//             uint8_t minutes=atoi(hms_str+3);
//             uint8_t seconds=atoi(hms_str+6);
//             this->hours=hours;
//             this->minutes=minutes;
//             this->seconds=seconds;
//             this->good_time=true;
//         }
//         Time add_seconds(uint32_t seconds)
//         {
//             Time new_time;
//             uint32_t new_seconds=seconds+this->seconds;
//             uint32_t overflow_seconds=new_seconds-(new_seconds%60);
//             new_seconds=new_seconds%60;
//             uint32_t new_minutes=overflow_seconds/60+this->minutes;
//             uint32_t overflow_minutes=new_minutes-(new_minutes%60);
//             new_minutes=new_minutes%60;
//             uint32_t new_hours=overflow_minutes/60+this->hours;
//             new_time.set_from_hms(new_hours, new_minutes, new_seconds);
//             return new_time;
//         }
//         void set_bad(void)
//         {
//             this->good_time=false;
//         }
//         uint32_t HHMMSSacd(void)
//         // Returns a 32 bit number representing HHMMSS
//         {
//             return this->hours*10000+this->minutes*100+this->seconds;
//         }
//         uint32_t total_seconds(void)
//         {
//             return this->hours*3600+this->minutes*60+this->seconds;
//         }
//         void c_str(char * buffer)
//         {
//             sprintf(buffer, "%02d:%02d:%02d", this->hours, this->minutes, this->seconds);
//         }


// };
