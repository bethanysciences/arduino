#include <Wire.h>
#include <SPI.h>
#include "RTClib.h"

byte clockPin =  8;         // yel
byte dataPin  =  9;         // blu
byte dimmPin  = 10;         // wht

RTC_DS3231 rtc;

uint8_t dec_to_bcd(int val) {return (uint8_t) ((val / 10 * 16) + (val % 10));}
byte digit[11] = {0x7e,0x0c,0xb6,0x9e,0xcc,0xda,0xfa,0x0e,0xfe,0xde,0x00};

void setup () {
  Wire.begin();
  rtc.begin();
  if (rtc.lostPower()) { rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));}
  DateTime now = rtc.now();
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dimmPin, OUTPUT);
}

void loop () {
  DateTime now = rtc.now();
  digitalWrite(dimmPin, HIGH);
  shiftOut(dataPin, clockPin, MSBFIRST, digit[now.second()%10]);
  shiftOut(dataPin, clockPin, MSBFIRST, digit[now.second()/10]);
  shiftOut(dataPin, clockPin, MSBFIRST, digit[now.minute()%10]);
  shiftOut(dataPin, clockPin, MSBFIRST, digit[now.minute()/10]);
  shiftOut(dataPin, clockPin, MSBFIRST, digit[now.hour()%10]);
  shiftOut(dataPin, clockPin, MSBFIRST, digit[now.hour()/10]);
  digitalWrite(dimmPin, LOW);
  delay(1000);
}

