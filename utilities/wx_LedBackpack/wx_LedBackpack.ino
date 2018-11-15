#include <SPI.h>
#include <Wire.h>
#include "Adafruit_WINC1500.h"
#include "Adafruit_WINC1500Udp.h"
#include "ArduinoJson.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "TimeLib.h"

Adafruit_AlphaNum4 alphaGRN = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alphaWHT = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alphaYEL = Adafruit_AlphaNum4();
Adafruit_7segment numYEL    = Adafruit_7segment();
Adafruit_8x16matrix matrix  = Adafruit_8x16matrix();

#define WINC_CS  8
#define WINC_IRQ 7
#define WINC_RST 4
#define WINC_EN  2
Adafruit_WINC1500 WiFi(WINC_CS, WINC_IRQ, WINC_RST);
Adafruit_WINC1500Client client;
Adafruit_WINC1500UDP Udp;

int status = WL_IDLE_STATUS;
char ssid[] = "pizgloria";
char pass[] = "HomeCall4";

unsigned int localPort = 8888;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE ];
IPAddress timeServer(129, 6, 15, 28);
const int timeZone = -4;
#define WUhost "api.wunderground.com"
#define WUkey  "712a91952a8b182e"
#define WUloc  "PDK"

char LastForecastFetch[30];
char LastConditionsFetch[30];
char ForecastDay0[30];
char tempStr[30];
char LtempStr[30];
char presStr[30];
char humStr[30];
char windStr[30];
char condStr[30];
char tStamp[10];
char NextFetch[20];
int fetchInt = 60;

void setup() {
#ifdef WINC_EN
    pinMode(WINC_EN, OUTPUT);
    digitalWrite(WINC_EN, HIGH);
#endif
    Serial.begin(115200);
    Wire.begin();
//    while (!Serial) { ; }
    while (status != WL_CONNECTED) { status = WiFi.begin(ssid, pass); }
    for (int i=9; i > 0; i--){ ; numYEL.print(i); numYEL.writeDisplay(); delay(1000); }
    // printWifiStatus();
    alphaGRN.begin(0x73);
    alphaWHT.begin(0x72);
    alphaYEL.begin(0x76);
    numYEL.begin(0x77);
    matrix.begin(0x70);
    matrix.setRotation(3);
    Udp.begin(localPort);
    setSyncProvider(getNtpTime);
    delay(2000);
    for (int8_t x=0; x>=-70; x--) {
        matrix.clear();
        matrix.setCursor(x,0);
        matrix.print("Start");
        matrix.writeDisplay();
        delay(100);
    }
    delay(2000);
    numYEL.print((hour()*100)+minute()); numYEL.writeDisplay();
}

static char respBuf[8192];

void loop() {    
    fetchWU("forecast");
    delay(5000);
    fetchWU("conditions");
    for (int i = fetchInt ; i > 0 ; i--){ 
        matrix.clear();
        matrix.setCursor(0,0);
        matrix.print(i); matrix.writeDisplay();
        numYEL.print((hour()*100)+minute()); numYEL.writeDisplay();
        delay(10000);
     }
}

void fetchWU(char *WUtype) {
    client.stop();
    delay(100);
    if (!client.connect(WUhost, 80)) { return; }
    client.print("GET /api/");
    client.print(WUkey);
    client.print("/");
    client.print(WUtype);
    client.print("/q/");
    client.print(WUloc);
    client.println(".json HTTP/1.1");
    client.print("Host: "); 
    client.println(WUhost);
    client.println("Connection: close");
    client.println();
    client.flush();
    int respLen = 0;
    bool skip_headers = true;
    while (client.connected() || client.available()) {
        if (skip_headers) { 
            String aLine = client.readStringUntil('\n'); 
            if (aLine.length() <= 1) { skip_headers = false; } 
        }
        else {
            int bytesIn;
            bytesIn = client.read((uint8_t *)&respBuf[respLen], sizeof(respBuf) - respLen);
            if (bytesIn > 0) {  respLen += bytesIn; if (respLen > sizeof(respBuf)) respLen = sizeof(respBuf); }
            else if (bytesIn < 0) { }
        }
        delay(1);
    }
    client.stop();
    if (respLen >= sizeof(respBuf)) { return; }
    respBuf[respLen++] = '\0';
    if (WUtype == "conditions") {
        parseConditions(respBuf);
        if (true) { return; }
        if (false) { Serial.println("Parse fail -conditions > \r\n"); return; }
    }
    if (WUtype == "forecast") {
        parseForecast(respBuf);
        if (true) { return; }
        if (false) { Serial.println("Parse fail -forecast> \r\n"); return; }
    }
    return; 
}

bool parseForecast(char *json) {
    StaticJsonBuffer<8192> jsonBuffer;
    char *jsonstart = strchr(json, '{');
    if (jsonstart == NULL) { return false; }
    json = jsonstart;
    JsonObject& root = jsonBuffer.parseObject(json);
    JsonObject& current = root["forecast"]["simpleforecast"];
    if (!root.success()) { return false; }
    int DateDa = current["forecastday"][0]["date"]["day"];
    int DateMo = current["forecastday"][0]["date"]["month"];
    int DateYr = current["forecastday"][0]["date"]["year"];
    int Htemp =  current["forecastday"][0]["high"]["fahrenheit"];
    int Ltemp =  current["forecastday"][0]["low"]["fahrenheit"];
    sprintf(ForecastDay0, "Forecast %02d/%02d/%02d", DateMo, DateDa, DateYr);
    sprintf(tStamp, "%02d:%02d", hour(),minute());
    alphaWHT.writeDigitAscii(0, ( Htemp / 10 + 48 ) );
    alphaWHT.writeDigitAscii(1, ( Htemp % 10 + 48 ) );
    alphaWHT.writeDigitAscii(2, 'F');
    alphaWHT.writeDigitAscii(3, ' ');
    alphaWHT.writeDisplay();
    alphaGRN.writeDigitAscii(0, (Ltemp / 10 + 48 ) );
    alphaGRN.writeDigitAscii(1, (Ltemp % 10 + 48) );
    alphaGRN.writeDigitAscii(2, 'F');
    alphaGRN.writeDigitAscii(3, ' ');
    alphaGRN.writeDisplay();
    return true;
}

bool parseConditions(char *json) {
    StaticJsonBuffer<8192> jsonBuffer;
    char *jsonstart = strchr(json, '{');
    if (jsonstart == NULL) { return false; }
    json = jsonstart;
    JsonObject& root = jsonBuffer.parseObject(json);
    JsonObject& current = root["current_observation"];
    if (!root.success()) { return false; }
    double temp =          current["temp_f"];
    const char *cond =     current["weather"];
    int visi =             current["visibility_mi"];
    const char *UV =       current["UV"];
    const char *humi =     current["relative_humidity"];
    const char *pres_in =  current["pressure_in"];
    const char *trend =    current["pressure_trend"];
    double wind_mph =      current["wind_mph"];
    int wind_mphR =        round(wind_mph);
    int wind_head =        current["wind_degrees"];
    const char *wind_dir = current["wind_dir"];
    int tempR = round(temp);
    sprintf(presStr, "Baro %sinHG-%s", pres_in, trend);
    sprintf(condStr, "%s %imi-vis UV-%s", cond, visi, UV);
    sprintf(humStr,  "Humidity-%s", humi);
    sprintf(windStr, "Wind %iMpH-%s", wind_mphR, wind_dir);
    sprintf(tStamp, "%02d:%02d", hour(),minute());
    alphaYEL.writeDigitAscii( 0, ( tempR / 10 ) + 48 );
    alphaYEL.writeDigitAscii( 1, ( tempR % 10 ) + 48 );
    alphaYEL.writeDigitAscii( 2, 'F');
    alphaYEL.writeDigitAscii( 3, ' ');    
    alphaYEL.writeDisplay();
    matrix.print(trend);
    matrix.writeDisplay();
    return true;
}

void printWifiStatus() {
    Serial.print("SSID: "); Serial.println(WiFi.SSID());
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: "); Serial.println(ip);
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):"); Serial.print(rssi); Serial.println(" dBm"); 
}

time_t prevDisplay = 0;

time_t getNtpTime() {
    while (Udp.parsePacket() > 0);
    sendNTPpacket(timeServer);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
        int size = Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
            Udp.read(packetBuffer, NTP_PACKET_SIZE);
            unsigned long secsSince1900;
            secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
        }
    }
    return 0;
}

void sendNTPpacket(IPAddress &address) {
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0]   = 0b11100011;
    packetBuffer[1]   = 0;
    packetBuffer[2]   = 6;
    packetBuffer[3]   = 0xEC;
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;              
    Udp.beginPacket(address, 123);
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}
