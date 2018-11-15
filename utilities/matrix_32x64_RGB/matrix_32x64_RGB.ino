// 32x64 RGB LED matrix and RGBmatrixPanel library demo
// https://learn.adafruit.com/32x16-32x32-rgb-led-matrix?view=all

#include <RGBmatrixPanel.h>
#define CLK  8                      // ARDUINO UNO + SAMD21 / USE A4 for SAMD51
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

void setup() {
    matrix.begin();
    matrix.fillScreen(matrix.Color333(0, 0, 0));        // Color333(r[1-7], g[1-7], b[1-7]) 
    matrix.setTextColor(matrix.Color333(7,0,0));
    matrix.setTextSize(1);                              // 1 (8 p high) or 2 (16 p high)
    matrix.setTextWrap(false);
    matrix.setCursor(8, 0);
    matrix.println("TEST message");
}

void loop() { }
