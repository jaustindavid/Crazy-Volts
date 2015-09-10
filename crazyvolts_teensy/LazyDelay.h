#ifndef LAZY_DELAY_H
#define LAZY_DELAY_H


// #define DEBUG_LAZY_DELAY

#include <Arduino.h>

class LazyDelay {
  private:
    long last_millis = 0;
    long delay_interval = 0;
    
  public:

  
    LazyDelay(void) {
      this->reset();
    } // Lazydelay()
    
    
    void reset() {
      last_millis = millis();
    } // reset()
    
    
    void sleep(long requested_delay) {  
      long delay_ms = requested_delay;
      
      #ifdef DEBUG_LAZY_DELAY
      Serial.println("Lazy:");
      Serial.print("requested: ");
      Serial.println(requested_delay);
      Serial.print("last slept: ");
      Serial.println(last_millis);
      Serial.print("current: ");
      Serial.println(millis());
      #endif
      
      if (requested_delay > 0) {
        if (last_millis) {
          delay_ms = min(last_millis + requested_delay - millis(), 
                         requested_delay);
        }
        #ifdef DEBUG_LAZY_DELAY
        Serial.print("sleeping ");
        Serial.println(delay_ms);
        #endif
      }
      
      delay(delay_ms);  
      last_millis = millis();  
    } // sleep(requested_delay)
    
}; // LazyDelay

#endif
