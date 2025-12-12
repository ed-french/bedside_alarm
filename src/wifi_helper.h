

#include <WiFi.h>
#include <HTTPClient.h> // To fetch the time and alarm time as required

#include "credentials.h"

#include <AsyncTCP.h>

#include "wifi_config.h"

#include "simple_time.h"
#include "remote_light.h"

// #define ELEGANTOTA_USE_ASYNC_WEBSERVER 1

#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

#define WIFI_AT_BOOT_TIMEOUT_MS 45000

AsyncWebServer  server(80);


void connect_wifi()
{


  
  WiFi.setHostname(hostname);

  /*                    

  ################################################################                                      
  
      JUST FOR THE 1.0.0 ESP32C3 Mini Lolin boards due to RF error
  
  
  */
  WiFi.setTxPower(WIFI_POWER_8_5dBm); // May be required to connect

   /*                    

  ################################################################                                      
 
  
  */



  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  delay(1000);
   /*                    

  ################################################################                                      
  
      JUST FOR THE 1.0.0 ESP32C3 Mini Lolin boards due to RF error
  
  
  */
  // WiFi.setTxPower(WIFI_POWER_8_5dBm); // May be required to connect

   /*                    

  ################################################################                                      
 
  
  */
  WiFi.begin(ssid, password);
  delay(1000);
  Serial.printf("Connecting to: %s\n",ssid);
  Serial.println("");

  uint32_t wifi_timeout=millis()+WIFI_AT_BOOT_TIMEOUT_MS;

  // Wait for connection
  while (true)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nWifi now connected");
      break;
    }
    if (millis()>wifi_timeout)
    {
      Serial.println("Pointless message saying we are restarting to have another go at connecting");
      esp_restart();
    }
    
    delay(200);
    Serial.print(".");
  }

  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  #ifdef USE_EXTERNAL_LIGHT

      xTaskCreate(
        httpTask,     // function
        "httpTask",   // name
        4096,         // stack
        NULL,         // params
        1,            // priority
        NULL          // handle
    );

  #endif

}


bool set_server_alarm_time(char* new_alarm)
{
  
  HTTPClient http;


      
  // Your Domain name with URL path or IP address with path
  char full_url[200];
  strcpy(full_url,set_alarm_url);
  strcat(full_url,new_alarm);


  http.begin(full_url);
    
  int httpResponseCode = http.GET();
      
  if (httpResponseCode>0)
  {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        http.end();
        return true; // successful




  } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return false;
}



Time get_alarm_time()
{
    

  HTTPClient http;


      
  // Your Domain name with URL path or IP address with path
  http.begin(get_alarm_time_url);
    
  int httpResponseCode = http.GET();
      
  if (httpResponseCode>0)
  {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        // strncpy(g_alarm_time_s,payload.c_str(),5);
        // disp.setAlarm(g_alarm_time_s);
        if (httpResponseCode!=200 || strlen(payload.c_str())!=5)
        {
          Time new_time=Time();
          new_time.set_bad();
          return new_time;
        }
        // We have a good time
        Time new_time=Time();
        new_time.set_from_HHMM_str((char *)payload.c_str());
        return new_time;






  } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        Time new_time=Time();
        new_time.set_bad();
        return new_time;
  }
  // Free resources
  http.end();
}

void setup_server(void)
{
  

  server.on("/",HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "This is the dawn light controller.\n"
                        "\t* Server uses /refetch_alarm to flag the device should refresh the alarm time\n"
                        "\tUse /update to install new firmware remotely (creds wificlassic)\n");
  });

  server.on("/refetch_alarm", HTTP_GET, [] (AsyncWebServerRequest * request)
  {
      get_alarm_time();
      request->send(200,"text/plain","OK");
  }
  );

  







  ElegantOTA.begin(&server);    // Start AsyncElegantOTA
  ElegantOTA.setAuth(ota_username,ota_password);
  server.begin();
  Serial.println("HTTP server started");

  

}

bool is_connected()
{
  return WiFi.status() == WL_CONNECTED;
}

bool get_time_now(char * time_buffer)
{
  

  HTTPClient http;


      
  // Your Domain name with URL path or IP address with path
  http.begin(time_server_url);
    
  int httpResponseCode = http.GET();
      
  if (httpResponseCode>0)
  {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        // g_millis_at_sync=millis(); // Note the time of the last sync

        // // Convert the string to hours, mins and seconds:
        // //2023-12-07T11:13:20
        // //0123456789012345678
        uint8_t hours_at_sync=payload.substring(11,13).toInt();
        uint8_t mins_at_sync=payload.substring(14,16).toInt();
        uint8_t seconds_at_sync=payload.substring(17,19).toInt();
        char time_buf[20];
        sprintf(time_buf,"%02d:%02d:%02d",hours_at_sync,mins_at_sync,seconds_at_sync);
        strcpy(time_buffer,time_buf);
        return true;
        




  } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return false;

}