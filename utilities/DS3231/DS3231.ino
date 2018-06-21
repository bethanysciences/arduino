#include <Wire.h>
#define DS3231_I2C_ADDR     0x68    // RTC I2C address
#define keyPL               2
#define SetMinute           58 
#define SetHour             8

byte decToBcd(byte val) { return ( (val/10*16) + (val%10) ); }
byte bcdToDec(byte val) { return ( (val/16*10) + (val%16) ); }

void setup() {
	Serial.begin(115200);
    Serial.println(">>>> Start");
  	pinMode(SetMinute, INPUT_PULLUP);
  	pinMode(SetHour, INPUT_PULLUP);
  	pinMode(keyPL, INPUT_PULLUP);
    Serial.println(">>>> Loop");
}
   

void loop() {
  	byte second, minute, hour, day, date, month, year; 
  	getDate(&second, &minute, &hour, &day, &date, &month, &year); 

    if (hour == 0) hour = 12;
    if (hour > 12) hour = hour - 12;
    
    char times[40]; 
    sprintf(times, " MCU %02d:%02d:%02d", hour, minute, second);
    Serial.println(times);

	if (!digitalRead(SetHour) && !digitalRead(keyPL)) {
      	second = 0;
      	hour++;
      	if (hour > 23) hour = 0;
      	setDate(second, minute, hour, day, date, month, year);
      	delay(200);
  	}
  	if (!digitalRead(SetMinute) && !digitalRead(keyPL)) {
      	second = 0;
      	minute++;
      	if (minute > 59) minute = 0;
      	setDate(second, minute, hour, day, date, month, year);
      	delay(200);
  	}
}

void setDate(byte second, byte minute, byte hour, byte day, 
             byte date, byte month, byte year) {
    Wire.beginTransmission(DS3231_I2C_ADDR);
    Wire.write(0);
    Wire.write(decToBcd(second));
    Wire.write(decToBcd(minute));
    Wire.write(decToBcd(hour));
    Wire.write(decToBcd(day));
    Wire.write(decToBcd(date));
    Wire.write(decToBcd(month));
    Wire.write(decToBcd(year));
    Wire.endTransmission();
}

void getDate(byte *second, byte *minute, byte *hour, byte *day,
             byte *date, byte *month, byte *year) {
    Wire.beginTransmission(DS3231_I2C_ADDR);
    Wire.write(0);
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDR, 7);
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour   = bcdToDec(Wire.read() & 0x3f); 
    *day    = bcdToDec(Wire.read());
    *date   = bcdToDec(Wire.read());
    *month  = bcdToDec(Wire.read());
    *year   = bcdToDec(Wire.read());
}

