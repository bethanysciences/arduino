#include "RTCZero.h"

RTCZero rtc;

void setup() {
  rtc.begin();
  byte seconds, minutes, hours;
  byte days, months, years;
  int thour, tminute, tsecond;
  int tmonth, tday, tyear;
  char s_month[5];
  static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  sscanf(__DATE__, "%s %d %d", s_month, &tday, &tyear);
  sscanf(__TIME__, "%d:%d:%d", &thour, &tminute, &tsecond);
  tmonth = (strstr(month_names, s_month) - month_names) / 3;
  years = tyear - 2000;
  months =  tmonth + 1;
  days = tday;
  hours = thour;
  minutes = tminute;
  seconds = tsecond;
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(days, months, years);
}

void loop() {
  char curtime[80];
  sprintf(curtime, "%02d:%02d", rtc.getHours(), rtc.getMinutes());
  Serial.print(curtime);
  delay(1000);
}
