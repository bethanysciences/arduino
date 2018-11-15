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
const char* CONNSTAT[2] = {"DISC", "CONN"};      // Connection preferred lables

char METARserver[]        = "www.aviationweather.gov";
int SSLport               = 443;
char METARstation[]       = "KDEN";
char METARdataSource[]    = "metars";
char METARrequestType[]   = "retrieve";
char METARformat[]        = "xml";
int METARhours            = 1;
char METARprotocol[]      = "HTTP/1.0";
int currPort   = 443;
String currServer = "aviationweather.gov";
WiFiSSLClient wifi;
HttpClient NOAA_metar = HttpClient(wifi, METARserver, SSLport);

typedef struct {
  String server;
     int port;
  String station;
  String dataSource;
  String requestType;
  String format;
     int hours;
  String protocol;
     int currPort;
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
} METARstruct;
METARstruct METAR;

void setup() {
  Serial.begin(115200);   while (!Serial);
  Serial.print("start >> ");
  while (status != WL_CONNECTED) status = WiFi.begin(ssid, pass);
  IPAddress ip = WiFi.localIP();
  char wifistat[80];
  sprintf(wifistat, "connected %s with %d.%d.%d.%d %ddBm strength",
          WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI());
  Serial.println(wifistat);
  METAR.server         = "www.aviationweather.gov";
  METAR.port           = 443;
  METAR.station        = "KDEN";
  METAR.dataSource     = "metars";
  METAR.requestType    = "retrieve";
  METAR.format         = "xml";
  METAR.hours           = 1;
  METAR.protocol       = "HTTP/1.0";
  GetMETAR(&METAR);
}


void loop() { }

void GetMETAR(METAR *m) {
  String req0 = "/adds/dataserver_current/httpparam?";
  String req1 = "dataSource=metars&requestType=retrieve&";
  String req2 = "format=xml&hoursBeforeNow=1&stationString=KDEN";
  String request = req0 + req1 + req2;
  NOAA_metar.get(request);

  metar.code            = NOAA_metar.responseStatusCode();
  metar.len             = NOAA_metar.contentLength();
  metar.response        = NOAA_metar.responseBody();
  metar.timetakenms     = xmlTakeParam(metar.response, "time_taken_ms");
  metar.text            = xmlTakeParam(metar.response, "raw_text");
  metar.station         = xmlTakeParam(metar.response, "station_id");
  metar.obstimeUTC      = xmlTakeParam(metar.response, "observation_time");
  metar.lat             = xmlTakeParam(metar.response, "latitude");
  metar.lon             = xmlTakeParam(metar.response, "longitude");
  metar.tempC           = xmlTakeParam(metar.response, "temp_c").toInt();
  metar.dewpointC       = xmlTakeParam(metar.response, "dewpoint_c").toInt();
  metar.winddirDeg      = xmlTakeParam(metar.response, "wind_dir_degrees").toInt();
  metar.windspeedKTS    = xmlTakeParam(metar.response, "wind_speed_kt").toInt();
  metar.visibilitySM    = xmlTakeParam(metar.response, "visibility_statute_mi").toInt();
  metar.altSettingHG    = xmlTakeParam(metar.response, "altim_in_hg").toDouble();
  metar.flightcategory  = xmlTakeParam(metar.response, "flight_category");
  metar.metartype       = xmlTakeParam(metar.response, "metar_type");
  metar.elevation_m     = xmlTakeParam(metar.response, "elevation_m");
  
  Serial.print("Code            ");   Serial.println(metar.code); 
  Serial.print("Length          ");   Serial.println(metar.len);
  Serial.print("Tranaction time ");   Serial.println(metar.timetakenms);
  // Serial.print("Body            ");   Serial.println(metar.response);
  // Serial.print("text            ");   Serial.println(metar.text);
  Serial.print("obstime UTC     ");   Serial.println(metar.obstimeUTC);
  Serial.print("temp C          ");   Serial.println(metar.tempC);
  Serial.print("dewpoint C      ");   Serial.println(metar.dewpointC);
  Serial.print("wind dir Deg    ");   Serial.println(metar.winddirDeg);
  Serial.print("windspeed KTS   ");   Serial.println(metar.windspeedKTS);
  Serial.print("visibility SM   ");   Serial.println(metar.visibilitySM);
  Serial.print("alt Setting     ");   Serial.println(metar.altSettingHG);
  Serial.print("flight category ");   Serial.println(metar.flightcategory);
  Serial.print("metar type      ");   Serial.println(metar.metartype);
  Serial.print("elevation_m     ");   Serial.println(metar.elevation_m);
}
