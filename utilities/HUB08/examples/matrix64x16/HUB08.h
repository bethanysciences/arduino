#ifndef _HUB08_H_
#define _HUB08_H_

#if ARDUINO >= 100
    #include "Arduino.h"
    #define WIRE_WRITE Wire.write
#else
    #include "WProgram.h"
    #define WIRE_WRITE Wire.send
#endif

#if defined(__SAM3X8E__)
    typedef volatile RwReg PortReg;
    typedef uint32_t PortMask;
 #define HAVE_PORTREG
#elif defined(ARDUINO_ARCH_SAMD)
// not supported
#elif defined(ESP8266) || defined(ARDUINO_STM32_FEATHER)
    typedef volatile uint32_t PortReg;
    typedef uint32_t PortMask;
#else
    typedef volatile uint8_t PortReg;
    typedef uint8_t PortMask;
 #define HAVE_PORTREG
#endif

#include <SPI.h>
#include <Adafruit_GFX.h>

#define BLACK 0
#define WHITE 1
#define INVERSE 0
#define HUB08_LCDWIDTH    64
#define HUB08_LCDHEIGHT   16

class HUB08 : public Adafruit_GFX {
 public:
  HUB08(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, uint8_t r1, uint8_t stb, uint8_t clk);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void begin();
  void on();
  void off();
  void scan();
  void clearDisplay();
 private:
  int8_t _i2caddr, _vccstate, sid, sclk, dc, rst, cs;
  uint8_t clk, r1, stb, oe, a, b, c, d;
  uint8_t  mask;
  uint8_t  state;
};
#endif /* _HUB08_H_ */

