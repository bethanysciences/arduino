#include "LEDMatrix.h"
#include "Arduino.h"
#include <SPI.h>

LEDMatrix::LEDMatrix { }
void LEDMatrix::begin(uint8_t *displaybuf, uint16_t width, uint16_t height) {
    this->displaybuf = displaybuf;
    this->width = width;
    this->height = height;
    mask = 0xff;
    state = 0;
    buffer = 0;
    DDRD |= 0xFC;
    TCCR2B = TCCR2B & 0b11111000 | 0x01;                            // 32khz pwm on pin 3 & 11  
    SPI.begin();
    analogWrite(3,128);
    state = 1;
}

void LEDMatrix::drawPoint(uint16_t x, uint16_t y, uint8_t pixel) {
    uint8_t *byte = displaybuf + x / 8 + y * width / 8;
    uint8_t  bit = x % 8;
    if (pixel) *byte |= 0x80 >> bit;
    else *byte &= ~(0x80 >> bit);
}
void LEDMatrix::drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t pixel) {
    for (uint16_t x = x1; x < x2; x++) for (uint16_t y = y1; y < y2; y++) drawPoint(x, y, pixel);
}
void LEDMatrix::clear() {
    uint8_t *ptr = displaybuf;
    for (uint16_t i = 0; i < (width * height / 8); i++) {
        *ptr = 0x00;
        ptr++;
    }
}

void LEDMatrix::reverse() { mask = ~mask; }                         // reverse mask = 0xff normal mask = 0x00
uint8_t LEDMatrix::isReversed() { return mask; }

void LEDMatrix::scan() {
    static uint8_t row = 0;                                         // from 0 to 15
    if (!state) return;
    uint8_t *head = displaybuf + row * (width / 8);
    if ( buffer ) head += (width/8) * height;
    
    
    for (uint8_t line = 0; line < (height / 16); line++) {
        uint8_t *ptr = head;
        head += width * 2;                                          // width * 16 / 8
        
        for (uint8_t byte = 0; byte < (width / 8); byte++) {
            uint8_t pixels = *ptr;
            ptr++;
            pixels = pixels ^ mask;                                 // reverse mask = 0xff normal mask = 0x00
		    SPI.transfer(pixels);
        }
    }
    uint16_t r = -2000;                                             // timeout
    while (!(PIND & (1<<3)) && ++r);
    uint8_t t = (PIND & 0x0F) | (row <<4);
    PORTD = t;
    PIND = 1<<2;                                                    // toggle latch
    PIND = 1<<2;
    row = (row + 1) & 0x0F;
    row = (row + 1) & 0x0F;
}

uint8_t *LEDMatrix::offscreen_buffer() {
	uint8_t *buff = displaybuf;
	if ( ! buffer ) buff += (width/8) * height;
	return buff;
}
void LEDMatrix::swap() { buffer = ! buffer; }
void LEDMatrix::on() { state = 1; }
void LEDMatrix::off() {
    state = 0;
    digitalWrite(oe, HIGH);
}
