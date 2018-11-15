#include "LEDMatrix.h"
#include "font.h"
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
#define REFRESH     200     // refresh delay (ms)

#define FONT8x8     0       // global font select

LEDMatrix matrix(PIN_A, PIN_B, PIN_C, PIN_D, PIN_OE, PIN_R1, PIN_STB, PIN_CLK);

uint8_t displaybuf[(WIDTH / 8) * HEIGHT * 2];                               // Display Buffer 128 = 64 * 16 / 8
uint8_t displaybuf_w[((WIDTH / 8) +1 ) * HEIGHT];
byte cell[16];

void MatrixWriteCharacter(int x,int y, char character) {
#if FONT8x8
    for(int i = 0; i < 8; i++) {
        uint8_t *pDst = displaybuf_w + (i + y) * ((WIDTH / 8) + 1) + x  ;
        *pDst = (font_8x8[character - 0x20][i]);
    }
#else
    for(int i=0; i<16; i++) {
        cell[i] = pgm_read_byte_near(&font_8x16[(character - 0x20)][i]);
    }
    // uint8_t *pDst = displaybuf_w + (y) * ((WIDTH / 8) + 1) + x  ;
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
#endif
}

void matrixPrint(String c) {
    for (int i=0 ; i<c.length() ; i++) {
        MatrixWriteCharacter(i,3,c[i]);
        matrix.scan();
    }
}

void setup() {
    matrix.begin(displaybuf, WIDTH, HEIGHT);
    matrix.clear();
	memset(displaybuf_w, 0, sizeof(displaybuf_w));
}

// void matrixDelay(int x) { for (int y=0; y<x; y++) matrix.scan(); }
String poruka="I love Rachel forever, ever, always and longer ";
int pos = 0;                                                        // position in circular display

void loop() {
    scrollText();
}

void scrollText() {
    for (int p=0; p<poruka.length() ; p++) {
	    pos = ( pos + 1 ) % (( WIDTH / 8 )+1);
        // int pos_ch = ( pos + (WIDTH/8) ) % (( WIDTH / 8 )+1);
	    int pos_ch = ( pos + (WIDTH/8) ) % (( WIDTH / 8 )+1);
	    MatrixWriteCharacter(pos_ch,0,poruka.charAt(p));
	    int step_up = ((WIDTH/8)+1)-pos;                            // move up one line when falling off circular buffer
	    for (int o=0; o<8; o++) {
		    uint8_t *src  = displaybuf_w + pos;
		    uint8_t *dest = matrix.offscreen_buffer();
		    int i = 0;
		    for (int y = 0; y < HEIGHT; y++ ) {
			    for (int x = 0; x < (WIDTH/8); x++) {
				    int j  = ( x   < step_up ? y : y-1 ) * ((WIDTH/8)+1) + x;
				    int j1 = ( x+1 < step_up ? y : y-1 ) * ((WIDTH/8)+1) + x+1;
				    *(dest + i) = ( *(src + j) << o ) | (( *(src + j1) & ( 0xff << 8 - o ) ) >> 8 - o );
                    // *(dest + i) = *(src + j);
				    matrix.scan();
				    delayMicroseconds(REFRESH / (WIDTH / 64));
				    i++;
			    }
            }
            matrix.swap();
        }
    }
}
