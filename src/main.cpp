#include <Arduino.h>


#include "live_time.h"
#include "wifi_helper.h"
#include "PWM_light.h"

#include "alarm_setter.h"




#include "display.h"

#include "pin_config.h"



#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#error  "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif



#define START_LIGHT_WINDOW_SECONDS (40*60)
#define END_LIGHT_WINDOW_SECONDS (10*60)
















Display disp=Display();



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
    Serial.println("Too late to alarm");
    return 0.0;
  } 
  if (seconds_to_max>0)
  {
    return (1-seconds_to_max/(float)START_LIGHT_WINDOW_SECONDS);
  }
  return 1-seconds_to_max/(float)END_LIGHT_WINDOW_SECONDS;

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











PWM_Light wake_light= PWM_Light(PIN_WAKE_LIGHT);
AlarmSetter alarm_setter=AlarmSetter();


void setup(void)
{
    Serial.begin(115200);



    startup_countdown(4);
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    ledcSetup(0, 2000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    // ledcWrite(0, 255); /* Screen brightness can be modified by adjusting this parameter. (0-255) */

    Serial.begin(115200);
    Serial.println("Hello T-Display-S3");
    wake_light.begin(0.5);

    connect_wifi();
    setup_server();
    get_alarm_time();
    alarm_setter.begin(get_alarm_time, \
                       set_server_alarm_time,\
                       &disp);



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
      Serial.printf("Last time fetched: %d\n",now.HHMMSSacd());
      char time_buffer[20];
      now.c_str(time_buffer);
      disp.setTime(time_buffer);
      disp.update_display();
      
      Time alarm_time=alarm_setter.get_alarm_time();  

      float light_level=calc_light_level(now,alarm_time);
      Serial.printf("Light level: %.02f\n",light_level);
      wake_light.set(light_level);
      for (uint8_t i=0;i<5;i++)
      {
        alarm_setter.update();
        delay(200);
      }
      
      
    }



    // int32_t newPosition = encoder.getCount();
    
    // if (newPosition!=0)
    // {
      // We have a change in the encoder
      // Serial.println(newPosition);
      // encoder.setCount(0);// reset the count, we'll take this one in..
      // g_server_alarm_outofdate=true; // Mark that we need to update the server
      // g_update_server_alarm_ms=millis()+2000; // We can wait a couple of seconds of quiet before sending to the server

      // char old_alarm[10];
      // disp.getAlarm(old_alarm);
      // uint32_t alarm_mins=time_str_to_mins(old_alarm);

      // Calculate the new alarm time based on the encoder move noted
      // int16_t increment=minutes_per_encoder_step*newPosition;
      // Serial.printf("\t\tIncrement: %d\n",increment);
      // alarm_mins=alarm_mins+increment;


      // char new_alarm[20];
      // mins_to_time_str(new_alarm,alarm_mins); // convert the new time in mins to a char array
      // disp.setAlarm(new_alarm);// Store the new time

    // }
    

    // if (millis() > running) {
    //     // Serial.print("Current running ");
    //     // Serial.print(millis());
    //     // Serial.println(" millis");
    //     char time_buf[20];

    //     disp.setTime(time_buf);
    //     uint8_t brightness=16;
    //     if (g_hours_at_sync>8 || g_hours_at_sync<21) brightness=255;
        
    //     disp.setBrightness(brightness);


    //     disp.update_display();

    //     running = millis() + 200;

    //     if (g_server_alarm_outofdate && millis()>g_update_server_alarm_ms)
    //     {
    //       Serial.println("\nATTEMPING UPDATE THE SERVER RECORD OF THE ALARM TIME NOW....!");
    //       char new_alarm[20];
    //       disp.getAlarm(new_alarm);
    //       bool success=set_server_alarm_time(new_alarm);
    //       if (success)
    //       {
    //         g_server_alarm_outofdate=false; // Just send once
    //         Serial.println("\t\tSUCCESSFUL!!!!!!!!!!!!");
    //       } else {
    //         Serial.println("Failed to update the server, will repeat");
    //         g_update_server_alarm_ms=millis()+5000;// Try again in a bit
    //       }



          

    //     }
    // }

}