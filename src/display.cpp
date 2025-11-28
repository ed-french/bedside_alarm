#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "FreeSans24pt7b.h"
#include "pin_config.h"
#include "display.h"

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
            this->refresh_required=true;
            strcpy(this->time_str,"--:--");
            strcpy(this->alarm_str,"??:??");
            this->brightness=50;
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
        }

void Display::setBrightness(uint8_t bright)
        {
          //Serial.printf("About to compare brightnesses of : %d and %d\n",bright,this->brightness);
            if(this->brightness!=bright)
            {
                //Serial.printf("Brightness updated to : %d\n",bright);
                this->refresh_required=true;
                this->brightness=bright;
            } 

            
        }

void Display::setTime(char * time_str)
        {
            //Serial.printf("About to compare times as strings: %s to %s\n",time_str,this->time_str);
            if (strncmp(time_str,this->time_str,5))
            {
                //Serial.printf("Time has changed to : %s !\n",time_str);
                strncpy(this->time_str,time_str,5);
                this->refresh_required=true;
            }
            

        }

void Display::getAlarm(char * alarm_str)
        {
          strncpy(alarm_str,this->alarm_str,5);

        }

void Display::setAlarm(char * alarm_str)
        {
            if (strncmp(alarm_str,this->alarm_str,5))
            {
                Serial.printf("Alarm has changed to : %s!\n", alarm_str);
                strncpy(this->alarm_str,alarm_str,5);
                this->refresh_required=true;
            }


            
        }

void Display::update_display()
        {
            // Do nothing if a refresh isn't needed
            if (!this->refresh_required)
            {
                Serial.println("No need to update display");
                return;
            } 
            ledcWrite(0, this->brightness);

            this->gfx->fillScreen(BLACK);


            // Show the time...
            this->gfx->setTextColor(WHITE);
            this->gfx->setCursor (48, 100);
            this->gfx->setFont(&FreeSans24pt7b);
            this->gfx->setTextSize(2); 

            this->gfx->print(this->time_str);


            // Show the alarm time
            this->gfx->setTextColor(RED);
            this->gfx->setCursor (160, 144);
            this->gfx->setFont(&FreeSans24pt7b);
            this->gfx->setTextSize(0);

            this->gfx->print(this->alarm_str);
            Serial.println("Display updated");

            this->refresh_required=false;
        }

        
