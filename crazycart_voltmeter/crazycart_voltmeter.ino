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

#include <SPI.h>
#include <Wire.h>
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

void setup()   {                
  Serial.begin(115200);
  
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
}


void loop() {
  float voltage;
  
  for (voltage = 26.4; voltage >= 20.0; voltage -= 0.1) {
    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,16);
    display.print("Crazy");
    display.setTextSize(1);
    display.print(" ");
    display.setTextSize(2);
    display.println("Volts");


    display.setTextSize(4);
    display.setCursor(0,32);
//    display.print(voltage);
    justify(4, voltage, 1);
    display.println("v");

    showFuelGauge(voltage);
    display.display();
    delay(100);
  }

  delay(5000);
}



// E [###########______] F 
void showFuelGauge(float voltage) {
  #define MAX_VOLTAGE (13.2*2)
  #define MIN_VOLTAGE (10.8*2)
  
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

