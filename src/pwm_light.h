#include <Arduino.h>
#include "alarm_state.h"

#define BIT_WIDTH 13
#define PWM_CHANNEL 3

class PWM_Light
{

    public:
        PWM_Light(int pin)
        {
            _pin = pin;
            _value = 128;

        }
        void begin(float starting_level)
        {
            this->_value = starting_level;
            int pin = _pin;
            pinMode(pin, OUTPUT);
            //Set up arduino PWM
            Serial.printf("Setup result for PWM: %d\n",ledcSetup(PWM_CHANNEL, 1000, BIT_WIDTH)); 

            
            ledcAttachPin(pin, PWM_CHANNEL);

            _set(this->_value);
            Serial.printf("Frequency check for PWM: %d\n",ledcReadFreq(PWM_CHANNEL));          // actual freq
        }

   
        void _set(float prop)
        {
            _value = prop;
            uint16_t pwm_setting = prop_to_level(prop);
            // Serial.printf("For prop level of %.02f, setting PWM to %d\n",_value, pwm_setting);
            ledcWrite(PWM_CHANNEL, pwm_setting);
        }
        // void inc(void)
        // {
        //     _value+=0.01;
        //     if (_value>1) _value=0.0; // wrap for now (only used for testing anyway?)
        //     set(_value);
        // }
        void update(AlarmState *alarm_state)
        {
            float probable_light_level=alarm_state->light_level;
            // Test if we should be overriding the light level

            if (alarm_state->override_active)
            { 
                // // First check if we should timeout the override
                // Time now=alarm_state->now;
                // if (now.total_seconds()>=alarm_state->override_timeout.total_seconds())
                // {
                //     Serial.println("Override timeout reached, disabling override");
                //     alarm_state->override_active=false;
                //     _set(alarm_state->light_level);
                //     return;
                // }
                // Figure out the over-ridden light level (the opporsite of the expected level)
                // i.e. if the alarm light is off, then it should be overridden to nearly on

                float actual_light_level=probable_light_level>0.01f ? 0.01f : 0.7f;
                alarm_state->light_level=actual_light_level;
                Serial.printf("Override active, setting light level to %.02f\n",actual_light_level);
                _set(actual_light_level);
                return;
            }
            _set(alarm_state->light_level);
        }


    private:
        int _pin;
        float _value;
        uint16_t light_levels[25] = { \
          0,     10,    20,    50, \
          100,   200,   400, \
          700,   900,   1000,  1200, \
          1600,  1950, \
          2400,  3000,  3500,  4000, \
          5000,  6000,  7000,  8500, \
          10000, 15000, 20000, \
           65535
        };
        
        uint16_t prop_to_level(float prop)
        {
            // linearly interpolates between two values
            uint8_t level_count=sizeof(light_levels)/2; // say 25 levels (0-24)
            uint16_t lower_index=int(prop*(level_count-1)); // at 100% we have 24, at 0% we have 0
            if (lower_index>(level_count-2)) 
            {
                return light_levels[level_count-1];// we hit the maximum
            }
            uint8_t upper_index=lower_index+1;
            float decimal_index=prop*(level_count-1);
            float sub_proportion=decimal_index-lower_index;// 0-1
            float lower_value=light_levels[lower_index];
            float upper_value=light_levels[upper_index];
            float interpolated_value=lower_value+(sub_proportion*(upper_value-lower_value));
            uint8_t right_shifts=16-BIT_WIDTH-1;
            return int(interpolated_value)>>right_shifts; // divide by 8 to allow for lower bit resolution on ESP32 S3
        
        }
};