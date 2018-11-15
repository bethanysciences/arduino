#include "HUB08.h"
#define WIDTH   64
#define HEIGHT  16

#define PIN_A   4           // 15
#define PIN_B   5           // 13
#define PIN_C   6           // 12
#define PIN_D   7           // 14
#define PIN_OE  3           // 4
#define PIN_R1  11          // 5
#define PIN_STB 2
#define PIN_CLK 13

HUB08 matrix(PIN_A,PIN_B,PIN_C,PIN_D,PIN_OE,PIN_R1,PIN_STB,PIN_CLK);

void setup() {
    matrix.begin();
    matrix.clearDisplay();
    matrix.setTextSize(1);
    matrix.setTextColor(WHITE);
    matrix.setCursor(0,0);
    matrix.println("Rachel ILU FEA&L");
}

void loop() {
    matrix.scan();
}

