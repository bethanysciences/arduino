/*----------------------------------------------------------------------*
  METARGet - gets Weather METAR strings from aviationweather.gov
  for avr-libc ATMEL series 32bit SAMD21 CPUs
  
  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/
#include <ArduinoHttpClient.h>
int port              = 80;
String server         = "aviationweather.gov";
String product        = "/adds/dataserver_current/";
String dataSource     = "metars";
String requestType    = "retrieve";
String format         = "xml";
String hoursBeforeNow = "1";
String request        = "";
String resp           = "";
WiFiClient wifi;
HttpClient xml = HttpClient(wifi, server, port);
void metarGet(String stat, String *resp, int *code, int *len) {
  String request = product + "httpparam?" + 
                   "dataSource=" + dataSource + "&" +
                   "requestType=" + requestType + "&" + 
                   "format=" + format + "&" + 
                   "hoursBeforeNow=" + hoursBeforeNow + "&" +  
                   "stationString=" + stat;
  xml.get(request);
  
  *code = xml.responseStatusCode();
  *len = xml.contentLength();
  *resp = xml.responseBody();
}
