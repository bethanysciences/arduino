/*----------------------------------------------------------------------------------*
  Weather and Information Console
    WiFi.setPins(8,7,4,2)       for Adafruit Feather M0 WiFi
    WiFi credentials updated in file /library/WiFiCreds.h
  For avr-libc ATMEL series 32bit SAMD21 CPUs e.g. Arduino MKR1000
  © 2018 Bob Smith https://github.com/bethanysciences/console
  MIT license
 *---------------------------------------------------------------------------------*/
#include "SimpleTimer.h"       // playground.arduino.cc/Code/SimpleTimer
#include "WiFi101.h"           // www.arduino.cc/en/Reference/WiFi101
#include "ArduinoHttpClient.h" // github.com/arduino-libraries/ArduinoHttpClient
#include "WiFiCreds.h"         // WiFi credentials
#include "xmlTakeParam.h"      // parse XML elements

int status  = WL_IDLE_STATUS;

struct METARstruct{
  String server;
     int port;
  String station;
  String dataSource;
  String requestType;
  String format;
     int hours;
  String protocol;
  String request;
  String code;
  String len;
  String timetakenms;
  String response;
  String text;
  String obstimeUTC;
  String lat;
  String lon;
     int tempC;
     int tempF;
     int dewpointC;
     int dewpointF;
     int winddirDeg;
     int windspeedKTS;
     int visibilitySM;
  String altSettingHG;
  String flightcategory;
  String metartype;
  String elevation_m;
     int year;
     int month;
     int date;
     int hour;
     int minute;
};

void setup() {
    Serial.begin(115200);
    while (!Serial);
    WiFi.setPins(8,7,4,2);                       // for Adafruit Feather M0 WiFi
    Serial.print("start >> ");
    while (status != WL_CONNECTED) { 
        status = WiFi.begin(ssid, pass);
        Serial.print("connecting");
    }
    IPAddress ip = WiFi.localIP();
    char wifistat[80];
    sprintf(wifistat, "connected %s with %d.%d.%d.%d %ddBm strength",
            WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI());
    Serial.println(wifistat);
  
    METARstruct METAR;
    METAR.server         = "www.aviationweather.gov";
    METAR.port           = 443;
    METAR.station        = "KDEN";
    METAR.dataSource     = "metars";
    METAR.requestType    = "retrieve";
    METAR.format         = "xml";
    METAR.hours          = 1;
    METAR.protocol       = "HTTP/1.0";
  
    GetMETAR(METAR);

    Serial.print("request          ");   Serial.println(METAR.request); 
    Serial.print("code             ");   Serial.println(METAR.code); 
    Serial.print("length           ");   Serial.println(METAR.len);
    Serial.print("transaction time ");   Serial.println(METAR.timetakenms);
//  Serial.print("body             ");   Serial.println(METAR.response);
    Serial.print("text             ");   Serial.println(METAR.text);
    Serial.print("obstime UTC      ");   Serial.println(METAR.obstimeUTC);
    Serial.print("temp C           ");   Serial.println(METAR.tempC);
    Serial.print("dewpoint C       ");   Serial.println(METAR.dewpointC);
    Serial.print("wind dir Deg     ");   Serial.println(METAR.winddirDeg);
    Serial.print("windspeed KTS    ");   Serial.println(METAR.windspeedKTS);
    Serial.print("visibility SM    ");   Serial.println(METAR.visibilitySM);
    Serial.print("alt Setting      ");   Serial.println(METAR.altSettingHG);
    Serial.print("flight category  ");   Serial.println(METAR.flightcategory);
    Serial.print("metar type       ");   Serial.println(METAR.metartype);
    Serial.print("elevation_m      ");   Serial.println(METAR.elevation_m);
}

void loop() { }

void GetMETAR(METARstruct &m) {
    WiFiSSLClient wifi;
    HttpClient NOAA_metar = HttpClient(wifi, m.server, m.port);
    m.request = "/adds/dataserver_current/httpparam?dataSource=" + 
                                        m.dataSource  +
                   "&requestType="    + m.requestType +
                   "&format="         + m.format +
                   "&stationString="  + m.station +
                   "&hoursBeforeNow=" + m.hours;
    NOAA_metar.get(m.request);
    m.code            = NOAA_metar.responseStatusCode();
    m.len             = NOAA_metar.contentLength();
    m.response        = NOAA_metar.responseBody();
    m.timetakenms     = xmlTakeParam(m.response, "time_taken_ms");
    m.text            = xmlTakeParam(m.response, "raw_text");
    m.station         = xmlTakeParam(m.response, "station_id");
    m.obstimeUTC      = xmlTakeParam(m.response, "observation_time");
    m.lat             = xmlTakeParam(m.response, "latitude");
    m.lon             = xmlTakeParam(m.response, "longitude");
    m.tempC           = xmlTakeParam(m.response, "temp_c").toInt();
    m.dewpointC       = xmlTakeParam(m.response, "dewpoint_c").toInt();
    m.winddirDeg      = xmlTakeParam(m.response, "wind_dir_degrees").toInt();
    m.windspeedKTS    = xmlTakeParam(m.response, "wind_speed_kt").toInt();
    m.visibilitySM    = xmlTakeParam(m.response, "visibility_statute_mi").toInt();
    m.altSettingHG    = xmlTakeParam(m.response, "altim_in_hg").toDouble();
    m.flightcategory  = xmlTakeParam(m.response, "flight_category");
    m.metartype       = xmlTakeParam(m.response, "metar_type");
    m.elevation_m     = xmlTakeParam(m.response, "elevation_m");
}
