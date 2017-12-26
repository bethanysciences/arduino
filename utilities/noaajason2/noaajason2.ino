/*----------------------------------------------------------------------------------*
  NOAA JSON Test Code
  Verify   WiFi.setPins(8,7,4,2) for Adafruit Feather M0 WiFi
           WiFi credentials updated in file /lib/WiFiCreds.h
           api.weather.gov:443     ssl certificate added via WiFi101 updater
           
           https://forecast-v3.weather.gov/documentation?redirect=legacy
           Buckhead, GA lat 33.8774° lon -84.3046°
             api.weather.gov/points/33.8774,-84.3046/forecast
           Denver Business District lat 39.73° lon -104.99° Elev. 5276 ft
             api.weather.gov/points/39.73,-104.99/forecast

  For avr-libc ATMEL series 32bit SAMD21 CPUs e.g. Arduino MKR1000
  © 2017 Bob Smith https://github.com/bethanysciences/console
  MIT license
 *-------------------------------------------------------------------------------------*/
#include "RTCZero.h"           // arduino.cc/en/Reference/RTC
#include "WiFi101.h"           // www.arduino.cc/en/Reference/WiFi101
#include "WiFiCreds.h"         // WiFi credentials
#include "dtostrf.h"           // Convert float to string
#include "ArduinoHttpClient.h" // https://github.com/arduino-libraries/ArduinoHttpClient
#include "wxConversions.h"     // https://bethanysciences.github/wxConversions.h
#include "ArduinoJson.h"       // https://bblanchon.github.io/ArduinoJson/

int status  = WL_IDLE_STATUS;
const char* CONNSTAT[2] = {"DISC", "CONN"};      // Connection preferred lables
RTCZero rtc;
const char* FcstServer = "api.weather.gov";
int FcstPort = 443;
WiFiSSLClient client;

// client for NOAAForecast
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
  while (!Serial);                                // wait for serial to open
  Serial.print("START UP");
  while (status != WL_CONNECTED) status = WiFi.begin(ssid, pass);  // see WiFiCreds.h 
  Serial.println("> connected ");  
  digitalWrite(LED_BUILTIN, HIGH);                // lite onboard LED on connection

  rtc.begin();
  unsigned long epoch;
  Serial.print("Time (epoch) ");       Serial.println(epoch);
  while (epoch == 0) epoch = WiFi.getTime();
  rtc.setEpoch(epoch);

  char stat[80];
  IPAddress ip = WiFi.localIP();
  sprintf(stat, "%s ip addr %d.%d.%d.%d %ddBm %02d/%02d/%02d %02d:%02d",
          WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI(),
          rtc.getMonth(), rtc.getDay(), rtc.getYear(),
          rtc.getHours(), rtc.getMinutes());
  Serial.println(stat);
  NOAAForecast(); 
}

void loop() {
//  delay(15000); NOAAForecast(); 
}

void NOAAForecast() {
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
  client.print("GET /points/");
  client.print("33.8774,-84.3046");
  Serial.println("33.8774,-84.3046");
  client.println("/forecast HTTP/1.1");
  client.println("Host: api.weather.gov");
  client.print("User-Agent: bob@bethanysciences.net/arduinowx01/");
  client.println("/forecast");
  client.println("Accept: application/ld+json");
  client.println("Connection: close");
  client.println();
  Serial.println("request sent");
}

bool skipResponseHeaders() {
  char endOfHeaders[] = "\r\n\r\n";
  client.setTimeout(HTTPTimeout);
  bool ok = client.find(endOfHeaders);
  
  Serial.println("response headers skipped");
  
  return ok;
}
bool parseForecast() {
  const size_t bufferSize = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(2) + 
  JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(10) + 14*JSON_OBJECT_SIZE(13) + 6740;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(client);

  Serial.println("pretty print");
  root.prettyPrintTo(Serial); Serial.println();
  
  if (!root.success()) {
    
    Serial.println("Forecast parse fail");
    
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
  
  Serial.println("json string parsed");

  char p0[100];
  sprintf(p0,"period %s temp %d %s", Forecast.p0name, Forecast.p0tempC, Forecast.p0short);
  char p1[100];
  sprintf(p1,"period %s temp %d %s", Forecast.p1name, Forecast.p1tempC, Forecast.p1short);
  char p2[100];
  sprintf(p2,"period %s temp %d %s", Forecast.p2name, Forecast.p2tempC, Forecast.p2short);
  char p3[100];
  sprintf(p3,"period %s temp %d %s", Forecast.p3name, Forecast.p3tempC, Forecast.p3short);
  char p4[100];
  sprintf(p4,"period %s temp %d %s", Forecast.p4name, Forecast.p4tempC, Forecast.p4short);
  char p5[100]; 
  sprintf(p5,"period %s temp %d %s", Forecast.p5name, Forecast.p5tempC, Forecast.p5short);

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
  Forecast.hour   = hour_utc;
  Forecast.date   = date_utc;
  
  Serial.println("time stamp parsed");
    
  char FcstStamp[50];
  sprintf(FcstStamp, "%d/%d/%d %d:%2d",  
                      Forecast.month, Forecast.date, Forecast.year, 
                      Forecast.hour, Forecast.minute);
  char FcstTime[50];
  sprintf(FcstTime, "%02d:%02d", rtc.getHours(), rtc.getMinutes());
                      
  Serial.print("Pero0 ");  Serial.println(Forecast.p0name);
  Serial.print("Temp ");   Serial.println(Forecast.p0tempC);
  Serial.print("icon ");   Serial.println(Forecast.p0icon);   
  Serial.print("short ");  Serial.println(Forecast.p0short);  
  Serial.print("Pulled        "); Serial.print(FcstTime);   Serial.println("utc");  
  Serial.print("Forcast Stamp "); Serial.println(FcstStamp);
  Serial.print(p0); Serial.print(" | ");  Serial.println(p1);   
  Serial.print(p2); Serial.print(" | ");  Serial.println(p3);
  Serial.print(p4); Serial.print(" | ");  Serial.println(p5);
}
