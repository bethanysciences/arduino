#include "ArduinoJson.h"
#include "WiFi101.h"
#include "RTCZero.h"
#include "SimpleTimer.h"
char ssid[] = "FWI-Guest";             // your network SSID (name)
char pass[] = "V1sualBr@nd";             // your network password
int status = WL_IDLE_STATUS;
WiFiClient client;

RTCZero rtc;
int tzOffset = -7;                    // Time zone -4 EDT

SimpleTimer timer;

void setup() {
  Serial.begin(115200);
  WiFi.setPins(8,7,4,2);                          // for Adafruit Feather M0 WiFi
  while (!Serial);
  Serial.println("START");
  

  while ( status != WL_CONNECTED) {
    Serial.print("connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }

  digitalWrite(LED_BUILTIN, HIGH);
  rtc.begin();
  unsigned long epoch;
  Serial.print("pre epoch ");   Serial.println(epoch);
  while (epoch == 0) {
    epoch = WiFi.getTime();
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("post epoch ");   Serial.println(epoch);

  rtc.setEpoch(epoch);
  statusLine();
  displayTime();
  fetchCurrent();
  // timer.setInterval(60000, fetchCurrent());
  // timer.setInterval(60000, fetchForecast());
  // timer.setInterval(10000, displayTime);
  // timer.setInterval(10000, statusLine);
}

void loop() {
  timer.run();
}

void displayTime() {
  Serial.print("DisplayTime ");
  int thours = rtc.getHours();
  int tday = rtc.getDay();
  const char* ampm = "am";
  thours = thours + tzOffset;
  if (thours < 0) { thours += 24; tday -= 1; }
  else if (thours > 23) { thours -= 24; tday -= 1; }
  if (thours >= 12) ampm = "pm";
  if (!TIME_24_HOUR) {
    if (thours > 12) thours -= 12;
    else if (thours == 0) thours += 12;
  }
  char DATE[10];
  char TIME[15];
  sprintf(DATE, "%02d/%02d/%02d", rtc.getMonth(), tday, rtc.getYear());
  sprintf(TIME, "%d:%02d:%02d%s", thours, rtc.getMinutes(), rtc.getSeconds(), ampm);

  Serial.print(DATE);   Serial.print("  ");
  Serial.println(TIME);
}

void fetchCurrent() {
  const size_t respBuf[] = 2000;
  #define host "https://api.weather.gov/stations/KPDK/observations/current"
  #define key  "712a91952a8b182e"
  #define type  "xxx"
  #define loc  "PDK"
  client.stop();
  delay(100);
  if (!client.connect(host, 80)) { return; }
  client.print("GET /api/");
  client.print(key);
  client.print("/");
  client.print(type);
  client.print("/q/");
  client.print(loc);
  client.println(".json HTTP/1.1");
  client.print("Host: "); 
  client.println(host);
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
      if (bytesIn > 0) {  
        respLen += bytesIn; 
        if (respLen > sizeof(respBuf)) respLen = sizeof(respBuf); 
      }
      else if (bytesIn < 0) { }
    }
      delay(1);
  }
  client.stop();
  if (respLen >= sizeof(respBuf)) { return; }
  respBuf[respLen++] = '\0';


  const size_t bufferSize = 2*JSON_ARRAY_SIZE(1) +
                          2*JSON_ARRAY_SIZE(2) +
                          6*JSON_OBJECT_SIZE(1) +
                          7*JSON_OBJECT_SIZE(2) +
                          16*JSON_OBJECT_SIZE(3) +
                          JSON_OBJECT_SIZE(4) +
                          JSON_OBJECT_SIZE(5) +
                          JSON_OBJECT_SIZE(16) +
                          JSON_OBJECT_SIZE(26) +
                          2680;

  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(client);

  Serial.print("root ");   Serial.println(bufferSize);

  JsonObject& properties = root["properties"];
  const char* properties_timestamp = properties["timestamp"];
  const char* properties_textDescription = properties["textDescription"];
  const char* properties_icon = properties["icon"];

  JsonObject& properties_temperature = properties["temperature"];
  float properties_temperature_value = properties_temperature["value"];

  JsonObject& properties_dewpoint = properties["dewpoint"];
  float properties_dewpoint_value = properties_dewpoint["value"];

  JsonObject& properties_windDirection = properties["windDirection"];
  int properties_windDirection_value = properties_windDirection["value"];

  JsonObject& properties_windSpeed = properties["windSpeed"];
  float properties_windSpeed_value = properties_windSpeed["value"];

  JsonObject& properties_barometricPressure = properties["barometricPressure"];
  long properties_barometricPressure_value = properties_barometricPressure["value"];

  JsonObject& properties_seaLevelPressure = properties["seaLevelPressure"];
  long properties_seaLevelPressure_value = properties_seaLevelPressure["value"];

  JsonObject& properties_relativeHumidity = properties["relativeHumidity"];
  float properties_relativeHumidity_value = properties_relativeHumidity["value"];

  JsonObject& properties_windChill = properties["windChill"];
  float properties_windChill_value = properties_windChill["value"];

  JsonObject& properties_heatIndex = properties["heatIndex"];
  float properties_heatIndex_value = properties_heatIndex["value"];

  Serial.print("time ");              Serial.println(properties_timestamp);
  Serial.print("cond ");              Serial.println(properties_textDescription);
  Serial.print("icon url ");          Serial.println(properties_icon);
  Serial.print("temp ");              Serial.print(properties_temperature_value,1);
  Serial.print(" | dew ");            Serial.println(properties_dewpoint_value,1);
  Serial.print("wind ");              Serial.print(properties_windDirection_value);
  Serial.print(" deg @ ");            Serial.print(properties_windSpeed_value,0);
  Serial.println(" mph");
  Serial.print("baro ");              Serial.print(properties_barometricPressure_value);
  Serial.print(" mb | set ");         Serial.print(properties_seaLevelPressure_value);
  Serial.println(" mb");
  Serial.print("rel um ");            Serial.print(properties_relativeHumidity_value);
  Serial.print(" % | wind chill ");   Serial.print(properties_windChill_value);
  Serial.print(" | heat idx ");       Serial.println(properties_heatIndex_value);
}

void fetchForecast() {
  Serial.println("fetchForecast");
}

void statusLine() {
  Serial.print("StatusLine: ssid ");
  char statusline[80];
  IPAddress ip = WiFi.localIP();
  sprintf(statusline, "%s %d.%d.%d.%d %ddBm  set %02d/%02d/%02d %02d:%02d:%02d UTC",
                            WiFi.SSID(),
                            ip[0], ip[1], ip[2], ip[3],
                            WiFi.RSSI(),
                            rtc.getMonth(),
                            rtc.getDay(),
                            rtc.getYear(),
                            rtc.getHours(),
                            rtc.getMinutes(),
                            rtc.getSeconds());
// tft.setCursor(10,232);
// tft.setTextSize(1);
// tft.setTextColor(ILI9341_BLACK, ILI9341_GREEN);
// tft.print(statusline);
  Serial.println(statusline);
}
