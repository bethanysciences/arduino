/*------------------------------------------------------------------------------------*
  Get and parse NOAA METAR www.aviationweather.gov
  For avr-libc ATMEL series 32bit SAMD21 CPUs e.g. Arduino MKR1000
  Returns as XML 
  Revision SEP18 convert to SSL transaction as required

  ©2017-2018 Bob Smith https://github.com/bethanysciences/getwx
  MIT license
 *-----------------------------------------------------------------------------------*/
#include "ArduinoHttpClient.h"  // github.com/arduino-libraries/ArduinoHttpClient
#include "xmlTakeParam.h"       // parse XML elements
                                // xmlTakeParam(String inStr, String needParam)
                                // input string       e.g. <temp_c>30.6</temp_c>
                                // input needParam    parameter e.g. temp_c
                                // returns value in string

//  #define DEBUG_METAR             // used for serial debugging

struct METARstruct{             // data variable struture
const char* station;            // supply with call
        int code;
        int len;
     String timetakenms;
     String response;
     String text;
  String obstimeUTC;
  String lat;
  String lon;
  String tempC;
  String dewpointC;
  String winddirDeg;
  String windspeedKTS;
  String visibilitySM;
  String altim_in_hg;
  String sea_level_pressure_mb;
  String flightcategory;
  String metartype;
  String elevation_m;
     int myear;
     int mmonth;
     int mdate;
     int mhour;
     int mminute; 
};
METARstruct METAR;

void GetMETAR(METARstruct &m) {
    String metarServer  ="www.aviationweather.gov"; // static URL
       int metarPort    = 443;
    String dataSource   = "metars";                 // specifies METAR string
    String requestType  = "retrieve";
    String format       = "xml";
    String hours        = "1";                      // Hours of strings to return
    String protocol     = "HTTP/1.0";
    WiFiSSLClient metarClient;
    HttpClient NOAA_metar = HttpClient(metarClient, metarServer, metarPort);
    String metarrequest = "/adds/dataserver_current/httpparam?dataSource=" + 
                                        dataSource  +
                   "&requestType="    + requestType +
                   "&format="         + format +
                   "&stationString="  + m.station +
                   "&hoursBeforeNow=" + hours;

    NOAA_metar.get(metarrequest);

    m.code             = NOAA_metar.responseStatusCode();
    m.len              = NOAA_metar.contentLength();
    m.response         = NOAA_metar.responseBody();
    m.timetakenms      = xmlTakeParam(m.response, "time_taken_ms");
    m.text             = xmlTakeParam(m.response, "raw_text");
    m.obstimeUTC       = xmlTakeParam(m.response, "observation_time");
    m.lat              = xmlTakeParam(m.response, "latitude");
    m.lon              = xmlTakeParam(m.response, "longitude");
    m.tempC            = xmlTakeParam(m.response, "temp_c");
    m.dewpointC        = xmlTakeParam(m.response, "dewpoint_c");
    m.winddirDeg       = xmlTakeParam(m.response, "wind_dir_degrees");
    m.windspeedKTS     = xmlTakeParam(m.response, "wind_speed_kt");
    m.visibilitySM     = xmlTakeParam(m.response, "visibility_statute_mi");
    m.altim_in_hg      = xmlTakeParam(m.response, "altim_in_hg");
    m.sea_level_pressure_mb  = xmlTakeParam(m.response, "sea_level_pressure_mb");
    m.flightcategory   = xmlTakeParam(m.response, "flight_category");
    m.metartype        = xmlTakeParam(m.response, "metar_type");
    m.elevation_m      = xmlTakeParam(m.response, "elevation_m");
    int firstDash      = m.obstimeUTC.indexOf("-");
    int secondDash     = m.obstimeUTC.lastIndexOf("-");
    int firstT         = m.obstimeUTC.indexOf("T");
    int firstColon     = m.obstimeUTC.indexOf(":");
    int secondColon    = m.obstimeUTC.lastIndexOf(":");
    String yearStr     = m.obstimeUTC.substring(0, firstDash);
    String monthStr    = m.obstimeUTC.substring(firstDash + 1);
    String dateStr     = m.obstimeUTC.substring(secondDash + 1);
    String hourStr     = m.obstimeUTC.substring(firstT + 1);
    String minuteStr   = m.obstimeUTC.substring(firstColon + 1);
    m.myear            = yearStr.toInt();
    m.mmonth           = monthStr.toInt();
    m.mdate            = dateStr.toInt();
    m.mhour            = hourStr.toInt();
    m.mminute          = minuteStr.toInt();
    
    #ifdef DEBUG_METAR
        char Mstamp[200];
        sprintf(Mstamp,"NOAA METAR Station %s at %d/%d %2d%2d reports %d°F curr temp", 
               METAR.station, METAR.mmonth, METAR.mdate, METAR.mhour, METAR.mminute,
               METAR.tempC.toInt());
        Serial.println(Mstamp);
        char mstamp[50]; 
        sprintf(mstamp, "observation time:  %d/%d/%d %02d:%02dZ",  
                                 m.mmonth, m.mdate, m.myear, m.mhour, m.mminute);
        Serial.println(mstamp);
        Serial.print("code        "); Serial.println(m.code); 
        Serial.print("length      "); Serial.println(m.len);
        Serial.print("get time ms "); Serial.println(m.timetakenms);
        Serial.print("timestamp Z "); Serial.println(m.obstimeUTC);
//      Serial.print("Body      "); Serial.println(m.response);
//      Serial.print("text        "); Serial.println(m.text);
        Serial.print("temp C°     "); Serial.println(m.tempC);
        Serial.print("dewpt C°    "); Serial.println(m.dewpointC);
        Serial.print("wind dir    "); Serial.println(m.winddirDeg);
        Serial.print("wind KTS    "); Serial.println(m.windspeedKTS);
        Serial.print("visi SM     "); Serial.println(m.visibilitySM);
        Serial.print("alt set     "); Serial.println(m.altim_in_hg.substring(0,5));
        Serial.print("press SL    "); Serial.println(m.sea_level_pressure_mb);     
        Serial.print("flight cat  "); Serial.println(m.flightcategory);
        Serial.print("type        "); Serial.println(m.metartype);
        Serial.print("elevation M "); Serial.println(m.elevation_m);
    #endif
}
