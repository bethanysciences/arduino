/*------------------------------------------------------------------------------------*
  Get and parse NOAA Forecast api.weather.gov
  For avr-libc ATMEL series 32bit SAMD21 CPUs e.g. Arduino MKR1000
  Returns as JSON-LD 

  ©2017-2018 Bob Smith https://github.com/bethanysciences/getwx
  MIT license
 
 
 *-----------------------------------------------------------------------------------*/
#include "ArduinoHttpClient.h" // github.com/arduino-libraries/ArduinoHttpClient
#include "ArduinoJson.h"       // bblanchon.github.io/ArduinoJson/

struct FCASTstruct {
  const char* server;
          int port;  
  const char* station;
  const char* generated;
  const char* p0;
  const char* p1;
  const char* p2;
  const char* p0name;
  const char* p1name;
  const char* p2name;
          int p0temp;
          int p1temp;
          int p2temp;
  const char* p0icon;
  const char* p1icon;
  const char* p2icon;
  const char* p0short;
  const char* p1short;
  const char* p2short;
          int fyear;
          int fmonth;
          int fdate;
          int fhour;
          int fminute;
};
FCASTstruct FCAST;

void NOAAForecast(FCASTstruct &f) {
    f.server = "api.weather.gov";
    f.port = 443;
    WiFiSSLClient fcastClient;
    fcastClient.connectSSL(f.server, f.port);
    fcastClient.print("GET /gridpoints/");
    fcastClient.print(f.station);
    fcastClient.println("/forecast HTTP/1.1");
    fcastClient.println("Host: api.weather.gov");
    fcastClient.println("User-Agent: bob@bethanysciences.net/arduinowx01");
    fcastClient.println("Accept: application/ld+json");
    fcastClient.println("Connection: close");
    fcastClient.println();
    char endOfHeaders[] = "\r\n\r\n";
    fcastClient.find(endOfHeaders);

    const size_t bufferSize = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(0) + 
                              JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 
                              JSON_OBJECT_SIZE(10) + 14*JSON_OBJECT_SIZE(13) + 6980;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(fcastClient);

    // root.prettyPrintTo(Serial);           // use to pretty print json respponse

    if (!root.success()) return;             // break out if root doesn't process
    
    fcastClient.stop();
    
    JsonObject& _context    = root["@context"];
    f.generated             = root["generatedAt"];
    JsonArray& periods      = root["periods"];
    JsonObject& periods0    = periods[0];
    f.p0name                = periods0["name"];
    const char* temphold0   = periods0["temperature"];
    f.p0temp                = atoi(temphold0);
    f.p0icon                = periods0["icon"];
    f.p0short               = periods0["shortForecast"];
    JsonObject& periods1    = periods[1];
    f.p1name                = periods1["name"];
    const char* temphold1   = periods1["temperature"];
    f.p1temp                = atoi(temphold1);
    f.p1icon                = periods1["icon"];
    f.p1short               = periods1["shortForecast"];
    JsonObject& periods2    = periods[2];
    f.p2name                = periods2["name"];
    const char* temphold2   = periods2["temperature"];
    f.p2temp                = atoi(temphold2);
    f.p2icon                = periods2["icon"];
    f.p2short               = periods2["shortForecast"];

    String fgen        = f.generated;
    int firstDash      = fgen.indexOf("-");
    int secondDash     = fgen.lastIndexOf("-");
    int firstT         = fgen.indexOf("T");
    int firstColon     = fgen.indexOf(":");
    int secondColon    = fgen.lastIndexOf(":");
    String yearStr     = fgen.substring(0, firstDash);
    String monthStr    = fgen.substring(firstDash + 1);
    String dateStr     = fgen.substring(secondDash + 1);
    String hourStr     = fgen.substring(firstT + 1);
    String minuteStr   = fgen.substring(firstColon + 1);
    f.fyear            = yearStr.toInt();
    f.fmonth           = monthStr.toInt();
    f.fdate            = dateStr.toInt();
    f.fhour            = hourStr.toInt();
    f.fminute          = minuteStr.toInt();

    #ifdef SERIAL_DEBUG
        Serial.print(FCAST.p0name); Serial.print(" "); Serial.print(FCAST.p0temp); 
        Serial.print(" ");          Serial.println(FCAST.p0short);
        Serial.print(FCAST.p1name); Serial.print(" "); Serial.print(FCAST.p1temp); 
        Serial.print(" ");          Serial.println(FCAST.p1short);
        Serial.print(FCAST.p2name); Serial.print(" "); Serial.print(FCAST.p2temp); 
        Serial.print(" ");          Serial.println(FCAST.p2short);        
        #ifdef FCAST_DEBUG
            StatusFCAST(FCAST);
        #endif
    #endif
}

void StatusFCAST(FCASTstruct &f) {
    char FStamp[50]; sprintf(FStamp, "CURR Obs time:  %d/%d %d%dGMT",  
                        f.fmonth, f.fdate, f.fhour, f.fminute);
    Serial.print("p0 \t"); Serial.print(f.p0name); Serial.print("\t");    
    Serial.print(f.p0temp); Serial.print("°F \t");   
    Serial.println(f.p0short);
    
    Serial.print("p1 \t");     Serial.print(f.p1name); Serial.print("\t");    
    Serial.print(f.p1temp); Serial.print("°F \t");   
    Serial.println(f.p1short);
    
    Serial.print("p2 \t");     Serial.print(f.p2name); Serial.print("\t");    
    Serial.print(f.p2temp); Serial.print("°F \t");   
    Serial.println(f.p2short);
}
