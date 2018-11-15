#include <RGBmatrixPanel.h>
#include "RTCZero.h"

#define CLK  8
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);
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
    matrix.begin();
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 0, 7));
    matrix.drawRect(1, 1, matrix.width()-2, matrix.height()-2, matrix.Color333(0, 7, 0));
    matrix.drawRect(2, 2, matrix.width()-4, matrix.height()-4, matrix.Color333(7, 0, 0));
    matrix.setTextSize(1);                                  // 1x8 pixels high
    matrix.setTextWrap(false);
    matrix.setTextColor(matrix.Color333(7,7,7),matrix.Color333(0,0,0));            // white
}

void loop() {
    char curDate[80];
    sprintf(curDate, "%02d/%02d/%02d", rtc.getMonth(), rtc.getDay(), rtc.getYear());
    char curTime[80];
    sprintf(curTime, "%02d:%02d:%02d", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
    delay(1000);
    matrix.setCursor(7, 8);
    matrix.println(curDate);
    matrix.setCursor(9, 19);
    matrix.println(curTime);
}

