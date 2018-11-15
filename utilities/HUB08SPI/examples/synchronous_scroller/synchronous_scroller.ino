#include <avr/pgmspace.h>
#include <SPI.h>
#include "HUB08SPI.h"
#include "fonts.h"
#include <TimerOne.h>

#define WIDTH   64
#define HEIGHT  16

HUB08SPI matrix;

uint8_t displaybuf[WIDTH * HEIGHT / 8];


char * string = "Bobby luvs Rachel";
int length = 480; 
volatile uint16_t x = 0;

void refresh() {
    static uint16_t count = 0;
    count++;
    if (count > 180){                                   // scoll every 180 line scans
        count= 0;
        x++;
        if (x > length) x = 0;                          // reset after 480 pixels
    }
    matrix.scan();                                      // refresh a single line of the display
}

void setup() {
    matrix.begin(displaybuf, WIDTH, HEIGHT);
    matrix.clear();
    Timer1.initialize(200);                             // refresh rate
    Timer1.attachInterrupt(refresh);
    matrix.setBrightness(200); 
}



void loop() {
  static uint16_t lx = 0;
  if (x != lx){
    lx = x;
    printString(string, 64 - lx);
  }
}

void printString(char* string, int x) {
    while (*string){
        byte c = *string - 32;
        printChar(c, x, 0);
        x+=pgm_read_byte_near(font + (c * 33))+1;
        string++;
    }
}

void printChar(byte c, int x, int y) {
   byte l = pgm_read_byte_near(font + (c * 33)) + 1;                    // get pixel width of character
    for (int a = 0; a < 16; a++){   
      clearLine(x, y + a, l);
      writeByte(x, y + a, pgm_read_byte_near(font + (c * 33) +a * 2 + 1));
      writeByte(x + 8, y + a, pgm_read_byte_near(font + (c * 33) +a * 2 + 2));
  }
}

void writeByte(int x, int y, uint8_t data) {
    if (x >= WIDTH ||y >= HEIGHT || x+8<=0 || y < 0) return;            // outside screen
    uint8_t offset = x & 7;                                             // bit offset
    if (offset) {
        writeByte(x-offset,   y, data>>offset);
        writeByte(x+8-offset, y, data<<(8 - offset));
    }
    else {
        uint8_t col = x / 8;
        displaybuf[y * 8 + col] |= data;
    }
}

void clearLine(int x, int y, int w) {
    if (y < 0 || y >= HEIGHT || x >= WIDTH) return;
    if (x < 0) {
        w = w+x;
        x = 0;
    }
    if (x + w > WIDTH) w = WIDTH-x;
    if (w <= 0) return;
    if ((x& 7)+w <= 8) {
        uint8_t m = 0xFF << (8-w);
        m >>= (x& 7);
        displaybuf[ y * 8 + x / 8] &= ~m;
    }
    else {
        uint16_t start = y * 8 + (x + 7) / 8;                               // included
        uint16_t end =   y * 8 + ( x + w) / 8;                              // not included
        if (x& 7) displaybuf[start - 1] &= ~(0xFF >> (x& 7));
        for (uint16_t p = start; p < end ; p++) displaybuf[p] = 0;
        if ((x + w)& 7) displaybuf[end] &= ~(0xFF << (8-(x + w)& 7));
    }
}
