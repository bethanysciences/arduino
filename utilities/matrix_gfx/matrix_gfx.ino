#include "HUB08.h"
#define WIDTH       64
#define HEIGHT      16
#define PIN_A       4
#define PIN_B       5
#define PIN_C       6
#define PIN_D       7
#define PIN_OE      3
#define PIN_R1      11
#define PIN_STB     2
#define PIN_CLK     13
HUB08 matrix(PIN_A, PIN_B, PIN_C, PIN_D, PIN_OE, PIN_R1, PIN_STB, PIN_CLK);

void setup() {
    matrix.begin();
    matrix.clearDisplay();
    matrix.setTextSize(1);
    matrix.setTextColor(WHITE);
    // matrix.setCursor(0,8);
    // matrix.println("Bob loves");
    // matrix.println("Rachel");
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
}

ISR(TIMER0_COMPA_vect) { matrix.scan(); } 

void loop() {
    for( int x = 0; x <= 1000000000; x++ ) {
        matrix.clearDisplay();
        matrix.setCursor(0,0);
        matrix.println(x);
        delay(1000);
    }
}
