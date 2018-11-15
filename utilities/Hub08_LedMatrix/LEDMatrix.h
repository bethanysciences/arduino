#ifndef __LED_MATRIX_H__
#define __LED_MATRIX_H__
#include <stdint.h>
#include <SPI.h>
class LEDMatrix {
public:
    LEDMatrix();
    void begin(uint8_t *displaybuf, uint16_t width, uint16_t height);
    void drawPoint(uint16_t x, uint16_t y, uint8_t pixel);
    void drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t pixel);
    void clear();                   // Set screen buffer to zero
    void scan();                    // turn off 1/16 leds and turn on another 1/16 leds
    void swap();
    uint8_t *offscreen_buffer();
    void reverse();                 // reverse screen call
    uint8_t isReversed();
    void on();
    void off();

private:
    uint8_t clk, r1, stb, oe, a, b, c, d;
    uint8_t *displaybuf;
    uint16_t width;
    uint16_t height;
    uint8_t  mask;
    uint8_t  state;
    uint8_t  buffer;
};
#endif
