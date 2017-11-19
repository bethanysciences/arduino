#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

#define CS    10
#define RST   9
#define DC    8
#define DTA   7
#define CLK   6

// https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_ST7565_NHD_C12864_F_4W_SW_SPI u8g2(U8G2_R0, CLK, DTA, CS, DC, RST);
//U8G2_ST7565_NHD_C12864_F_4W_HW_SPI u8g2(U8G2_R0, CS, DC, RST);

void setup() {
  u8g2.begin();
}

void loop(void) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB18_tr);
  u8g2.drawStr(3,20,"Bob Smith");
  u8g2.setFont(u8g2_font_helvR12_tr);
  u8g2.drawStr(0,40,"Bethany");
  u8g2.drawStr(64,40,"Sciences");
  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawStr(0,60,"github.com/bethanysciences");
  u8g2.sendBuffer();
  delay(1000);  
}

