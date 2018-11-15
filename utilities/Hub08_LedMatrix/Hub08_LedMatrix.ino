/**
 * LED Matrix library for http://www.seeedstudio.com/depot/ultrathin-16x32-red-led-matrix-panel-p-1582.html
 * The LED Matrix panel has 32x16 pixels. Several panel can be combined together as a large screen.
 *
 * Coordinate & Connection (Arduino -> panel 0 -> panel 1 -> ...)
 *   (0, 0)                                     (0, 0)
 *     +--------+--------+--------+               +--------+--------+
 *     |   5    |    4   |    3   |               |    1   |    0   |
 *     |        |        |        |               |        |        |<----- Arduino
 *     +--------+--------+--------+               +--------+--------+
 *     |   2    |    1   |    0   |                              (64, 16)
 *     |        |        |        |<----- Arduino
 *     +--------+--------+--------+
 *                             (96, 32)
 *
    Pins [FUNCT] (Uno/Zero) HUB08 Port       (Uno/Zero)   [FUNC]
                            GND    A           (8/4)
                            GND    B           (7/5)
                            GND    C           (6/6)
                   (3/3) (EN)OE    D           (5/7)
      [SPI:MOSI] (11/23)     R1    NC
                             NC    NC
                            GND    STB(Latch)  (2/2)     
                            GND    CLK(clock)  (13/24)    [SPI:CLK]
*/
#include "LEDMatrix.h"
#include "font.h"
#include "TimerOne.h"
#include <SPI.h>
#define PIN_A       4       // Line select a - any digital pin 
#define PIN_B       5       // Line select b - any digital pin
#define PIN_C       6       // Line select c - any digital pin
#define PIN_D       7       // Line select d - any digital pin
#define PIN_OE      3       // Output Enable (PWM brightness control) - any PWM enabled digital pin 
#define PIN_R1      11      // SPI MOSI(serial data) red pixel pin (Arduino R3 pin 11 only)
#define PIN_STB     2       // latch Pin - any digital pin
#define PIN_CLK     13      // SPI serial clock pin (Arduino R3 pin 13 only)
#define WIDTH       64      // display pixel width
#define HEIGHT      16      // display pixel height

LEDMatrix matrix;

#define FONT8x8 1           // define to use 8x8 font
uint8_t displaybuf[WIDTH * HEIGHT / 8];
uint8_t displaybuf_w[((WIDTH/8)+1) * HEIGHT];
byte cell[16];
char * string = "eMGoz HUB08 library demo! 0123456789";
int length = 480;

void setup() {
    matrix.begin(displaybuf, WIDTH, HEIGHT);
    Timer1.initialize(200);                                             // slightly faster refresh rate
    Timer1.attachInterrupt(refresh);
    matrix.reverse();
    matrix.clear();

    // memset(displaybuf_w, 0, sizeof(displaybuf_w));
}

volatile uint16_t x = 0;

void loop () {
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
        if (x > length) x = 0;                                          // reset after 480 pixels
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
        writeByte(x-offset, y, data>>offset);
        writeByte(x+8-offset, y, data<<(8-offset));
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

void MatrixWriteCharacter(int x,int y, char character){
    for(int i=0; i<16; i++) cell[i] = pgm_read_byte_near(&font_8x16[(character - 0x20)][i]);
    uint8_t *pDst = displaybuf_w + x  + y * ((WIDTH/8)+1);
    byte mask = 1;
    for(int j=0; j<8; j++) {
        byte out  = 0;
        for(int i=0; i<8; i++) {
	        out <<= 1;
	        if ( cell[i] & mask ) out |= 1;
        }
        *pDst = out;
        pDst += (WIDTH/8)+1;
        mask <<= 1;
    }
    mask = 1;
    for(int j=0; j<8; j++) {
        byte out  = 0;
        for(int i=8; i<16; i++) {
	        out <<= 1;
	        if ( cell[i] & mask ) out |= 1;
        }
        *pDst = out;
        pDst += (WIDTH/8)+1;
        mask <<= 1;
    }
}

void MatrixWriteChar8x8(int x,int y, char character){
    for(int i=0; i<8; i++) {
        uint8_t *pDst = displaybuf_w + (i+y) * ((WIDTH / 8) + 1) + x  ;
        *pDst = (font_8x8[character - 0x20][i]);
    }
}


void matrixPrint(String c) {
    for (int i=0 ; i<c.length() ; i++) {
        MatrixWriteCharacter(i,3,c[i]);
        matrix.scan();
    }
}


