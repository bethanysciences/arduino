#include "HUB08.h"
#include "Adafruit_GFX.h"
#include "TimerOne.h"


#define WIDTH   64
#define HEIGHT  16
uint8_t displaybuf[WIDTH * HEIGHT / 8];

HUB08_REDMatrix matrix = HUB08_REDMatrix();
TimerOne timer;

//void refresh() {
//     matrix.scan();
//}

void setup() {
    matrix.begin();
//    Timer1.initialize(500);                                         // refresh rate
//    Timer1.attachInterrupt(refresh);
    matrix.clearDisplay();
    matrix.setBrightness(200);                                   // 3-255
    matrix.setTextSize(1);
    matrix.setTextColor(BLACK);
    matrix.setCursor(0,0);
    matrix.println("Rachel ILU FEA&L");
}

void loop() {
    matrix.scan();
}

