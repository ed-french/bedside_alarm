#include <Arduino.h>

#define TIME_BETWEEN_SYNCS_MS 60*60*1000

#include "simple_time.h"

class TimeSyncer
{
    private: 
    
        uint32_t last_sync_millis;
        bool (*time_fetcher_fn)(char *);
        void (*reset_fn)(void);
        Time last_time_fetched;

    public:
        TimeSyncer(void)
        {

        }

        Time fetch_time(void)
        {
            // Always fetches a fresh time and returns it
            // (Compare with now that only fetches if the last reading is stale)
            Serial.println("Fetching a fresh time");
            char time_buffer[10];
            if (time_fetcher_fn(time_buffer))
            {
                this->last_time_fetched.set_from_hms_str(time_buffer);
                this->last_sync_millis=millis();
                return this->last_time_fetched;
            }
            // We failed to fetch a new time right now so we'll reset instead
            Time bad_time=Time();
            bad_time.set_bad();
            return bad_time;
        }


        Time now(void)
        {
            // Returns a Time object with the current time
            // or with Time.good_time set false if it failed
            if (millis()<TIME_BETWEEN_SYNCS_MS+this->last_sync_millis)
            { // We can just estimate the new time based on millis
                uint32_t seconds_since_last_sync=(millis()-this->last_sync_millis)/1000;
                Time new_time=this->last_time_fetched.add_seconds(seconds_since_last_sync);
                return new_time;
            }
            // We have now to fetch the fresh time and use that instead
            return this->fetch_time();
        }

        void begin(bool (*time_fetcher)(char *), void (*reset)(void))
        // time_fetecher_fn takes a buffer and responds with HH:MM:SS from server
        {
            this->time_fetcher_fn=time_fetcher;   
            this->reset_fn=reset; 
            this->fetch_time();
        }
        void update(void)
        // Called periodically to update the time ONLY if that's required
        {
            this->now();
        }
};