#include <SPI.h>
#include "U8g2lib.h"    // https://github.com/olikraus/u8g2
#include "RTCZero.h"


#define RST   9   // grn
#define CS    6   // org
#define A0    5   // wht

U8G2_ST7565_NHD_C12864_1_4W_HW_SPI lcd(U8G2_R2, CS, A0, RST);

RTCZero rtc;

void setup() {
  lcd.begin(); 
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
  lcd.firstPage();
  do {
    lcd.setFontMode(1); 
    lcd.setDrawColor(1);
    lcd.setFont(u8g2_font_fur20_tf);
    lcd.drawStr(0,22,"Bob");
    lcd.drawStr(0,47,"Smith");
    lcd.setFont(u8g2_font_crox1h_tf);
    lcd.drawStr(0,60,"Managing Director");
    lcd.setFont(u8g2_font_6x12_me);
    lcd.drawStr(99,63,curtime);
    lcd.drawFrame(83, 0, 45, 17 );    
    lcd.drawBox(83, 17, 45, 30);
    
    lcd.setFont(u8g2_font_crox2h_tf);
    lcd.setFontMode(1);
    lcd.setDrawColor(0);
    lcd.setFont(u8g2_font_6x12_me);
    lcd.drawStr(85, 37, "Bethany");
    lcd.setFont(u8g2_font_5x8_mr);
    lcd.drawStr(85, 45, "Sciences");

    lcd.drawEllipse( 105 , 18,  23, 10 , U8G2_DRAW_LOWER_RIGHT | U8G2_DRAW_LOWER_LEFT);
    lcd.drawEllipse( 120 , 15,  23, 10 , U8G2_DRAW_LOWER_LEFT);
    lcd.drawEllipse( 105 , 28,  23, 10 , U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);  
    
  } while ( lcd.nextPage() );
  delay(1000);
}


