#include <Arduino.h>



#include "remote_light.h"
#include <HTTPClient.h> // To fetch the time and alarm time as required
#include "alarm_state.h"

const char URL_SLAVE_LIGHT[] = "http://192.168.1.183/set_level?level=";

Light::Light(int pin)
{
    if (pin != -1)
    {
        Serial.println("Warning: Remote light does not use a pin, ignoring pin argument");
    }
}

void Light::begin(float starting_level)
{
    Serial.printf("Remote light begin called with starting level %.02f\n",starting_level);
    // We can just send that level using set
    // set(starting_level);
}

void Light::set(float prop)
{
    Serial.printf("Remote light set called with prop %.02f\n",prop);
    String level_string = String(prop);
    String url = URL_SLAVE_LIGHT + level_string;
    Serial.printf("Sending url %s\n",url.c_str());
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    Serial.printf("Got http code %d\n",httpCode);
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("Error setting remote light level, HTTP code %d\n",httpCode);
    }
    
    http.end();
}

void Light::update(AlarmState *alarm_state)
{
    float probable_light_level=alarm_state->light_level;

    // Test if we should be overriding the light level
    if (alarm_state->override_active)
    { 

        // Figure out the over-ridden light level (the opporsite of the expected level)
        // i.e. if the alarm light is off, then it should be overridden to nearly on

        float actual_light_level=probable_light_level>0.01f ? 0.01f : 0.7f;
        alarm_state->light_level=actual_light_level;
        Serial.printf("Override active, setting light level to %.02f\n",actual_light_level);
        set(actual_light_level);
        return;
    }
    set(alarm_state->light_level);
}