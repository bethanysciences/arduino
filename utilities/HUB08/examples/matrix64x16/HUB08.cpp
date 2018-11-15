#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

#if !defined(__ARM_ARCH) && !defined(ENERGIA) && !defined(ESP8266)
 #include <util/delay.h>
#endif

#include <stdlib.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "HUB08.h"

static uint8_t buffer[HUB08_LCDHEIGHT * HUB08_LCDWIDTH / 8] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x01, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,
    0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0xE0, 0x07, 0x8F, 0xC7, 0xC7, 0xC7, 0xE0,
    0x00, 0x40, 0x0C, 0xCE, 0x6C, 0x6C, 0x6C, 0xE0, 0x00, 0xE0, 0x0C, 0x0C, 0x6C, 0x6C, 0x6C, 0x60,
    0x01, 0xF0, 0x07, 0x8C, 0x6F, 0xEF, 0xEC, 0x60, 0x23, 0xF8, 0x00, 0xCC, 0x6C, 0x0C, 0x0C, 0x60,
    0x33, 0xF8, 0x0C, 0xCE, 0x6C, 0x6C, 0x6C, 0xE0, 0x3B, 0xF8, 0x07, 0x8F, 0xC7, 0xC7, 0xC7, 0xE0,
    0x3B, 0xF8, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x1B, 0xF8, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00,
    0x0B, 0xF8, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void HUB08::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;
    uint8_t *byte = buffer + x / 8 + y * width() / 8;
    uint8_t  bit = x % 8;
    if (color) *byte |= 0x80 >> bit;
    else *byte &= ~(0x80 >> bit);
}

HUB08::HUB08(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, 
             uint8_t r1, uint8_t stb, uint8_t clk) :
             Adafruit_GFX(HUB08_LCDWIDTH, HUB08_LCDHEIGHT) {
    this->clk = clk;
    this->r1 = r1;
    this->stb = stb;
    this->oe = oe;
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
    mask = 0x00;                                            // reverse: mask = 0xff, normal: mask = 0x00
    state = 0;
}

void HUB08::begin() {
    pinMode(a, OUTPUT);
    pinMode(b, OUTPUT);
    pinMode(c, OUTPUT);
    pinMode(d, OUTPUT);
    pinMode(oe, OUTPUT);
    pinMode(r1, OUTPUT);
    pinMode(clk, OUTPUT);
    pinMode(stb, OUTPUT);
    state = 1;
}

void HUB08::scan() {
    static uint8_t row = 0;  // from 0 to 15
    if (!state) return;
    uint8_t *head = buffer + row * (width() / 8);
    for (uint8_t line = 0; line < (height() / 16); line++) {
        uint8_t *ptr = head;
        head += width() * 2;                                        // width * 16 / 8
        for (uint8_t byte = 0; byte < (width() / 8); byte++) {
            uint8_t pixels = *ptr;
            ptr++;
            pixels = pixels ^ mask;                         // reverse: mask = 0xff, normal: mask =0x00
            for (uint8_t bit = 0; bit < 8; bit++) {
                digitalWrite(clk, LOW);
                digitalWrite(r1, pixels & (0x80 >> bit));
                digitalWrite(clk, HIGH);
            }
        }
    }
    digitalWrite(oe, HIGH);                                 // disable display
    digitalWrite(a, (row & 0x01));                          // select row
    digitalWrite(b, (row & 0x02));
    digitalWrite(c, (row & 0x04));
    digitalWrite(d, (row & 0x08));
    digitalWrite(stb, LOW);                                 // latch data
    digitalWrite(stb, HIGH);
    digitalWrite(stb, LOW);
    digitalWrite(oe, LOW);                                  // enable display
    row = (row + 1) & 0x0F;
}

void HUB08::on() { state = 1; }
void HUB08::off() {
    state = 0;
    digitalWrite(oe, HIGH);
}

void HUB08::clearDisplay(void) { memset(buffer, 0, (HUB08_LCDWIDTH * HUB08_LCDHEIGHT / 8)); }
