/*----------------------------------------------------------------------------------*
  Weather and Information Console
  for ATMEL(Microship) SAMD21 Cortex-M0+ 32bit low power ARM MCU's 
                        and integrated ATWINC1500 Wi-Fi Network Controller 
  e.g. Arduino MKR1000, Adafruit Adafruit Feather M0 WiFi w/ ATWINC1500
  
  NOTE: WiFi.setPins(8,7,4,2) for Adafruit Feather M0 WiFi
  
  Current time through rtc library and wifi101 libraries
   UTCOFFSET=-6 TIMEZONE="MDT" DST on 2nd sun mar - off 1st sun NOV

  Forecasts
   from https://www.weather.gov/documentation/services-web-api
   api.weather.gov:443         add ssl certificate via Arduino WiFi101 updater
   STATION in code Buckley Air Force Base Airport KBKF BOU/70,59
   Lat: 39.71°N    Lon: 104.76°W   Elev: 5577ft

  Current
   from https://www.aviationweather.gov
   aviationweather.gov:443    add ssl certificate via Arduino WiFi101 updater
   STATION in code Denver, Denver International Airport (KDEN)
   Lat: 39.85°N     Lon: 104.66°W   Elev: 5404ft
   
  Displays using Hoover Beam 5x24 cool-white led grid
   from http://www.hoverlabs.co/beam-basics
   connections     -  3.3v -
                   -  gnd  -
                   - sync  -
                   - reset -
                   -  irq  -
                   -  scl  -
                   -  sda  -
   b.print("text");    load beam memory
   b.setSpeed(3);      set scroll speed fastst 1 - 15 slowest
   b.setLoops(7);      set scroll speed fastst 1 - 15 slowest
   b.play();           scroll text
   b.display();        print text
   NOTE:for static display function use updated library beam.cpp included in package

  © 2018 Bob Smith https://github.com/bethanysciences/getwxbeam
  MIT license
*----------------------------------------------------------------------------------*/
#include "Wire.h"
#include "RTCZero.h"                        // arduino.cc/en/Reference/RTC
#include "beam.h"                           // http://www.hoverlabs.co/beam-basics
#include "convertTime.h"                    // timezone and 12/24 hr conversion
#include "SimpleTimer.h"                    // playground.arduino.cc/Code/SimpleTimer
#include "WiFi101.h"                        // arduino.cc/en/Reference/WiFi101
#include "WiFiCreds.h"                      // WiFi credentials
#include "NOAAmetar.h"
#include "NOAAforecast.h"
#include "dtostrf.h"                        // Convert float to string
#include "wxConversions.h"

// #define DEBUG_METAR                          // used for serial debugging
// #define DEBUG_FCAST                      
// #define DEBUG 

#define RSTPIN 5                            // any digital pin
#define BEAMCOUNT 2                         // number of beams
Beam b = Beam(RSTPIN, BEAMCOUNT);           // initialize
int status  = WL_IDLE_STATUS;
RTCZero rtc;
 #define UTCOFFSET     -6                   // EDT 2nd sun mar | EST 1st sun NOV
 int UTCEPOCH         = UTCOFFSET * 3600;   // epoch timezone offset
 const char* TIMEZONE = "MST";              // timezone lables
 const char* AMPM[2]  = {"am", "pm"};       // AM/PM preferred lables
 bool TIME24          = false;              // false = use 12 hour time
 int MIN              = 60000;              // 1 minute of microseconds
SimpleTimer timer;
int GetNOAAInt        = 45;                  // update interval (min)

const char* METARstation = "KDEN";
const char* FCASTstation = "BOU/62,61";

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    Wire.begin();
    b.begin();
    #ifdef DEBUG
        Serial.begin(115200);  
        while (!Serial) {
            Serial.println("start");
        }
    #endif
    b.print("START"); b.display();

    WiFi.setPins(8,7,4,2);                  // pins for Adafruit ATWINC1500 Feather
    while ( status != WL_CONNECTED) {
        status = WiFi.begin(ssid, pass);
        b.print("CONNECT"); b.display();
    }

    rtc.begin();
    unsigned long epoch = 0;
    while (epoch == 0) epoch = WiFi.getTime();
    epoch = epoch + UTCEPOCH;
    rtc.setEpoch(epoch);
    timer.setInterval(MIN * GetNOAAInt, GetNOAA);
    char stat[100];
    sprintf(stat, "connect %02d-%02d-20%02d %02d%02d %s to %s %ddBm",
                   rtc.getMonth(), rtc.getDay(), rtc.getYear(),rtc.getHours(), 
                   rtc.getMinutes(), TIMEZONE, WiFi.SSID(), WiFi.RSSI()); 
    b.print(stat); b.play();
    digitalWrite(LED_BUILTIN, HIGH);                // setup complete
    GetNOAA(); 
}

void loop() { 
    timer.run();
}

void GetNOAA() {
    METAR.station         = METARstation;
    GetMETAR(METAR);
    FCAST.station         = FCASTstation;
    NOAAForecast(FCAST);
    int CurrTempF = c2f(METAR.tempC.toInt());
    int TempF0    = FCAST.p0temp;
    int TempF1    = FCAST.p1temp;
    int TempF2    = FCAST.p2temp;
    
    char stat[100];
    sprintf(stat, "at %02d/%02d %02d%02d%s %ddBm",
                   rtc.getMonth(), rtc.getDay(), rtc.getHours(), 
                   rtc.getMinutes(), TIMEZONE, WiFi.RSSI()); 
    char temps[20];
    sprintf(temps, "%dF %dF %dF",CurrTempF, TempF0, TempF1);            // "°F"
    b.print(temps); b.display();
    
    #ifdef DEBUG
        Serial.println(stat);
        Serial.println(temps);
        Serial.println("CUR NXT NXT");
        Serial.println("--------------------------");
    #endif
}
