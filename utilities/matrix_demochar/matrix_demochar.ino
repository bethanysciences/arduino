/* -------------------------------------------------------------------------------
  Library only works for Arduino Uno and others based on the ATmega328.
  The pins are hard coded into the library for better performance
  only port D and the SPI Pins are used.

  HUB08-Pin | Arduino Pin | Note
    LA / A  |     4       | Line select A
    LB / B  |     5       | Line select B 
    LC / C  |     6       | Line select C
    LD / D  |     7       | Line select D
    CLK     |   13 (SCK)  | Serial clock
    R1 / R  |   11 (MOSI) | Serial Data (Red pixels)
    OE / EN |     3       | Output Enable (PWM brightness control)
   LAT / STB|     2       | Latch
    The library increases the Timer2 frequency so that the PWM runs on 32 kHz.
    Affected are only the pins 3 and 11, both occupied by the interface.
------------------------------------------------------------------------------- */

#include <avr/pgmspace.h>
#include <SPI.h>
#include "HUB08SPI.h"
#include <TimerOne.h>
#define WIDTH   64
#define HEIGHT  16
HUB08SPI display;
uint8_t displaybuf[WIDTH * HEIGHT / 8];
char * string = "eMGoz HUB08 library demo! 0123456789";
int length = 480; 

void setup() {
    display.begin(displaybuf, WIDTH, HEIGHT);
    Timer1.initialize(200);                                             // slightly faster refresh rate
    Timer1.attachInterrupt(refresh);
    display.setBrightness(200);                                         // 3-255
    display.reverse();
}

volatile uint16_t x = 0;

void loop() {
  static uint16_t lx = 0;
  if (x != lx){
    lx = x;
    printString(string,64-lx);
  }
}

void refresh() {
    static uint16_t count = 0;
    count++;
    if (count > 180){                                                   // scoll every 180 line scans
      count= 0;
      x++;
      if (x > length) x = 0;                                            // reset after 480 pixels
    }
    display.scan();                                                     // refresh a single line of the display
}

void printString(char* string, int x){
    while (*string){
        byte c = *string-32;
        printChar(c,x,0);
        x+=pgm_read_byte_near(font + (c * 33))+1;
        string++;
    }
}

void printChar(byte c, int x, int y){
    byte l = pgm_read_byte_near(font + (c * 33))+1;                     // get pixel width of character
        for (int a=0;a<16;a++){
            clearLine(x,y+a,l);
            writeByte(x,y+a,pgm_read_byte_near(font + (c * 33) +a*2 +1));
            writeByte(x+8,y+a,pgm_read_byte_near(font + (c * 33) +a*2 +2));
    }
}

void writeByte(int x, int y, uint8_t data) {
    if (x >= WIDTH ||y >= HEIGHT || x+8<=0 || y < 0) return;            // outside screen
    uint8_t offset = x & 7;                                             // bit offset
    if (offset) {
        writeByte(x-offset,y,data>>offset);
        writeByte(x+8-offset,y,data<<(8-offset));
    }
    else {
        uint8_t col = x / 8;
        displaybuf[y*8+col] |= data;
    }
}

void clearLine(int x,int y, int w) {
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
        displaybuf[y*8+x/8] &= ~m;
    }
    else {
        uint16_t start = y*8+(x+7)/8;                               // included
        uint16_t end =   y*8+(x+w)/8;                               // not included
        if (x& 7) displaybuf[start-1] &= ~(0xFF >> (x& 7));
        for (uint16_t p = start; p < end ; p++) displaybuf[p] = 0;
        if ((x+w)& 7) displaybuf[end] &= ~(0xFF << (8-(x+w)& 7));
    }
}
