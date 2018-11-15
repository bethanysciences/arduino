/*----------------------------------------------------------------------------------*
  Weather and Information Console

    WiFi.setPins(8,7,4,2)       for Adafruit Feather M0 WiFi
    WiFi credentials updated in file /library/WiFiCreds.h

    UTCOFFSET=-6 TIMEZONE="MDT" DST on 2nd sun mar off 1st sun NOV

    https://www.weather.gov/documentation/services-web-api
    api.weather.gov:443         add ssl certificate added via WiFi101 updater
    STATION Buckley Air Force Base Airport KBKF BOU/70,59
    Lat: 39.71°N    Lon: 104.76°W   Elev: 5577ft

  For avr-libc ATMEL series 32bit SAMD21 CPUs e.g. Arduino MKR1000
  © 2018 Bob Smith https://github.com/bethanysciences/console
  MIT license
 *---------------------------------------------------------------------------------*/
#include "RTCZero.h"            // arduino.cc/en/Reference/RTC
#include "SimpleTimer.h"        // playground.arduino.cc/Code/SimpleTimer
#include "WiFi101.h"            // arduino.cc/en/Reference/WiFi101
#include "WiFiCreds.h"          // WiFi credentials
#include "NOAAmetar.h"
#include "NOAAforecast.h"
#include "convertTime.h"        // timezone and 12/24 hr conversion
#include "dtostrf.h"            // Convert float to string
          // avr-libc sprintf does not format floats
          //  char  *dtostrf(val, width, prec, char *sout)
          //   val  double / float variable
          // width  string length returned INCLUDING decimal point + sign
          //  prec  number of digits after the deimal point to print
          //  sout  destination of output buffer (must be large enough)
#include "wxConversions.h"
          // Celc > Fahr double c2f(double [temp °celcius])
          //             returns (double [temp °fahrenheit])
          // Fahr > Celc double f2c(double [temp °fahrenheit])
          //             returns (double [temp °celcius])
          // Humidity double rh(double [dew point °celc], double [temp °celc])
          //             returns (double [% rel humidity])
          // Windchill float wc(double [temp °celcius], int [MPH windspeed]}
          //             returns (float [windchill °celcius])
          // PA -> "HG double p2h(double pascals)
          //             returns double [presure in inches mercury])
          // Dew Point double dp(double [temp °celc or °fahr], int [% rel humd])
          //             returns double [dew point °celcius or °fahrenheit]
          // Heat Index double hi(double [temp °celcius or °fahrenheit],
          //             int [% rel humidity], bool [°celcius input ?])
          //             returns heat index double [temp °celcius or °fahrenheit]
          // MED  double med(int [uvindex], int [altitude meters], bool [on water ?],
          //             bool [on snow ?], int [fitz skin type], int [spf applied])
          //             returns int [mins] to Min Erythemal Dose (MED) - sunburn

#define SERIAL_DEBUG                          // used for serial debugging
// #define METAR_DEBUG                        // used for serial debugging
// #define FCAST_DEBUG                        // used for serial debugging

int status  = WL_IDLE_STATUS;

RTCZero rtc;
 #define UTCOFFSET       -6                   // EDT 2nd sun mar | EST 1st sun NOV
 int UTCEPOCH            = UTCOFFSET * 3600;  // epoch timezone offset
 const char* TIMEZONE    = "MST";             // timezone lables
 const char* AMPM[2]     = {"am", "pm"};      // AM/PM preferred lables
 bool TIME24             = false;             // false = use 12 hour time
 int MIN                 = 60000;             // 1 minute of microseconds
 int SEC                 = 1000;              // 1 second of microseconds

SimpleTimer timer;
int GetNOAAInt           = 60;                // update interval (min)

const char* METARstation = "KDEN";
const char* FCASTstation = "BOU/62,61";

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);                   // setup onboard LED
    digitalWrite(LED_BUILTIN, LOW);                 // onboard LED off

//    #ifdef SERIAL_DEBUG
//        Serial.begin(115200);         
//        while (!Serial);                            // wait on serial open
//    #endif

    Serial.begin(115200); while (!Serial);
        
    while (status != WL_CONNECTED) status = WiFi.begin(ssid, pass);
        
    rtc.begin();
    unsigned long epoch = 0;
    // Serial.println(epoch);
    while (epoch == 0) epoch = WiFi.getTime();
    epoch = epoch + UTCEPOCH;
    rtc.setEpoch(epoch);

    #ifdef SERIAL_DEBUG
        Status();
    #endif

    timer.setInterval(MIN * GetNOAAInt, GetNOAA);
    digitalWrite(LED_BUILTIN, HIGH);                // setup complete
    GetNOAA();
}

void loop() { timer.run(); }

void GetNOAA() {
    METAR.station         = METARstation;
    GetMETAR(METAR);
    FCAST.station         = FCASTstation;
    NOAAForecast(FCAST);
    int CurrTempF = c2f(METAR.tempC.toInt());
    int TempF0    = FCAST.p0temp;
    int TempF1    = FCAST.p1temp;
    int TempF2    = FCAST.p2temp;   
    Serial.print("CurrTempF>"); Serial.print(CurrTempF); Serial.println("°F");
    Serial.print("TempF0>");   Serial.print(TempF0); Serial.println("°F");
    Serial.print("TempF1>");   Serial.print(TempF1); Serial.println("°F");
    Serial.print("TempF2>");   Serial.print(TempF2); Serial.println("°F");
}

void Status() {
    IPAddress ip = WiFi.localIP();
    char stat[100];
    sprintf(stat, 
        "Connected %02d-%02d-20%02d %02d%02d %s to %s on %d.%d.%d.%d with %ddBm",
                   rtc.getMonth(), rtc.getDay(), rtc.getYear(),
                   rtc.getHours(), rtc.getMinutes(), TIMEZONE,
                   WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI());
    Serial.println(stat);
}
