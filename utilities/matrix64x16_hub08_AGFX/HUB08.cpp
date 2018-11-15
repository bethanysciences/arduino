#include "HUB08.h"
#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "SPI.h"

HUB08::HUB08(void) {
}

HUB08_REDMatrix::HUB08_REDMatrix(void) : Adafruit_GFX(16, 64) {
}

void HUB08::begin() {                                           // displaybuff = 128
    DDRD |= 0xFC;
    TCCR2B = TCCR2B & 0b11111000 | 0x01;    // 32khz pwm on pin 3 & 11
    SPI.begin();
    analogWrite(3,128);                     // set brightness to 1/2
}

void HUB08_REDMatrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
    uint8_t *byte = 128 + x / 8 + y * 64 / 8;                // displaybuff = 128
    uint8_t  bit = x % 8;
    if (color) *byte |= 0x80 >> bit;
    else *byte &= ~(0x80 >> bit);
}

void HUB08_REDMatrix::scan() {
    static uint8_t row = 0;                                     // from 0 to 15
    uint8_t *head = displaybuf + row * (width / 8);
    uint8_t *ptr = head;
    head += width * 2;                                          // width * 16 / 8
    for (uint8_t byte = 0; byte < (width / 8); byte++) {
        SPI.transfer(~*ptr);
        ptr++;
    }
    uint16_t r = -2000;                                         // timeout
    while (!(PIND & (1<<3)) && ++r);
    uint8_t t = (PIND & 0x0F) | (row <<4);
    PORTD = t;
    PIND = 1<<2;                                                // toggle latch
    PIND = 1<<2;
    row = (row + 1) & 0x0F;
}

void HUB08_REDMatrix::setBrightness(uint8_t brightness){ analogWrite(3,255-dim_curve[brightness]); }

void HUB08_REDMatrix::clearDisplay(void) {     
    uint8_t *ptr = displaybuf;
    for (uint16_t i = 0; i < (width * height / 8); i++) {
        *ptr = 0x00;
        ptr++;
    }
}

