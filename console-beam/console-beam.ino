/***********************************

  Weather and Information Console

  Verify   WiFi.setPins(8,7,4,2) for Adafruit Feather M0 WiFi
           api.weather.gov:443     ssl certificate added via WiFi101 updater
           UTCOFFSET = -4          set (EDT 2nd sun mar | EST 1st sun NOV)
           TIMEZONE  = "EDT"       timezone lables
           FCSTLAT = "33.8774" FCSTLON = "-84.3046" KPDK Peachtree Decalb, Atlanta
           FCSTLAT = "39.73" FCSTLON = "-104.99" Denver CO 80204
           WiFi credentials updated in file /lib/WiFiCreds.h
           
  For avr-libc ATMEL series 32bit SAMD21 CPUs e.g. Arduino MKR1000
  © 2018 Bob Smith https://github.com/bethanysciences/console
  MIT license

  MKR1000 https://store.arduino.cc/usa/arduino-mkr1000
  - aref       5v -
  - dac0/a0   vin -
  - a1        vcc - 
  - a2        gnd -
  - a3      reset - 
  - a4      14 tx -
  - a5      13 rx - 
  - a6     12 scl - 
  - 0      11 sda - 
  - 1     10 miso - 
  - ~2      9 sck -
  - ~3     8 mosi -
  - ~4          7 - 
  - ~5          6 - 

  Hoover Beam 5x24 cool-white led grid http://www.hoverlabs.co/beam-basics
  -   3.3v -
  -  gnd  -
  - sync  -
  - reset -
  -  irq  -
  -  scl  -
  -  sda  -
  b.setSpeed(3);
  b.setLoops(7);
  b.play();

***********************************/ 
#include "Arduino.h"
#include "Wire.h"
#include "stdint.h"
#include "RTCZero.h"              // arduino.cc/en/Reference/RTC
#include "SimpleTimer.h"          // playground.arduino.cc/Code/SimpleTimer
#include "WiFi101.h"              // www.arduino.cc/en/Reference/WiFi101
#include "Adafruit_HX8357.h"      // github.com/adafruit/Adafruit_HX8357_Library
#include "Adafruit_LEDBackpack.h" // github.com/adafruit/Adafruit-LED-Backpack-Library
#include "Adafruit_GFX.h"         // github.com/adafruit/Adafruit-GFX-Library
#include "Fonts/FreeSans9pt7b.h"  // font included with Adafruit-GFX-Library
#include "Fonts/FreeSans12pt7b.h" // font included with Adafruit-GFX-Library
#include "WiFiCreds.h"            // WiFi credentials
#include "convertTime.h"          // timezone and 12/24 hr conversion
#include "dtostrf.h"              // Convert float to string
              // avr-libc sprintf does not format floats
              // char *dtostrf(val, width, prec, char *sout)
              //   val  double / float variable
              // width  string length returned INCLUDING decimal point + sign
              //  prec  number of digits after the deimal point to print
              //  sout  destination of output buffer (must be large enough)
#include "ArduinoHttpClient.h"
#include "wxConversions.h"
          // Celc > Fahr double c2f(double [temp °celcius])
          //              returns (double [temp °fahrenheit])
          // Fahr > Celc double f2c(double [temp °fahrenheit])
          //              retuns (double [temp °celcius])
          // Humidity double rh(double [dew point °celc], double [temp °celc])
          //              returns (double [% rel humidity])
          // Windchill  float wc(double [temp °celcius], int [MPH windspeed]}
          //              returns (float [windchill °celcius])
          // PA -> "HG  double p2h(double pascals)
          //              returns double [presure in inches mercury])
          // Dew Point  double dp(double [temp °celc or °fahr], int [% rel humd])
          //              returns double [dew point °celcius or °fahrenheit]
          // Heat Index double hi(double [temp °celcius or °fahrenheit],
          //                       int [% rel humidity], bool [°celcius input ?])
          //              returns heat index double [temp °celcius or °fahrenheit]
          // MED  double med(int [uvindex], int [altitude meters], bool [on water ?],
          //               bool [on snow ?], int [fitz skin type], int [spf applied])
          //              returns int [mins] to Min Erythemal Dose (MED) - sunburn
#include "xmlTakeParam.h"   // parse XML elements
                            // xmlTakeParam(String inStr, String needParam)
                            // input string       e.g. <temp_c>30.6</temp_c>
                            // input needParam    parameter e.g. temp_c
                            // returns value in string
#include "ArduinoJson.h"    // https://bblanchon.github.io/ArduinoJson/
#include "beam.h"


#define SERIAL_DEBUG     // used for serial debugging
#define RSTPIN 6                            // any digital pin
#define IRQPIN 9                            // not used
#define BEAMCOUNT 2                         // number of beams
Beam b = Beam(RSTPIN, IRQPIN, BEAMCOUNT);   // initialize

void setup() {
    Wire.begin();
    b.begin();
    b.print("I Love Rachel FEAL");
    b.play();
}

void loop() {

}
