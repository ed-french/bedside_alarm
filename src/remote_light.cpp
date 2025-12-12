#include <Arduino.h>



#include "remote_light.h"
#include "alarm_state.h"
#include <AsyncTCP.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char SLAVE_LIGHT_HOST[] = "192.168.1.183";
const uint16_t SLAVE_LIGHT_PORT = 80;
const char SLAVE_LIGHT_PATH[] = "/set_level";
// const char URL_SLAVE_LIGHT[] = "http://192.168.1.183/set_level?level=";

struct HttpRequest {
    String host;
    String path;
    uint16_t port;
    bool newRequest;
};

static HttpRequest pendingRequest = {"", "", 80, false};

// This function just updates the pending request
void sendHttp(const char* host, uint16_t port, const char* path) {
    pendingRequest.host = host;
    pendingRequest.port = port;
    pendingRequest.path = path;
    pendingRequest.newRequest = true;
}

// HTTP task function
void httpTask(void* pvParameters) {
    while (true) {
        if (pendingRequest.newRequest) {
            HTTPClient http;
            String url = String("http://") + pendingRequest.host + ":" + pendingRequest.port + pendingRequest.path;

            Serial.printf("[httpTask] Sending GET: %s\n", url.c_str());

            http.begin(url);
            int code = http.GET();
            if (code > 0) {
                Serial.printf("[httpTask] Response code: %d\n", code);
                String payload = http.getString();
                Serial.println("[httpTask] Payload: " + payload);
            } else {
                Serial.printf("[httpTask] GET failed, error: %s\n", http.errorToString(code).c_str());
            }
            http.end();

            pendingRequest.newRequest = false;
        }

        vTaskDelay(50 / portTICK_PERIOD_MS); // small delay
    }
}




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
    // Make the full path from the constant and the level
    String full_path = String(SLAVE_LIGHT_PATH) + "?level=" + String(prop, 6);
    sendHttp(SLAVE_LIGHT_HOST, SLAVE_LIGHT_PORT, full_path.c_str());
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