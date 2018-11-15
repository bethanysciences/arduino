/*---------------------------------------------------------------------------------*
  NOAA JSON Test Code
  Verify   WiFi.setPins(8,7,4,2) for Adafruit Feather M0 WiFi
           WiFi credentials updated in file /lib/WiFiCreds.h
           api.weather.gov:443     ssl certificate added via WiFi101 updater
           
  For avr-libc ATMEL series 32bit SAMD21 CPUs e.g. Arduino MKR1000
  © 2017 Bob Smith https://github.com/bethanysciences/console
  MIT license
 *--------------------------------------------------------------------------------*/
#include "WiFi101.h"           // arduino.cc/en/Reference/WiFi101
#include "WiFiCreds.h"         // WiFi credentials
#include "ArduinoHttpClient.h" // github.com/arduino-libraries/ArduinoHttpClient
#include "ArduinoJson.h"       // bblanchon.github.io/ArduinoJson/

int status  = WL_IDLE_STATUS;

struct FCASTstruct {
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

const char*   FcstServer = "api.weather.gov";
int FcstPort = 443;
const char* FCASTStation = "BOU/62,61";
WiFiSSLClient client;
const unsigned long HTTPTimeout = 10000;   

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);                   // setup onboard LED
    digitalWrite(LED_BUILTIN, LOW);                 // onboard LED
    while (!Serial);                                // wait for serial to open
    Serial.print("|-start-> ");
    while (status != WL_CONNECTED) status = WiFi.begin(ssid, pass);
    Serial.print("|-connected "); Serial.print(WiFi.RSSI()); Serial.print("db ->");
    digitalWrite(LED_BUILTIN, HIGH);
    NOAAForecast();
}

void loop() { }

void NOAAForecast() {
    if (connectServer(FcstServer, FcstPort)) {
        Serial.print("|-connectServer->"); 
        if (sendRequest() && skipResponseHeaders()) parseForecast();
        client.stop();
        return;
    }
}
bool connectServer(const char* SSLServer, int SSLPort) {
    Serial.print("|-connectSSL->"); 
    bool ok = client.connectSSL(SSLServer, SSLPort);
    return ok;
}

bool sendRequest() {   
    Serial.println("|-sendRequest->"); 
    client.print("GET /gridpoints/");
    client.print(FCASTStation);
    client.println("/forecast HTTP/1.1");
    client.println("Host: api.weather.gov");
    client.println("User-Agent: bob@bethanysciences.net/arduinowx01");
    client.println("Accept: application/ld+json");
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
    const size_t bufferSize = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(0) + 
                              JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 
                              JSON_OBJECT_SIZE(10) + 14*JSON_OBJECT_SIZE(13) + 6980;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(client);

    #ifdef DEBUG 
        Serial.println("|print root->"); 
        root.prettyPrintTo(Serial); 
        Serial.println();
    #endif

    if (!root.success()) {
        Serial.println("Forecast parse fail");
        return false;
    }
    
    JsonObject& _context    = root["@context"];
    FCAST.generated         = root["generatedAt"];
    JsonArray& periods      = root["periods"];
    JsonObject& periods0    = periods[0];
    FCAST.p0name            = periods0["name"];
    const char* temphold0   = periods0["temperature"];
    FCAST.p0temp            = atoi(temphold0);
    FCAST.p0icon            = periods0["icon"];
    FCAST.p0short           = periods0["shortForecast"];
    JsonObject& periods1    = periods[1];
    FCAST.p1name            = periods1["name"];
    const char* temphold1   = periods1["temperature"];
    FCAST.p1temp            = atoi(temphold1);
    FCAST.p1icon            = periods1["icon"];
    FCAST.p1short           = periods1["shortForecast"];
    JsonObject& periods2    = periods[2];
    FCAST.p2name            = periods2["name"];
    const char* temphold2   = periods2["temperature"];
    FCAST.p2temp            = atoi(temphold2);
    FCAST.p2icon            = periods2["icon"];
    FCAST.p2short           = periods2["shortForecast"];
    
    Serial.print("p0 \t"); Serial.print(FCAST.p0name); Serial.print("\t");    
    Serial.print(FCAST.p0temp); Serial.print("°F \t");   
    Serial.println(FCAST.p0short);
    
    Serial.print("p1 \t");     Serial.print(FCAST.p1name); Serial.print("\t");    
    Serial.print(FCAST.p1temp); Serial.print("°F \t");   
    Serial.println(FCAST.p1short);
    
    Serial.print("p2 \t");     Serial.print(FCAST.p2name); Serial.print("\t");    
    Serial.print(FCAST.p2temp); Serial.print("°F \t");   
    Serial.println(FCAST.p2short);
}
