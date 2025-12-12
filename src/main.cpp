#include <Arduino.h>


#include "live_time.h"
#include "wifi_helper.h"

#if USE_EXTERNAL_LIGHT
#include "remote_light.h"
#else
#include "pwm_light.h"
#endif

#include "alarm_setter.h"




#include "display.h"

#include "pin_config.h"

#include "alarm_state.h"

#include "simple_button.h"



#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#error  "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif



#define START_LIGHT_WINDOW_SECONDS (40*60)
#define END_LIGHT_WINDOW_SECONDS (10*60)




#define BACKLIGHT_MINIMUM_BRIGHTNESS 20
#define BACKLIGHT_MAXIMUM_BRIGHTNESS 255

#define BACKLIGHT_DARK_UNTIL_SECONDS (6*60*60) // 6am
#define BACKLIGHT_MAX_BRIGHTNESS_SECONDS (9*60*60) // 9am
#define BACKLIGHT_START_DIMMING_SECONDS (20*60*60) // 8pm 
#define BACKLIGHT_FULLY_DIMMED_SECONDS (22*60*60) // 10pm







AlarmState alarm_state=AlarmState();



Display disp=Display();

SimpleButton knob_press=SimpleButton(PIN_KNOB_BUTTON,LOW); // Active high button



float calc_light_level(Time now,Time alarm_time)
{
  uint32_t seconds_now=now.total_seconds();
  uint32_t alarms_seconds=alarm_time.total_seconds();
  int32_t seconds_to_max=alarms_seconds-seconds_now;
  Serial.printf("seconds_to_max: %d\n",seconds_to_max);
  if (seconds_to_max>START_LIGHT_WINDOW_SECONDS)
  {
    Serial.println("Too early to alarm");
    return 0.0; // Long while to alarm
  } 
  if (seconds_to_max<-END_LIGHT_WINDOW_SECONDS)
  {
    // Serial.println("Too late to alarm");
    return 0.0;
  } 
  if (seconds_to_max>0)
  {
    Serial.println("Early phase before alarm");
    return (1-seconds_to_max/(float)START_LIGHT_WINDOW_SECONDS);
  }
  return 1+seconds_to_max/(float)END_LIGHT_WINDOW_SECONDS;

}

uint8_t calc_backlight_level(Time now)
{
  uint32_t seconds_now=now.total_seconds();
  if (seconds_now<BACKLIGHT_DARK_UNTIL_SECONDS)
  {
    return BACKLIGHT_MINIMUM_BRIGHTNESS;
  }
  if (seconds_now<BACKLIGHT_MAX_BRIGHTNESS_SECONDS)
  {
    // Initial ramp up
    uint32_t seconds_into_ramp=seconds_now-BACKLIGHT_DARK_UNTIL_SECONDS;
    uint32_t ramp_duration=BACKLIGHT_MAX_BRIGHTNESS_SECONDS-BACKLIGHT_DARK_UNTIL_SECONDS;
    float prop=seconds_into_ramp/(float)ramp_duration;
    return BACKLIGHT_MINIMUM_BRIGHTNESS+(prop*(BACKLIGHT_MAXIMUM_BRIGHTNESS-BACKLIGHT_MINIMUM_BRIGHTNESS));
  }
  if (seconds_now<BACKLIGHT_START_DIMMING_SECONDS)
  {
    return BACKLIGHT_MAXIMUM_BRIGHTNESS;
  }
  if (seconds_now<BACKLIGHT_FULLY_DIMMED_SECONDS)
  {
    // Initial ramp down
    uint32_t seconds_into_ramp=seconds_now-BACKLIGHT_START_DIMMING_SECONDS;
    uint32_t ramp_duration=BACKLIGHT_FULLY_DIMMED_SECONDS-BACKLIGHT_START_DIMMING_SECONDS;
    float prop=seconds_into_ramp/(float)ramp_duration;
    return BACKLIGHT_MAXIMUM_BRIGHTNESS-(prop*(BACKLIGHT_MAXIMUM_BRIGHTNESS-BACKLIGHT_MINIMUM_BRIGHTNESS));
  }
  return BACKLIGHT_MINIMUM_BRIGHTNESS;

  

}


void startup_countdown(uint8_t seconds)
{
  Serial.begin(115200);
  for (uint8_t i=seconds;i>0;i--)
  {
    Serial.println(i);
    delay(1000);
  }
}











Light wake_light= Light(PIN_WAKE_LIGHT);
AlarmSetter alarm_setter=AlarmSetter();


void setup(void)
{
    Serial.begin(115200);



    startup_countdown(4);
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    // ledcWrite(0, 255); /* Screen brightness can be modified by adjusting this parameter. (0-255) */

    Serial.begin(115200);
    Serial.println("Hello T-Display-S3");
    wake_light.begin(0.5);

    connect_wifi();
    setup_server();
    alarm_state.alarm_time=get_alarm_time();
    alarm_setter.begin(get_alarm_time, \
                       set_server_alarm_time,\
                       &disp, \
                       &alarm_state \
                       );



    disp.begin();
    delay(2000);



    
}



void loop()
{
    Serial.println("Startin loop");





    TimeSyncer syncer=TimeSyncer();
    syncer.begin(get_time_now,esp_restart);
    // char time_buffer[20];
    // Time alarm_time=get_alarm_time();
    // char time_buffer_temp[10];
    // alarm_time.c_str(time_buffer_temp);
    // disp.setAlarm(time_buffer_temp);


    while (true)
    {
      Time now=syncer.now();
      alarm_state.now=now;
      Serial.printf("\n\nLast time fetched: %d\n",now.HHMMSSacd());
      // Serial.printf("Button state: %d\n",knob_press.is_pressed());
      knob_press.is_pressed();
      Serial.printf(">>>>>>>>>>>>>>> Button toggling state: %d\n",knob_press.toggling_state);
      char time_buffer[20];
      // now.c_str(time_buffer);
      // disp.setTime(time_buffer);
      disp.update_display(&alarm_state);
      
      // Time alarm_time=alarm_setter.get_alarm_time(alarm_state); 
      alarm_setter.update();

      float light_level=calc_light_level(now,alarm_state.alarm_time);
      alarm_state.light_level=light_level;
      uint8_t backlight_level=calc_backlight_level(now);
      alarm_state.backlight_brightness=backlight_level;
      // If the main light is basically lit, then force the display level to maximum brightness regardless
      if (light_level>0.2f) light_level=BACKLIGHT_MAXIMUM_BRIGHTNESS;
      alarm_state.backlight_brightness=backlight_level;
      // disp.setBrightness(backlight_level);
      // Serial.printf("Light level: %.02f, backlight level: %d\n",light_level, backlight_level);
      wake_light.update(&alarm_state);
      for (uint8_t i=0;i<40;i++)
      {
        alarm_setter.update();
        knob_press.is_pressed();
        alarm_state.override_active=knob_press.toggling_state;
        delay(50);
      }
      
      
    }



}