#include <Arduino.h>
#ifndef TIME_H
#define TIME_H


class Time {
public:
    Time();
    void set_from_HHMM_str(char * hhmm_str);
    void set_from_hms(uint8_t hours, uint8_t minutes, uint8_t seconds);
    void set_from_hms_str(char * hms_str);
    Time add_seconds(int32_t seconds);
    void set_bad(void);
    uint32_t HHMMSSacd(void);
    uint32_t total_seconds(void);
    void c_str(char * buffer);
    void HH_MM_str(char * buffer);
    void set_offset_seconds(int32_t offset);
    int32_t get_offset_seconds(void);
    void consolidate_offset(void);

private:
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;    
        bool good_time;
        int32_t encoder_offset_seconds;
        Time _self_with_offset(void);
        Time _add_seconds_no_offset(int32_t seconds);
        uint32_t _last_changed_ms;
};

#endif // TIME_H
