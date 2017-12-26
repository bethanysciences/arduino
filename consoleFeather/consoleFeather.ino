/*----------------------------------------------------------------------------------*
  Weather and Information Console

  Verify   WiFi.setPins(8,7,4,2) for Adafruit Feather M0 WiFi
           api.weather.gov:443     ssl certificate added via WiFi101 updater
           UTCOFFSET = -4          set (EDT 2nd sun mar | EST 1st sun NOV)
           TIMEZONE  = "EDT"       timezone lables
           FCSTLAT = "33.8774" FCSTLON = "-84.3046" KPDK Peachtree Decalb, Atlanta
           FCSTLAT = "39.73" FCSTLON = "-104.99" Denver CO 80204
           WiFi credentials updated in file /lib/WiFiCreds.h
           
  For avr-libc ATMEL series 32bit SAMD21 CPUs e.g. Arduino MKR1000
  © 2017 Bob Smith https://github.com/bethanysciences/console
  MIT license
 *----------------------------------------------------------------------------------*/
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

#define SERIAL_DEBUG     // used for serial debugging

// -------------------------- Defines for WIFI Operations ------------------------- //

int status  = WL_IDLE_STATUS;
const char* CONNSTAT[2] = {"DISC", "CONN"};      // Connection preferred lables

// --------------  Defines for Adafruit Feather HX8357 TFT display ---------------- //

#define TFT_CS    9                         // HX8357 3.5" Feather Wing
#define TFT_DC    10                        // HX8357 3.5" Feather Wing
#define TFT_RST   5                         // HX8357 3.5" Feather Wing
#define TFT_MISO  5                         // HX8357 3.5" Feather Wing
#define SD_CS     2                         // HX8357 3.5" Feather Wing
#define FGROUND   HX8357_WHITE                          // primary text color
#define BGROUND   HX8357_BLUE                     // primary background color
#define GREEN     HX8357_GREEN
#define BLACK     HX8357_BLACK
#define RED       HX8357_RED
#define CYAN      HX8357_CYAN
#define MAGENTA   HX8357_MAGENTA
#define YELLOW    HX8357_YELLOW
int LineNu[80];                                             // line structure
int firstl = 1;
#define MARGIN    8                                        // margin (pixels)
#define SLINE     16                             // base line height (pixels)
#define LINE      25                                  // line height (pixels)
#define TAB       130                                   // tab width (pixels)
int CENTER;                                                // center (pixels)
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

// ------------------------- Defines for Time Displays ---------------------------- //

#define UTCOFFSET       -7            // EDT 2nd sun mar | EST 1st sun NOV
int UTCEPOCH          = -7 * 3600;    // epoch timezone offset
const char* TIMEZONE  = "MST";        // timezone lables
const char* AMPM[2]   = {"am", "pm"}; // AM/PM preferred lables
bool TIME24           = false;        // false = use 12 hour time
int MIN               = 60000;        // 1 minute of microseconds
int SEC               = 1000;         // 1 second of microseconds
bool BLINKCOLON       = false;
RTCZero rtc;
SimpleTimer timer;

// --------------- Defines for Adafruit LED Backpack i2c LED displays ------------- //

#define hTemp_I2C   0x71    // red
#define cTemp_I2C   0x72    // grn
#define lTemp_I2C   0x70    // blu
Adafruit_7segment HTempDisp = Adafruit_7segment();
Adafruit_7segment LTempDisp = Adafruit_7segment();
Adafruit_7segment CTempDisp = Adafruit_7segment();

// ------------------------------ Defines NOAA METAR ------------------------------ //

WiFiClient wifi;
   int currPort   = 80;
String currServer = "aviationweather.gov";
HttpClient xml    = HttpClient(wifi, currServer, currPort);
struct c_rent {
  String metar;
  String obstimeUTC;
int tempC;
int tempF;
int dewpointC;
int dewpointF;
   int winddirDeg;
   int windspeedKTS;
   int visibilitySM;
  char altSettingHG[6];
   int year;
   int month;
   int date;
   int hour;
   int minute;
  bool pm;
};
typedef struct c_rent Curr;
Curr Current;

// -------------------------- Defines NOAA FORECAST ------------------------------- //

const char* FCSTLAT    = "39.7300";
const char* FCSTLON    = "-104.9900";
const char* FcstServer = "api.weather.gov";
          int FcstPort = 443;
WiFiSSLClient client;                                 // client for NOAAForecast
const unsigned long HTTPTimeout = 10000;
struct f_cast {
const char* generated;
const char* p0;
const char* p1;
const char* p3;
const char* p4;
const char* p5;
const char* p0name;
const char* p1name;
const char* p2name;
const char* p3name;
const char* p4name;
const char* p5name;
int p0tempC;
int p1tempC;
int p2tempC;
int p3tempC;
int p4tempC;
int p5tempC;
const char* p0icon;
const char* p1icon;
const char* p2icon;
const char* p3icon;
const char* p4icon;
const char* p5icon;
const char* p0short;
const char* p1short;
const char* p2short;
const char* p3short;
const char* p4short;
const char* p5short;
   int year;
   int month;
   int date;
   int hour;
   int minute;
  bool pm;
};
typedef struct f_cast Fcst;
Fcst Forecast;

void setup() {            // ===================== SETUP ========================== //
  pinMode(LED_BUILTIN, OUTPUT);                   // setup onboard LED

  WiFi.setPins(8,7,4,2);                          // for Adafruit Feather M0 WiFi

  #ifdef SERIAL_DEBUG   // ----------------------- DEBUG -------------------------- //
    Serial.begin(115200);
    while (!Serial);                              // wait for serial to open
    Serial.print("START UP");
  #endif

  // ------------------------------------- LEDS SETUP ----------------------------- //
  HTempDisp.begin(hTemp_I2C);
  LTempDisp.begin(lTemp_I2C);
  CTempDisp.begin(cTemp_I2C);

  // ------------------------------------- TFT SETUP ------------------------------ //
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.setTextWrap(0);
  tft.fillScreen(BGROUND);

  int l = 0;                      // counts display lines based on LINE height pixels
  for (int i = 0; i <= tft.width()/LINE; i++) {
    LineNu[i] = l; l += LINE;
  }

  CENTER = tft.width()/2;
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(0, LineNu[firstl]);

  tft.print("TFT width ");     tft.println(tft.width());
  tft.print("TFT height ");    tft.println(tft.height());
  tft.print("TFT Lines ");     tft.println(tft.height() / LINE);

  #ifdef SERIAL_DEBUG   // -:--:--:--:--:--:--:--: DEBUG :----:--:--:--:--:--:--:- //
    Serial.print("TFT width ");  Serial.println(tft.width());
    Serial.print("TFT height "); Serial.println(tft.height());
    Serial.print("TFT Lines ");  Serial.println(tft.height() / LINE);
  #endif
  
  // ------------------------------------- WIFI SETUP ----------------------------- //

  tft.print("connect to >");    tft.print(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    tft.print(">");
    delay(500);
  }
  tft.print("> connected ");    tft.println();
  
  digitalWrite(LED_BUILTIN, HIGH);          // lite onboard LED to indicate connection

  // ------------------------------------- SET CLOCK ------------------------------ //
  
  rtc.begin();
  unsigned long epoch;

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.print("Time (epoch) ");       tft.println(epoch);
  
  while (epoch == 0) {
    epoch = WiFi.getTime();
    tft.print(".");
    delay(500);
  }
  
  epoch = epoch + UTCEPOCH;

  rtc.setEpoch(epoch);

  #ifdef SERIAL_DEBUG   // -:--:--:--:--:--:--:--: DEBUG :----:--:--:--:--:--:--:- //
    int hour12;
    bool pm;
    Serial.println();
    Serial.print("Epoch timezone adjusted ");  Serial.println(epoch);
    convertTime(rtc.getHours(), TIME24, &hour12, &pm);
    char stat[80];
    IPAddress ip = WiFi.localIP();
    sprintf(stat, "%s ip addr %d.%d.%d.%d %ddBm %02d/%02d/%02d %d:%02d%s %s",
          WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI(),
          rtc.getMonth(), rtc.getDay(), rtc.getYear(),
          hour12, rtc.getMinutes(), AMPM[pm], TIMEZONE);
    Serial.println(stat);
  #endif

  
  // ------------------------------------- SET TIMERS ----------------------------- //
  
  timer.setInterval(SEC * 5, statusLine);
  timer.setInterval(MIN * 10, NOAAmetar);
  timer.setInterval(MIN * 30, NOAAForecast);
  statusLine();  
  NOAAmetar();  
  NOAAForecast(); 
}

void loop() {             // ======================== LOOP ======================== //
  timer.run();
}

void NOAAmetar() {        // ======================== CURRENT ===================== //
  String req0 = "/adds/dataserver_current/httpparam?";
  String req1 = "dataSource=metars&requestType=retrieve&";
  String req2 = "format=xml&hoursBeforeNow=1&stationString=KFTG";
  String request = req0 + req1 + req2;
  xml.get(request);
  int code = xml.responseStatusCode();
  int len = xml.contentLength();
  String response = xml.responseBody();

  // ------------------------------------- PARSE ---------------------------------- //
  
  Current.metar         = xmlTakeParam(response, "raw_text");
  Current.obstimeUTC    = xmlTakeParam(response, "observation_time");
  Current.tempC         = xmlTakeParam(response, "temp_c").toInt();
  Current.dewpointC     = xmlTakeParam(response, "dewpoint_c").toInt();
  Current.winddirDeg    = xmlTakeParam(response, "wind_dir_degrees").toInt();
  Current.windspeedKTS  = xmlTakeParam(response, "wind_speed_kt").toInt();
  Current.visibilitySM  = xmlTakeParam(response, "visibility_statute_mi").toInt();
  double altim_in_hg    = xmlTakeParam(response, "altim_in_hg").toDouble();
  dtostrf(altim_in_hg, 5, 2, Current.altSettingHG);
  
  #ifdef SERIAL_DEBUG    // -:--:--:--:--:--:--:--: DEBUG :----:--:--:--:--:--:--:- //
    Serial.print("Code ");          Serial.print(code); 
    Serial.print("Length ");        Serial.println(len);
    Serial.print("Body ");          Serial.println(response);
    Serial.print("METAR --> ");     Serial.println(Current.metar );
    Serial.print("obstimeUTC   ");  Serial.println(Current.obstimeUTC);
    Serial.print("tempC        ");  Serial.println(Current.tempC);
    Serial.print("dewpointC    ");  Serial.println(Current.dewpointC);
    Serial.print("winddirDeg   ");  Serial.println(Current.winddirDeg);
    Serial.print("windspeedKTS ");  Serial.println(Current.windspeedKTS);
    Serial.print("visibilitySM ");  Serial.println(Current.visibilitySM);
    Serial.print("altSettingHG ");  Serial.println(Current.altSettingHG);
  #endif
  
  // ------------------------------------- PARSE TIME STAMP ----------------------- //
  int firstDash    = Current.obstimeUTC .indexOf("-");
  int secondDash   = Current.obstimeUTC .lastIndexOf("-");
  int firstT       = Current.obstimeUTC .indexOf("T");
  int firstColon   = Current.obstimeUTC .indexOf(":");
  int secondColon  = Current.obstimeUTC .lastIndexOf(":");

  String yearStr   = Current.obstimeUTC .substring(0, firstDash);
  String monthStr  = Current.obstimeUTC .substring(firstDash + 1);
  String dateStr   = Current.obstimeUTC .substring(secondDash + 1);
  String hourStr   = Current.obstimeUTC .substring(firstT + 1);
  String minuteStr = Current.obstimeUTC .substring(firstColon + 1);

  Current.year     = yearStr.toInt();
  Current.month    = monthStr.toInt();
  Current.minute   = minuteStr.toInt();
  int date_utc     = dateStr.toInt();
  int hour_utc     = hourStr.toInt();
  hour_utc        += UTCOFFSET;                         // TZ offset adjust
  if (hour_utc < 0) { 
    hour_utc += 24; 
    date_utc -= 1; 
  }
  else if (hour_utc > 23) { 
    hour_utc -= 24; 
    date_utc -= 1; 
  }
  else if (hour_utc == 0) hour_utc += 12;
  
  Current.pm = false;
  if (!TIME24) {                                         // 12/24 hour adjust
    if (hour_utc >= 12) { 
      hour_utc -= 12; 
      Current.pm = true; 
    }
  }
  Current.hour      = hour_utc;
  Current.date      = date_utc;
  Current.tempF     = c2f(Current.tempC);
  Current.dewpointF = c2f(Current.dewpointC);

  // ------------------------------------- RENDER --------------------------------- //

  char CurrStamp[50];
  sprintf(CurrStamp, "Current as of: %d/%d/%d %d:%2d%s",  
                      Current.month, Current.date, Current.year, 
                      Current.hour, Current.minute, AMPM[Current.pm]);


  char CurrtempF[50]; sprintf(CurrtempF, "Temperature %02d°F", 
                                                              Current.tempF);
  char CurrdwptF[50]; sprintf(CurrdwptF, "Dew Point %02d°F", 
                                                          Current.dewpointF);
  char Currbaro[50];  sprintf(Currbaro, "Barometer %s""hg", Current.altSettingHG);
  char Currwind[50];  sprintf(Currwind, "Wind %dKTS %03ddeg",
                                   Current.windspeedKTS, Current.winddirDeg);
  char Currvisi[50];  sprintf(Currvisi, "Visibility %d Miles", 
                                                       Current.visibilitySM);

  // ------------------------------------- WRITE TFT ------------------------------ //
  int firstl = 1;     int lastl = 3;                                   // line limits
  tft.fillRect(0, 0, tft.width(), LineNu[lastl+1], BGROUND);          // clear screen
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, LineNu[firstl]);
  tft.print(CurrStamp);
  tft.fillRect((tft.width()/3) * 2, LineNu[firstl]-10, tft.width()/2, 10, GREEN);

  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(FGROUND);
  
  tft.setCursor(MARGIN,          LineNu[firstl + 1]); tft.print(CurrdwptF);
  tft.setCursor(MARGIN + CENTER, LineNu[firstl + 1]); tft.print(Currbaro);
  tft.setCursor(MARGIN ,         LineNu[firstl + 2]); tft.print(Currwind);
  tft.setCursor(MARGIN + CENTER, LineNu[firstl + 2]); tft.print(Currvisi);
  
  // ------------------------------------- WRITE LED ------------------------------ //
  
  if (Current.tempF > 99) {
    CTempDisp.writeDigitNum(0, (Current.tempF / 100));
    CTempDisp.writeDigitNum(1, (Current.tempF / 10));
    CTempDisp.writeDigitNum(3, (Current.tempF % 10));
    CTempDisp.writeDigitRaw(4, B01110001);
  }
  else {
    CTempDisp.writeDigitNum(0, (Current.tempF / 10));
    CTempDisp.writeDigitNum(1, (Current.tempF % 10));
    CTempDisp.writeDigitRaw(3, B01110001);
    CTempDisp.writeDigitRaw(4, B00000000);
  }
  CTempDisp.writeDisplay();

  #ifdef SERIAL_DEBUG   // -:--:--:--:--:--:--:--: DEBUG :----:--:--:--:--:--:--:- //
    Serial.print(" ");    Serial.print(CurrtempF);  
    Serial.print(" ");    Serial.print(CurrdwptF);
    Serial.print(" ");    Serial.print(Currbaro);
    Serial.print(" ");    Serial.print(Currwind); 
    Serial.print(" ");    Serial.println(Currvisi); 

    char DCurrStamp[50]; sprintf(DCurrStamp, "%2d:%02d%s", Current.hour, 
                                                    Current.minute, AMPM[Current.pm]);
    int cchour;
    bool pm;
    convertTime(rtc.getHours(), TIME24, &cchour, &pm);
    char TimeStamp[50];
    sprintf(TimeStamp, "%d:%02d%s", cchour, rtc.getMinutes(), AMPM[pm]);
    Serial.print("Currents --->");        Serial.print("\t");
    Serial.print("pulled");               Serial.print("\t");
    Serial.print(" stamp");               Serial.print("\t");
    Serial.print("temp");                 Serial.print("\t");
    Serial.print("dwpt");                 Serial.print("\t");
    Serial.print("baro");                 Serial.print("\t");
    Serial.print("wspd");                 Serial.print("\t");
    Serial.print("wdir");                 Serial.print("\t");
    Serial.println("visi");
  
    Serial.print("             ");        Serial.print("\t");  
    Serial.print(TimeStamp);              Serial.print("\t");
    Serial.print(DCurrStamp);             Serial.print("\t");
    Serial.print(Current.tempC);          Serial.print("\t");
    Serial.print(Current.dewpointC);      Serial.print("\t");
    Serial.print(Current.altSettingHG);   Serial.print("\t");
    Serial.print(Current.windspeedKTS);   Serial.print("\t");
    Serial.print(Current.winddirDeg);     Serial.print("\t");
    Serial.println(Current.visibilitySM);
  #endif  
}

void NOAAForecast() {     // ======================== FORECAST ==================== //
  if (connectServer(FcstServer, FcstPort)) {
    if (sendRequest() && skipResponseHeaders()) parseForecast();
    client.stop();
    return;
  }
}

bool connectServer(const char* SSLServer, int SSLPort) {
  bool ok = client.connectSSL(SSLServer, SSLPort);
  return ok;
}


bool sendRequest() {
  // client.println("GET /points/33.8774,-84.3046/forecast HTTP/1.1");
  // client.println("GET /points/39.7467,-104.999/forecast HTTP/1.1");
  client.println("GET /gridpoints/BOU/62,61/forecast HTTP/1.1");
  // client.println("GET /gridpoints/FFC/52,92 HTTP/1.1");
  client.println("Host: api.weather.gov");
  client.println("Accept: application/ld+json");
  client.print("User-Agent: bob@bethanysciences.net/arduinowx01");
  client.println("/forecast");
  client.println("Connection: close");
  client.println();
}

bool skipResponseHeaders() {
  char endOfHeaders[] = "\r\n\r\n";
  client.setTimeout(HTTPTimeout);
  bool ok = client.find(endOfHeaders);
  return ok;
}

bool parseForecast() {
  const size_t bufferSize = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(2) + 
  JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(10) + 14*JSON_OBJECT_SIZE(13) + 6740;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  
  JsonObject& root = jsonBuffer.parseObject(client);

  #ifdef SERIAL_DEBUG   // -:--:--:--:--:--:--:--: DEBUG :----:--:--:--:--:--:--:- //
    Serial.println("pretty print"); root.prettyPrintTo(Serial); Serial.println();
  #endif 
  
  if (!root.success()) {
  #ifdef SERIAL_DEBUG   // -:--:--:--:--:--:--:--: DEBUG :----:--:--:--:--:--:--:- //
    Serial.println("Forecast parse fail");
  #endif 
    return false;
  }
  JsonObject& _context  = root["@context"];
   Forecast.generated   = root["generatedAt"];
  JsonArray& periods    = root["periods"];
  JsonObject& periods0  = periods[0];   
   Forecast.p0name      = periods0["name"];  
 const char* temphold0  = periods0["temperature"];
   Forecast.p0tempC     = atoi(temphold0);
   Forecast.p0icon      = periods0["icon"];
   Forecast.p0short     = periods0["shortForecast"];
  JsonObject& periods1  = periods[1];
   Forecast.p1name      = periods1["name"];
 const char* temphold1  = periods1["temperature"];
   Forecast.p1tempC     = atoi(temphold1);
   Forecast.p1icon      = periods1["icon"];
   Forecast.p1short     = periods1["shortForecast"];
  JsonObject& periods2  = periods[2];
   Forecast.p2name      = periods2["name"];
 const char* temphold2  = periods2["temperature"];
   Forecast.p2tempC     = atoi(temphold2);
   Forecast.p2icon      = periods2["icon"];
   Forecast.p2short     = periods2["shortForecast"];
  JsonObject& periods3  = periods[3];
   Forecast.p3name      = periods3["name"];
 const char* temphold3  = periods3["temperature"];
   Forecast.p3tempC     = atoi(temphold3);
   Forecast.p3icon      = periods3["icon"];
   Forecast.p3short     = periods3["shortForecast"];
  JsonObject& periods4  = periods[4];
   Forecast.p4name      = periods4["name"];
 const char* temphold4  = periods4["temperature"];
   Forecast.p4tempC     = atoi(temphold4);
   Forecast.p4icon      = periods4["icon"];
   Forecast.p4short     = periods4["shortForecast"];
  JsonObject& periods5  = periods[5];
   Forecast.p5name      = periods5["name"];
 const char* temphold5  = periods5["temperature"];
   Forecast.p5tempC     = atoi(temphold5);
   Forecast.p5icon      = periods5["icon"];
   Forecast.p5short     = periods5["shortForecast"];   

  // ------------------------------------- PARSE TIME STAMP ----------------------- //
  
  String stamp(Forecast.generated);
  int firstDash    = stamp.indexOf("-");
  int secondDash   = stamp.lastIndexOf("-");
  int firstT       = stamp.indexOf("T");
  int firstColon   = stamp.indexOf(":");
  int secondColon  = stamp.lastIndexOf(":");
  String yearStr   = stamp.substring(0, firstDash);
  String monthStr  = stamp.substring(firstDash + 1);
  String dateStr   = stamp.substring(secondDash + 1);
  String hourStr   = stamp.substring(firstT + 1);
  String minuteStr = stamp.substring(firstColon + 1);
  Forecast.year    = yearStr.toInt();
  Forecast.month   = monthStr.toInt();
  Forecast.minute  = minuteStr.toInt();
  int date_utc     = dateStr.toInt();
  int hour_utc     = hourStr.toInt();
  hour_utc        += UTCOFFSET;                              // TZ offset adjust
  if (hour_utc < 0) { 
    hour_utc += 24; 
    date_utc -= 1; 
  }
  else if (hour_utc > 23) { 
    hour_utc -= 24; 
    date_utc -= 1; 
  }
  else if (hour_utc == 0) hour_utc += 12;
  
  Forecast.pm = false;
  if (!TIME24) {                                              // 12/24 hour adjust
    if (hour_utc >= 12) { 
      hour_utc -= 12; 
      Forecast.pm = true; 
    }
  }
  Forecast.hour   = hour_utc;
  Forecast.date   = date_utc;

  // ------------------------------------- RENDER FORECAST ------------------------ //
  
  char FcstStamp[50];
  sprintf(FcstStamp, "Forecast as of: %d/%d/%d %d:%2d%s",  
                      Forecast.month, Forecast.date, Forecast.year, 
                      Forecast.hour, Forecast.minute, AMPM[Forecast.pm]);
  char p0[100];
  sprintf(p0,"%s %d %s", Forecast.p0name, Forecast.p0tempC, Forecast.p0short);
  char p1[100];
  sprintf(p1,"%s %d %s", Forecast.p1name, Forecast.p1tempC, Forecast.p1short);
  char p2[100];
  sprintf(p2,"%s %d %s", Forecast.p2name, Forecast.p2tempC, Forecast.p2short);
  char p3[100];
  sprintf(p3,"%s %d %s", Forecast.p3name, Forecast.p3tempC, Forecast.p3short);
  char p4[100];
  sprintf(p4,"%s %d %s", Forecast.p4name, Forecast.p4tempC, Forecast.p4short);
  char p5[100]; 
  sprintf(p5,"%s %d %s", Forecast.p5name, Forecast.p5tempC, Forecast.p5short);

  // ------------------------------------- WRITE TFT ------------------------------ //

  int firstl = 5;     int lastl = 11;                           // display line limits
  tft.fillRect(0, LineNu[firstl]-LINE, tft.width(), LineNu[lastl], BGROUND);  // clear
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, LineNu[firstl]);
  tft.fillRect((tft.width()/3) * 2, LineNu[firstl]-10, tft.width()/2, 10, GREEN);
  tft.print(FcstStamp);
  tft.setCursor(MARGIN, LineNu[firstl+1]);  tft.print(p0);
  tft.setCursor(MARGIN, LineNu[firstl+2]);  tft.print(p1);
  tft.setCursor(MARGIN, LineNu[firstl+3]);  tft.print(p2);
  tft.setCursor(MARGIN, LineNu[firstl+4]);  tft.print(p3);
  tft.setCursor(MARGIN, LineNu[firstl+5]);  tft.print(p4);
  tft.setCursor(MARGIN, LineNu[firstl+6]);  tft.print(p5);

  // ------------------------------------- WRITE LEDS ----------------------------- //

  int LTemp, HTemp;
  
  if (Forecast.p0tempC <= Forecast.p1tempC) {
    LTemp = Forecast.p0tempC;
    HTemp = Forecast.p1tempC;
  } else {
    LTemp = Forecast.p1tempC;
    HTemp = Forecast.p0tempC;
  }
  
  if (LTemp > 99) {
    LTempDisp.writeDigitNum(0, (LTemp / 100));
    LTempDisp.writeDigitNum(1, (LTemp / 10));
    LTempDisp.writeDigitNum(3, (LTemp % 10));
    LTempDisp.writeDigitRaw(4, B01110001);
  } else {
    LTempDisp.writeDigitNum(0, (LTemp / 10));
    LTempDisp.writeDigitNum(1, (LTemp % 10));
    LTempDisp.writeDigitRaw(3, B01110001);
    LTempDisp.writeDigitRaw(4, B00000000);
  }
  if (HTemp > 99) {
    HTempDisp.writeDigitNum(0, (HTemp / 100));
    HTempDisp.writeDigitNum(1, (HTemp / 10));
    HTempDisp.writeDigitNum(3, (HTemp % 10));
    HTempDisp.writeDigitRaw(4, B01110001);
  } else {
    HTempDisp.writeDigitNum(0, (HTemp / 10));
    HTempDisp.writeDigitNum(1, (HTemp % 10));
    HTempDisp.writeDigitRaw(3, B01110001);
    HTempDisp.writeDigitRaw(4, B00000000);
  }
  HTempDisp.writeDisplay();
  LTempDisp.writeDisplay();

  #ifdef SERIAL_DEBUG   // -:--:--:--:--:--:--:--: DEBUG :----:--:--:--:--:--:--:- //
    Serial.println(client);
    Serial.print("Pero0 ");  Serial.println(Forecast.p0name);
    Serial.print("Temp ");   Serial.println(Forecast.p0tempC);
    Serial.print("icon ");   Serial.println(Forecast.p0icon);   
    Serial.print("short ");  Serial.println(Forecast.p0short);  
    
    char FcstTime[50];
    sprintf(FcstTime, "%02d:%02d", rtc.getHours(), rtc.getMinutes());
    
    Serial.print("Pulled ");                Serial.print(FcstTime);
    Serial.print(" | Updated ");            Serial.println(FcstStamp);
    Serial.print(p0); Serial.print(" | ");  Serial.println(p1);   
    Serial.print(p2); Serial.print(" | ");  Serial.println(p3);
    Serial.print(p4); Serial.print(" | ");  Serial.println(p5);
  #endif
}

void statusLine() {       // ======================== STATUS LINE ================= //
  int qual        = 2 * (WiFi.RSSI() + 100);                 // signal strength in %
  int xstart      = MARGIN;
  int lineH       = SLINE - 4;                               // Text line height
  int ystart      = tft.height() - lineH - MARGIN;           // graphic vertical pos
  int tline       = tft.height() - MARGIN;                   // text vertical pos
  int bars        = 4;                                       // number of bars
  int barG        = 2;                                       // gap between bars
  int barW        = 6;                                       // bar width
  int barSpace    = barG + barW;
  int iconW       = (barW + barG) * bars;                    // icon width

  bool conn;
  IPAddress ip    = WiFi.localIP();
 
  if(qual < 45) {                // bar 1
    conn = false;
            tft.fillRect(xstart, ystart, barW, lineH, YELLOW);
            tft.fillRect(xstart + barSpace, ystart, barW, lineH, BGROUND);
            tft.fillRect(xstart + (barSpace * 2), ystart, barW, lineH, BGROUND);
            tft.fillRect(xstart + (barSpace * 3), ystart, barW, lineH, BGROUND);
  }
  if(qual >= 45) {               // bar 2
    conn = true;
            tft.fillRect(xstart, ystart, barW, lineH, GREEN);
            tft.fillRect(xstart + barSpace, ystart, barW, lineH, GREEN);
            tft.fillRect(xstart + (barSpace * 2), ystart, barW, lineH, BGROUND);
            tft.fillRect(xstart + (barSpace * 3), ystart, barW, lineH, BGROUND);
  }
  if(qual >= 70) {               // bar 3
            tft.fillRect(xstart, ystart, barW, lineH, GREEN);
            tft.fillRect(xstart + barSpace, ystart, barW, lineH, GREEN);
            tft.fillRect(xstart + (barSpace * 2), ystart, barW, lineH, GREEN);
            tft.fillRect(xstart + (barSpace * 3), ystart, barW, lineH, BGROUND);
  }
  if(qual >= 90) {               // bar 4
            tft.fillRect(xstart, ystart, barW, lineH, GREEN);
            tft.fillRect(xstart + barSpace, ystart, barW, lineH, GREEN);
            tft.fillRect(xstart + (barSpace * 2), ystart, barW, lineH, GREEN);
            tft.fillRect(xstart + (barSpace * 3), ystart, barW, lineH, GREEN);
  }
  
  char cStat[80]; sprintf(cStat, "%s Quality %d%% RSSI %ddBm %s %d.%d.%d.%d", 
         CONNSTAT[conn], qual, WiFi.RSSI(), WiFi.SSID(), ip[0], ip[1], ip[2], ip[3]);

  if (qual >= 45) return;

  tft.fillRect(xstart + iconW + MARGIN, ystart, tft.width(), 
                                    lineH + MARGIN, BGROUND);         // clear screen
  tft.setCursor(xstart + iconW + MARGIN, tline);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.print(cStat);
}

/*
void renderBattery() {       // ================== RENDER BATTERY ================= //
  int firstl = 12;    int lastl = 12;         // start and end lines
  int xstart = MARGIN;                        // horizontal placement
  int ystart = LineNu[firstl];                // vertical placement
  int icon = 18;                              // icon width
  int bar = (icon - 6) / 3;                   // bar width
  int sensorValue = analogRead(ADC_BATTERY);

  float voltage = sensorValue * (4.3 / 1023.0);         // Convert reading to voltage

  tft.fillRect(xstart, LineNu[firstl], icon, SLINE, BGROUND);         // clear screen

  // draw battery icon
  tft.drawLine (xstart+1,      ystart,   xstart+1+icon-4, ystart,   FGROUND);
  tft.drawLine (xstart,        ystart+1, xstart,          ystart+5, FGROUND);
  tft.drawLine (xstart+icon-1, ystart+2, xstart+icon-1,   ystart+4, FGROUND);
  tft.drawPixel(xstart+icon-2, ystart+1, FGROUND);
  tft.drawPixel(xstart+icon-2, ystart+1, FGROUND);
  tft.drawLine (xstart+1,      ystart+6, xstart+icon-4,   ystart+6, FGROUND);
  tft.drawPixel(xstart+icon-2, ystart+5, FGROUND);
  tft.drawPixel(xstart+icon-3, ystart+5, FGROUND);  
  tft.drawPixel(xstart+icon-3, ystart+6, FGROUND);

  // draw bars
  if (voltage > 4.26F) tft.fillRect(xstart, ystart, bar*3, 3, GREEN);          // full
  else if ((voltage <= 4.26F) && (voltage >= 4.1F)) for (uint8_t i = 0; i < 3; i++) {
    tft.fillRect(xstart + (i * bar), ystart, bar-1, 3, GREEN);               // 3 bars
  }
  else if ((voltage < 4.1F) && (voltage >= 3.8F)) for (uint8_t i = 0; i < 2; i++) {
    tft.fillRect(xstart + (i * bar), ystart, bar-1, 3, YELLOW);              // 2 bars
  }
  else if ((_battery < 3.8F) && (_battery >= 3.4F)) {                         // 1 bar
    tft.fillRect(xstart, ystart, bar-1, 3, MAGENTA);
  }
}
*/
