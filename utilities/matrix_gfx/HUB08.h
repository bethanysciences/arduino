#ifndef _HUB08_H_
#define _HUB08_H_

#include "Arduino.h"
#include <SPI.h>
#include <Adafruit_GFX.h>

#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define HUB08_LCDWIDTH    64
#define HUB08_LCDHEIGHT   16

class HUB08 : public Adafruit_GFX {
 public:
  HUB08(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, uint8_t r1, uint8_t stb, uint8_t clk);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void begin();
  void scan();
  void clearDisplay();

 private:
  uint8_t clk, r1, stb, oe, a, b, c, d;
  uint8_t  mask;
  uint8_t  state;
};

#endif /* _HUB08_H_ */
