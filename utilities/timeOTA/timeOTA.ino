#include <WiFi101OTA.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>
#include <SimpleTimer.h>

char ssid[] = "iotworld";             // your network SSID (name)
char pass[] = "iotworld";             // your network password
int status = WL_IDLE_STATUS;
WiFiClient client;

RTCZero rtc;
int tzOffset = -4;                    // Time zone -4 EDT
#define TIME_24_HOUR      false

SimpleTimer timer;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial);
//  WiFi.setPins(8,7,4,2);          // for Adafruit Feather M0 WiFi
  Serial.println("START");

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    Serial.print(">");
    delay(500);
  }
  Serial.println();
  WiFiOTA.begin("Arduino", "password", InternalStorage);
  digitalWrite(LED_BUILTIN, HIGH);
  
  rtc.begin();
  unsigned long epoch;
  Serial.print("post epoch ");   Serial.println(epoch);
  while (epoch == 0) {
    epoch = WiFi.getTime();
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("post epoch ");   Serial.println(epoch);
  rtc.setEpoch(epoch);
  
  statusLine();
  timer.setInterval(10000, statusLine);
}

void loop() {
  WiFiOTA.poll();
  timer.run();
}

void statusLine() {
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
  
  Serial.print("StatusLine: ssid ");
  char statusline[80];
  IPAddress ip = WiFi.localIP();
  sprintf(statusline, "%s %d.%d.%d.%d %ddBm  set %02d/%02d/%02d %d:%02d:%02d%s EDT",
                            WiFi.SSID(),
                            ip[0], ip[1], ip[2], ip[3],
                            WiFi.RSSI(),
                            rtc.getMonth(),
                            tday,
                            rtc.getYear(),
                            thours,
                            rtc.getMinutes(),
                            rtc.getSeconds(),
                            ampm);
  Serial.println(statusline);
}

