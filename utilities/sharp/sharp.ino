#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
// #define SHARP_SCK   24      // 20
// #define SHARP_MOSI  23      // 19
#define SHARP_CS    12
Adafruit_SharpMem display(SCK, MOSI, SHARP_CS);
#define BLACK 0
#define WHITE 1

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("setup");
    display.begin();
    display.clearDisplay();
    display.drawPixel(10, 10, BLACK);
    display.refresh();
    delay(500);
    display.clearDisplay();
    testdrawline();
    delay(500);
    display.clearDisplay();
    testdrawrect();
    delay(500);
    display.clearDisplay();
    testfillrect();
    display.refresh();
    delay(500);
    display.clearDisplay();
    display.fillCircle(display.width()/2, display.height()/2, 10, BLACK);
    display.refresh();
    delay(500);
    display.clearDisplay();
    testdrawroundrect();
    display.refresh();  
    delay(500);
    display.clearDisplay();
    testfillroundrect();
    display.refresh();
    delay(500);
    display.clearDisplay();
    testdrawtriangle();
    display.refresh();
    delay(500);
    display.clearDisplay();  
    testfilltriangle();
    display.refresh();
    delay(500);
    display.clearDisplay();
    testdrawchar();
    display.refresh();
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0,0);
    display.println("Hello, world!");
    display.setTextColor(WHITE, BLACK); // 'inverted' text
    display.println(3.141592);
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.print("0x"); display.println(0xDEADBEEF, HEX);
    display.refresh();
    delay(2000);
}

void loop() {
    display.refresh();
    Serial.println("loop");
    delay(1000);
}

void testdrawchar() {
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0,0);
    for (uint8_t i=0; i < 168; i++) {
        if (i == '\n') continue;
            display.write(i);
        }    
        display.refresh();
}
void testdrawcircle() {
    for (uint8_t i=0; i<display.height(); i+=2) {
        display.drawCircle(display.width()/2-5, display.height()/2-5, i, BLACK);
        display.refresh();
    }
}
void testfillrect() {
    uint8_t color = 1;
    for (uint8_t i=0; i<display.height()/2; i+=3) {
        display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color%2);
        display.refresh();
        color++;
    }
}
void testdrawtriangle() {
    for (uint16_t i=0; i<min(display.width(),display.height())/2; i+=5) {
        display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, BLACK);
        display.refresh();
    }
}
void testfilltriangle() {
    uint8_t color = BLACK;
    for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5) {
        display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, color);
        if (color == WHITE) color = BLACK;
        else color = WHITE;
        display.refresh();
    }
}
void testdrawroundrect() {
    for (uint8_t i=0; i<display.height()/4; i+=2) {
        display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, BLACK);
        display.refresh();
    }
}
void testfillroundrect() {
    uint8_t color = BLACK;
    for (uint8_t i=0; i<display.height()/4; i+=2) {
        display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
        if (color == WHITE) color = BLACK;
        else color = WHITE;
        display.refresh();
    }
}
void testdrawrect() {
    for (uint8_t i=0; i<display.height()/2; i+=2) {
        display.drawRect(i, i, display.width()-2*i, display.height()-2*i, BLACK);
        display.refresh();
    }
}
void testdrawline() {  
    for (uint8_t i=0; i<display.width(); i+=4) {
        display.drawLine(0, 0, i, display.height()-1, BLACK);
        display.refresh();
    }
    for (uint8_t i=0; i<display.height(); i+=4) {
        display.drawLine(0, 0, display.width()-1, i, BLACK);
        display.refresh();
    }
    delay(250);
    display.clearDisplay();
    for (uint8_t i=0; i<display.width(); i+=4) {
        display.drawLine(0, display.height()-1, i, 0, BLACK);
        display.refresh();
    }
    for (int8_t i=display.height()-1; i>=0; i-=4) {
        display.drawLine(0, display.height()-1, display.width()-1, i, BLACK);
        display.refresh();
    }
    delay(250);
    display.clearDisplay();
    for (int8_t i=display.width()-1; i>=0; i-=4) {
        display.drawLine(display.width()-1, display.height()-1, i, 0, BLACK);
        display.refresh();
    }
    for (int8_t i=display.height()-1; i>=0; i-=4) {
        display.drawLine(display.width()-1, display.height()-1, 0, i, BLACK);
        display.refresh();
    }
    delay(250);
    display.clearDisplay();
    for (uint8_t i=0; i<display.height(); i+=4) {
        display.drawLine(display.width()-1, 0, 0, i, BLACK);
        display.refresh();
    }
    for (uint8_t i=0; i<display.width(); i+=4) {
        display.drawLine(display.width()-1, 0, i, display.height()-1, BLACK); 
        display.refresh();
    }
    delay(250);
}
