/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using SPI to communicate
4 or 5 pins are required to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <Time.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


#define MAX_VOLTAGE (50.4)
#define MIN_VOLTAGE (47.2)

#include "LazyDelay.h"

LazyDelay loop_sleep;
LazyDelay seconds_sleep;

#include <CallbackTimer.h>

CallbackTimer cbt;

void setup()   {                
  Serial.begin(115200);
  setup_time();
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  // display.begin(SSD1306_SWITCHCAPVCC);
  display.begin();
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  
  cbt.every(1000, showClock);
  cbt.every(500, showVoltage);
  cbt.every(200, showFuelGauge);
  cbt.every(100, display.display);
}


void loop() {
  Serial.print("boop ");
  Serial.println(millis() / 1000);
  // undrift();
  
  float voltage;
  
  for (voltage = MAX_VOLTAGE; voltage >= MIN_VOLTAGE-1; voltage -= 0.1) {
    display.clearDisplay();

/*
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,16);
    display.print("Crazy");
    display.setTextSize(1);
    display.print(" ");
    display.setTextSize(2);
    display.println("Volts");
*/

    showVoltage(voltage);
    showFuelGauge(voltage);
    showClock();
    display.display();
    loop_sleep.sleep(100);
  }

  for (int i = 0; i < 5; i++) {
    showClock();
    display.display();
    seconds_sleep.sleep(1000);
  }
}


void showVoltage(float voltage) {
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(34,16);
    justify(4, voltage, 1);
    display.print("v");
} // showVoltage(float)


void showClock() {
    display.fillRect(109,24,128,64, BLACK);
    display.fillRect(7,34,128,64, BLACK);
    display.setTextColor(WHITE);
    display.setTextSize(4);
    display.setCursor(7,34);
    display.print(hour());
    display.print(millis()/1000 % 2 ? ":" : " ");
    if (minute() < 10) {
      display.print("0");
    }
    display.print(minute());

    display.setTextSize(1);
    display.setCursor(109, 24);
    display.print(":");
    if (second() < 10) {
      display.print("0");
    }    
    display.print(second());
} // showVoltage(float)


// E [###########______] F 
void showFuelGauge(float voltage) {
  
  float fuelLevel = (voltage - MIN_VOLTAGE)/(MAX_VOLTAGE - MIN_VOLTAGE);
  fuelLevel = constrain(fuelLevel, 0.0, 1.0);

  #define SFG_LETTER_H   14   // E / F height, pixels
  #define SFG_LETTER_W   5    // E / F width, pixels
  #define SFG_LETTER_SPC 2    // space between letters, pixels
  #define SFG_LEFT       0
  #define SFG_RIGHT      127
  #define SFG_TOP        0
  #define SFG_BOTTOM     SFG_TOP + SFG_LETTER_H

  // fullness
  #define SFG_LEFT_BORDER (SFG_LEFT + SFG_LETTER_W + SFG_LETTER_SPC)
  #define SFG_RIGHT_BORDER (SFG_RIGHT - (SFG_LETTER_W + SFG_LETTER_SPC))
  byte rightEdge = (byte) (1.0 * fuelLevel * (SFG_RIGHT - SFG_LEFT) 
                             + SFG_LEFT);
  display.fillRect(SFG_LEFT, SFG_TOP, 
                   rightEdge, SFG_BOTTOM, 
                   WHITE);
  // ticks @ 25, 50, 75%
  for (byte i = 1; i < 8; i++) {
    byte x = (byte)(1.0 * i/8 * (SFG_RIGHT - SFG_LEFT) 
                               + SFG_LEFT);
    byte top = SFG_TOP;
    if (i % 2 != 0) {
      top = SFG_TOP + (SFG_BOTTOM - SFG_TOP)/2;
    }
    display.drawLine( x, top, x, SFG_BOTTOM - 1, 
                     (x > rightEdge ? WHITE : BLACK));
  }
  
  // frame          
  display.drawRect(SFG_LEFT, SFG_TOP, 
                   SFG_RIGHT, SFG_BOTTOM, 
                   WHITE);
} // showFueldGauge()




void justify(byte width, float value, byte decimals) {
  byte i;
  for (i = 0; i < decimals; i++) {
    value *= 10;
  }
  value = (int)value;
  for (i = 0; i < decimals; i++) {
    value /= 10;
  }

  // total = log10(value) + 1 + decimals
  byte spaces = 1;
  float working = value;
  if (working < 0) {
    working = working * -1;
    spaces += 1;
  }
  while (working >= 10) {
    ++spaces;
    working = working / 10;
  }

  if (decimals) {
    spaces += 1 + decimals;
  }

  
  if (spaces > width) {
    decimals = max(0, decimals - (spaces - width));

    if (decimals == 0) {
      spaces -= 3;
    }
  }
  
  for (; spaces < width; spaces ++) {
    display.print(F(" "));        
  }
  display.print(value, decimals);
} // justify(width, value, decimals)


/*
 * https://www.pjrc.com/teensy/td_libs_Time.html
 */

time_t getTeensy3Time() {
  return Teensy3Clock.get();
} // time_t getTeensy3Time()


/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}


void setup_time()  {
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);
  return;
  while (!Serial);  // Wait for Arduino Serial Monitor to open
  delay(100);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  
  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      Teensy3Clock.set(t); // set the RTC
      setTime(t);
    }
  }

} // setup_time()


// "delay" until the next requeted_delay since the last
// lazy delay.  
//
// Usage:
//   for (;;) {
//      math();
//      lazy_delay(100);
//   }
//  ... to run the loop exactly every 100 ms.
//
//  lazy_delay(0) to "reset" the clock to "now".
void lazy_delay(long requested_delay) {
  static long last_millis = 0;
  static long delay_ms = requested_delay;
  
  if (requested_delay > 0) {
    if (last_millis) {
      delay_ms = min(last_millis + requested_delay - millis(), 
                      requested_delay);
    }
    delay(delay_ms);  
  }
  last_millis = millis();
}


// DRIFT_RATE = 10800 / #s drifted per day
#define DRIFT_RATE 1000 // seconds between drift events
#define DRIFT_ADDY 64   // bytes 64-67 are occupado
void undrift(void) {
  time_t last_drift, nowish;
  long distance;
  byte first_byte;
 
  nowish = now();
  first_byte = EEPROM.read(DRIFT_ADDY);
  if (first_byte == 255) {
    Serial.println("initializing drift record");
    EEPROM.put(DRIFT_ADDY, nowish);
  } else {
    EEPROM.get(DRIFT_ADDY, last_drift);
    Serial.print("last drift: ");
    Serial.print(last_drift);
    Serial.print(" = ");
    Serial.print(nowish - last_drift);
    Serial.println("s ago");
    if (nowish - last_drift > DRIFT_RATE) {
      distance = (nowish - last_drift)/DRIFT_RATE;
      adjustTime(distance);
      Serial.print("drifting ");
      Serial.println(distance);
      EEPROM.put(DRIFT_ADDY, nowish);
    }
  }
}
