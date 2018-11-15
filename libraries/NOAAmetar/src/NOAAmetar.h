/*----------------------------------------------------------------------*
  METARGet - gets Weather METAR strings from aviationweather.gov
      https://www.aviationweather.gov/metar
  for avr-libc ATMEL series 32bit SAMD21 CPUs
  
  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/
#include <ArduinoHttpClient.h>
#include "xmlTakeParam.h"                 // parse XML elements

struct c_rent {
  String code;
  String len;
  String timetakenms;
  String response;
  String text;
  String station;
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
    bool pm;  
};
typedef struct c_rent Curr;
Curr metar;

// void metarGet(String station, String *resp, int *code, int *len) {
void NOAA_metar(String station, String *resp, int *code, int *len) {
  char METARserver[]        = "www.aviationweather.gov";
  int SSLport               = 443;
  char station[]            = "KDEN";
  char METARdataSource[]    = "metars";
  char METARrequestType[]   = "retrieve";
  char METARformat[]        = "xml";
  int METARhours            = 1;
  char METARprotocol[]      = "HTTP/1.0";
  String currServer = "aviationweather.gov";
  WiFiSSLClient wifi;
  HttpClient NOAA_metar = HttpClient(wifi, METARserver, SSLport);

  String request = product + "httpparam?" + 
                   "dataSource=" + dataSource + "&" +
                   "requestType=" + requestType + "&" + 
                   "format=" + format + "&" + 
                   "hoursBeforeNow=" + hoursBeforeNow + "&" +  
                   "stationString=" + stat;
  xml.get(request);
  
  *metar.code = xml.responseStatusCode();
  *metar.len  = xml.contentLength();
  *metar.resp = xml.responseBody();

  void NOAAmetar() {
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