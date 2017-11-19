#include <SPI.h>
#include "Ucglib.h"

#define TFT_CS   9
#define TFT_DC   10

Ucglib_ILI9341_18x240x320_HWSPI ucg(TFT_DC, TFT_CS, 8);

void setup() {
  delay(1000);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  //ucg.begin(UCG_FONT_MODE_SOLID);
  ucg.clearScreen();
}

void loop(void) {
  ucg.setRotate90();
  ucg.setFont(ucg_font_fur49_tn);
  ucg.setColor(255, 255, 255);
  //ucg.setColor(0, 255, 0);
  ucg.setColor(1, 255, 0,0);
  ucg.setPrintPos(0,60);
  ucg.print("Bob 1234");
  ucg.setPrintPos(0,120);
  ucg.print("Smith");
  delay(500);  
}
