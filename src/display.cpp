#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "FreeSans24pt7b.h"
#include "pin_config.h"
#include "display.h"

#define PWM_CHANNEL_LCD_BL 0

Arduino_DataBus *bus = new Arduino_ESP32LCD8(PIN_LCD_DC /* DC */, \
                                            PIN_LCD_CS /* CS */, \
                                            PIN_LCD_WR /* WR */, \
                                            PIN_LCD_RD /* RD */, \
                                            PIN_LCD_D0 /* D0 */,\
                                            PIN_LCD_D1 /* D1 */, \
                                            PIN_LCD_D2 /* D2 */, \
                                            PIN_LCD_D3 /* D3 */, \
                                            PIN_LCD_D4 /* D4 */, \
                                            PIN_LCD_D5 /* D5 */, \
                                            PIN_LCD_D6 /* D6 */, \
                                            PIN_LCD_D7 /* D7 */);







Display::Display()
        {
            // this->refresh_required=true;
            // strcpy(this->time_str,"--:--");
            // strcpy(this->alarm_str,"??:??");
            // this->brightness=50;
        }
void Display::begin()
        {
           this->gfx = new Arduino_ST7789(bus, PIN_LCD_RES, /* RST */ \
                                        2 /* rotation */, \
                                        true /* IPS */, \
                                        170 /* width */, \
                                        320 /* height */, \
                                        35 /* col offset 1 */, \
                                        0 /* row offset 1 */, \
                                        35 /* col offset 2 */, \
                                        0 /* row offset 2 */);
            this->gfx->begin();
            this->gfx->setRotation(3); 
            
            ledcSetup(PWM_CHANNEL_LCD_BL, 2000, 8);
            ledcAttachPin(PIN_LCD_BL, PWM_CHANNEL_LCD_BL);
            // this->setBrightness(100);


        }


// void Display::_setBrightness(uint8_t bright)
//         {
//           //Serial.printf("About to compare brightnesses of : %d and %d\n",bright,this->brightness);
//             if(this->brightness!=bright)
//             {
//                 //Serial.printf("Brightness updated to : %d\n",bright);
//                 this->refresh_required=true;
//                 this->brightness=bright;
//             } 

            
//         }

// void Display::setTime(char * time_str)
//         {
//             //Serial.printf("About to compare times as strings: %s to %s\n",time_str,this->time_str);
//             if (strncmp(time_str,this->time_str,5))
//             {
//                 //Serial.printf("Time has changed to : %s !\n",time_str);
//                 strncpy(this->time_str,time_str,5);
//                 this->refresh_required=true;
//             }
            

//         }

// void Display::_getAlarm(char * alarm_str)
//         {
//           strncpy(alarm_str,this->alarm_str,5);

//         }

// void Display::_setAlarm(char * alarm_str)
//         {
//             if (strncmp(alarm_str,this->alarm_str,5))
//             {
//                 Serial.printf("Alarm has changed to : %s!\n", alarm_str);
//                 strncpy(this->alarm_str,alarm_str,5);
//                 this->refresh_required=true;
//             }


            
//         }

void Display::update_display(AlarmState *alarm_state = nullptr)
        {
            // Do nothing if a refresh isn't needed
            if (!alarm_state->display_needs_update)
            {
                Serial.println("No need to update display");
                return;
            } 
            ledcWrite(0, alarm_state->backlight_brightness);

            this->gfx->fillScreen(BLACK);


            // Show the time...
            this->gfx->setTextColor(WHITE);
            this->gfx->setCursor (48, 100);
            this->gfx->setFont(&FreeSans24pt7b);
            this->gfx->setTextSize(2); 
            char now_str[20];
            alarm_state->now.HH_MM_str(now_str);
            this->gfx->print(now_str);


            // Show the alarm time
            this->gfx->setTextColor(CYAN);
            this->gfx->setCursor (160, 144);
            this->gfx->setFont(&FreeSans24pt7b);
            this->gfx->setTextSize(0);

            char alarm_str[20];
            alarm_state->alarm_time.HH_MM_str(alarm_str);
            this->gfx->print(alarm_str);

            uint8_t height_of_brightness_bar= (uint8_t)(alarm_state->light_level*170.0f);
            // Serial.printf("Height of brightness bar is : %d\n",height_of_brightness_bar);
            this->gfx->fillRect(0, 170-height_of_brightness_bar , 20, height_of_brightness_bar, YELLOW);


            // Serial.println("Display updated");

            alarm_state->display_needs_update = false;
        }

        
