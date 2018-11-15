#include "HUB08SPI.h"
#include "Arduino.h"
#include <TimerOne.h>
HUB08SPI::HUB08SPI() { }
void HUB08SPI::begin(uint8_t *displaybuf, uint16_t width, uint16_t height) {
    this->displaybuf = displaybuf;
    this->width = width;
    this->height = height;
    DDRD |= 0xFC;
    TCCR2B = TCCR2B & 0b11111000 | 0x01;                    // 32khz pwm on pin 3 & 11
    SPI.begin();                            
    analogWrite(3,128); 
}
void HUB08SPI::drawPoint(uint16_t x, uint16_t y, uint8_t color) {
    uint8_t *byte = displaybuf + x / 8 + y * width / 8;
    uint8_t  bit = x % 8;
    if (color) *byte |= 0x80 >> bit;
    else *byte &= ~(0x80 >> bit);
}
void HUB08SPI::drawRect(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint8_t color) {
    for (uint16_t x = x1; x < x1+w; x++) for (uint16_t y = y1; y < y1+h; y++) drawPoint(x, y, color);
}
void HUB08SPI::clear() {
    uint8_t *ptr = displaybuf;
    for (uint16_t i = 0; i < (width * height / 8); i++) {
        *ptr = 0x00;                                        // reverse mask 0xff normal mask 0x00 
        ptr++;
    }
}
void HUB08SPI::scan() {
    static uint8_t row = 0;                                 // from 0 to 15
    uint8_t *head = displaybuf + row * (width/ 8);
    for (uint8_t line = 0; line < (height / 16); line++) {  // when more than 16 line display
        uint8_t *ptr = head;
        head += width * 2;                                  // width * 16 / 8
        for (uint8_t byte = 0; byte < (width / 8); byte++) {
            uint8_t pixels = *ptr;
            ptr++;
            pixels = pixels ^ mask;                         // reverse mask = 0xff normal mask = 0x00
            SPI.transfer(pixels);
        }
    }
    uint16_t r = -2000;                                     // timeout
    while (!(PIND & (1<<3)) && ++r);
    uint8_t t = (PIND & 0x0F) | (row <<4);
    PORTD = t;
    PIND = 1<<2;                                            // toggle latch
    PIND = 1<<2;
    row = (row + 1) & 0x0F;
}
void HUB08SPI::setBrightness(uint8_t brightness) { analogWrite(3,255-dim_curve[brightness]); }
void HUB08SPI::clearLine(int x,int y, int w) {
    if (y < 0 || y >= height || x >= width) return;
    if (x < 0) {
        w = w+x;
        x = 0;
    }
    if (x + w > width) w = width-x;
    if (w <= 0) return;
    if ((x& 7)+w <= 8) {
        uint8_t m = 0xFF << (8-w);
        m >>= (x& 7);
        displaybuf[y*8+x/8] &= ~m;
    }
    else {
        uint16_t start = y *8 + (x + 7) / 8;                                       // included
        uint16_t end =   y * 8+ (x + w) / 8;                                       // not included
        if (x& 7) displaybuf[start-1] &= ~(0xFF >> (x& 7));
        for (uint16_t p = start; p < end ; p++) displaybuf[p] = 0;
        if ((x+w)& 7) displaybuf[end] &= ~(0xFF << (8-(x+w)& 7));
    }
}
