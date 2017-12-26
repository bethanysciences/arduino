#include "Adafruit_HX8357.h"
#include "SD.h"
#ifndef _GFX_UI_H
#define _GFX_UI_H
#define BUFFPIXEL 20
enum TextAlignment LEFT, CENTER, RIGHT;
class GfxUi {
  public:
    GfxUi(Adafruit_HX8357 * tft);
    void drawString(int x, int y, char *text);
    void drawString(int x, int y, String text);
    void setTextAlignment(TextAlignment alignment);
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t bg);
    void drawBmp(String filename, uint16_t x, uint16_t y);
    void drawProgressBar(uint16_t x, uint16_t y, uint16_t w, 
    uint16_t h, uint8_t percentage, uint16_t frameColor, uint16_t barColor);
  private:
    Adafruit_HX8357 * _tft;
    TextAlignment _alignment = LEFT;
    uint16_t _textColor;
    uint16_t _backgroundColor;
    uint16_t read16(File &f);
    uint32_t read32(File &f);
};
#endif
