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

#include <SPI.h>
#include "HUB08SPI.h"
#include <TimerOne.h>

#define WIDTH   64
#define HEIGHT  16

HUB08SPI display;

uint8_t displaybuf[WIDTH * HEIGHT / 8] = {
   0x00, 0x0C, 0x00, 0x3F, 0xFF, 0xC0, 0x00, 0x00, 
   0x00, 0x0E, 0x00, 0x3F, 0xFF, 0xC0, 0x00, 0x00, 
   0x00, 0x07, 0x00, 0x30, 0x00, 0xC0, 0x00, 0x00, 
   0x00, 0x03, 0x80, 0x30, 0x00, 0xC0, 0x00, 0x00, 
   0x00, 0x01, 0xC4, 0x30, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0xEC, 0x30, 0x00, 0x00, 0x00, 0x00, 
   0x03, 0xFC, 0x7C, 0x30, 0x3F, 0xFF, 0xFF, 0xC0, 
   0x03, 0xFC, 0x38, 0x30, 0x3F, 0xFF, 0xFF, 0xC0, 
   0x00, 0x0C, 0x10, 0x30, 0x00, 0xC0, 0x03, 0x80, 
   0x00, 0x0C, 0x00, 0x30, 0x00, 0xC0, 0x07, 0x00, 
   0x0F, 0xFC, 0x00, 0x00, 0x00, 0xC0, 0xCE, 0x30, 
   0x0F, 0xFC, 0x00, 0x00, 0x00, 0xC0, 0xDC, 0x30, 
   0x00, 0x0C, 0x00, 0x00, 0x00, 0xC0, 0xF8, 0x00, 
   0x00, 0x0C, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0x00, 
   0x3F, 0xFC, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
   0x3F, 0xFC, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC 
};

void setup() {
    display.begin(displaybuf, WIDTH, HEIGHT);
    Timer1.initialize(800);                         // 800 refresh rate 1250 Hz per line / 16 = 78 fps
    Timer1.attachInterrupt(refresh);    
    display.setBrightness(64);                      // set brightness to save energy
    delay(2000);                                    // show splash screen
    display.clear();
}

void loop() {
    for (uint16_t t = 0; t < 2000; t++){
        uint16_t x = random(0,WIDTH);
        uint16_t y = random(0,HEIGHT);
        display.drawPoint(x,y,1);
        delay(1);
    }
    for (uint16_t x = 0; x < WIDTH/2; x++){
        display.drawRect(x,0,1,HEIGHT,0);
        display.drawRect(WIDTH-1-x,0,1,HEIGHT,0);
        delay(10);
    }
}

void refresh() { display.scan(); }                  //refresh a single line of the display
