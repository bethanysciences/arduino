#include "SPI.h"
#include "Wire.h"
#include "WiFi101OTA.h"
#include "WiFi101.h"
#include "WiFiUdp.h"
#include "SD.h"
#include "RTCZero.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_STMPE610.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSans24pt7b.h"
#include "SimpleTimer.h"
#ifdef LOAD_GLCD
  #include "glcdfont.c"
#endif
#ifdef LOAD_FONT2
  #include "Font16.h"
#endif
#ifdef LOAD_FONT4
#include "Font32.h"
#endif
#ifdef LOAD_FONT6
#include "Font64.h"
#endif
#ifdef LOAD_FONT7
  #include "Font7s.h"
#endif

char ssid[] = "iotworld";             // your network SSID (name)
char pass[] = "iotworld";             // your network password
int status = WL_IDLE_STATUS;
WiFiClient client;

RTCZero rtc;
SimpleTimer timer;
int UTCOffset       = -4;             // EDT 2nd sun mar | EST 1st sun NOV
long UTCOffsetEpoch = UTCOffset * 3600;
char timeZone[]     = "EDT";
char ampm[2][3]     = {"am", "pm"};
#define Time24      false
#define MIN         60000             // 1 minute microseconds

#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);
#define FGROUND   ILI9341_WHITE
#define BGROUND   ILI9341_BLUE
#define GREEN     ILI9341_GREEN
#define BLACK     ILI9341_BLACK
#define RED       ILI9341_RED
#define CYAN      ILI9341_CYAN
#define MAGENTA   ILI9341_MAGENTA
#define YELLOW    ILI9341_YELLOW
int LineNu[80];                           // max number of lines
#define MARGIN    8                       // pixels
#define SLINE     16                      // base line height (pixels)
#define LINE      25                      // line height (pixels)
#define TAB       130                     // tab width (pixels)

void convertTime(int Hour24, int *Hour12, int *AorP) {
  *AorP = 0;                              // set as am
  *Hour12 = Hour24;
  if (!Time24) {                          // 24 hour = false
    if (*Hour12 >= 12) {
      *Hour12 -= 12;                      // 1300 > 100
      *AorP = 1;                          // pm
    }
  }
}

void setup() {              // =============== SETUP ====================== //
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  // while (!Serial);
  Serial.print("starting");

                            // -------------- TFT SETUP ------------------- //
  Serial.println("tft start");
  tft.begin();
  tft.setRotation(1);
  tft.setTextWrap(0);
  tft.fillScreen(BGROUND);
  int l = 0;
  for (int i = 0; i <= tft.width()/LINE; i++) {
    LineNu[i] = l; l += LINE;
  }
  int firstl = 1;
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(0, LineNu[firstl]);
  tft.print("TFT width ");          tft.print(tft.width());
  tft.setCursor(tft.width()/2, LineNu[firstl]);
  tft.print("TFT height ");         tft.print(tft.height());

  tft.setCursor(0, LineNu[firstl+1]);
  tft.print("Lines Height");        tft.print(LINE);
  tft.setCursor(tft.width()/2, LineNu[firstl+1]);
  tft.print("TFT Lines ");          tft.print(tft.height() / LINE);


  Serial.print("TFT width ");       Serial.print(tft.width());
  Serial.print("    TFT height ");  Serial.println(tft.height());
  Serial.print("Lines Height");     Serial.print(LINE);
  Serial.print("    TFT Lines ");   Serial.println(tft.height() / LINE);


                           // --------------- SD SETUP -------------------- //
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Fail");
    tft.setCursor(0, LineNu[firstl+2]);
    tft.setTextColor(MAGENTA);
    tft.println("SD Fail");
  }
  tft.setCursor(0, LineNu[firstl+2]);
  tft.setTextColor(FGROUND);
  Serial.println("SD Passed");
  tft.println("SD Passed");

                           // -------------- TOUCH SETUP ------------------ //
  if (!ts.begin()) {
    Serial.println("Touch Fail");
    tft.setCursor(0, LineNu[firstl+3]);
    tft.setTextColor(MAGENTA);
    tft.print("Touch Fail");
    while (1);
  }
  tft.setCursor(0, LineNu[firstl+3]);
  tft.setTextColor(FGROUND);
  Serial.println("Touchscreen started");

                           // --------------- WIFI SETUP ------------------ //
  WiFi.setPins(8,7,4,2);          // for Adafruit Feather M0 WiFi
  Serial.println("start wifi");
  tft.setCursor(0, LineNu[firstl+3]);
  tft.setTextColor(FGROUND);
  tft.print("start wifi");
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    Serial.print(">");
    tft.print(">");
    delay(500);
  }
  Serial.println();
  WiFiOTA.begin("Arduino", "password", InternalStorage);
  digitalWrite(LED_BUILTIN, HIGH);

                          // ----------------- SET CLOCK ------------------ //
  rtc.begin();
  unsigned long epoch;
  Serial.print("post epoch ");    Serial.println(epoch);
  tft.setCursor(0, LineNu[firstl+4]);
  tft.print("post epoch ");       tft.print(epoch);
  while (epoch == 0) {
    epoch = WiFi.getTime();
    Serial.print(".");
    tft.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("post epoch ");  Serial.println(epoch);
  tft.setCursor(0, LineNu[firstl+5]);
  tft.print("post epoch ");     tft.print(epoch);
  rtc.setEpoch(epoch);

  statusLine();
  timer.setInterval(1000, statusLine);
  timer.setInterval(1000, timeLine);
  printScreen();
}


void loop() {
  WiFiOTA.poll();
  timer.run();
}

void printScreen() {     // ================ PRINT SCREEN ================= //
  tft.fillRect(0, 0, 320, 240, BGROUND);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, 50);
  tft.print("Bob");
  tft.setCursor(MARGIN, 100);
  tft.print("Smith");

  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(200, 120);
  tft.print("Bethany");
  tft.setCursor(200, 140);
  tft.print("Sciences");

  tft.setFont(&FreeSans18pt7b);
  tft.setCursor(MARGIN, 200);
  tft.print("Managing Director");


  delay(1000);
}


void statusLine() {       // ================== STATUS ==================== //
  IPAddress ip = WiFi.localIP();
  char stat[80];
  sprintf(stat, "%s %d.%d.%d.%d %ddBm",
          WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI());
  if (WiFi.RSSI() < -70) {
    tft.setFont();
    tft.setTextColor(MAGENTA, BGROUND);
    tft.setCursor(4, 240-8);
    tft.print(stat);
    return;
  }
  tft.setFont();
  tft.setTextColor(FGROUND, BGROUND);
  tft.setCursor(4, 240-8);
  tft.print(stat);
}

void timeLine() {         // =================== TIME ===================== //
  int hour, AorP;
  convertTime(rtc.getHours(), &hour, &AorP);
  char datetime[80];
  sprintf(datetime, "%02d/%02d/%02d %02d:%02d:%02d%s %s",
          rtc.getMonth(), rtc.getDay(), rtc.getYear(),
          hour, rtc.getMinutes(), rtc.getSeconds(),
          ampm[AorP], timeZone);
  tft.setFont();
  tft.setTextColor(FGROUND, BGROUND);
  tft.setCursor(160, 240-8);
  tft.print(datetime);
}

#define BUFFPIXEL 20      // ================== DRAW BMP ================== //
void bmpDraw(char *filename, uint8_t x, uint16_t y) {
  File     bmpFile;
  int      bmpWidth, bmpHeight;
  uint8_t  bmpDepth;
  uint32_t bmpImageoffset;
  uint32_t rowSize;
  uint8_t  sdbuffer[3*BUFFPIXEL];
  uint8_t  buffidx = sizeof(sdbuffer);
  boolean  goodBmp = false;
  boolean  flip    = true;
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  if((x >= tft.width()) || (y >= tft.height())) return;
  Serial.print("loading ");
  Serial.println(filename);
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.println("file not found");
    return;
  }
  if(read16(bmpFile) == 0x4D42) {
    Serial.print("file size:    ");
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile);
    bmpImageoffset = read32(bmpFile);
    Serial.print("image offset: ");
    Serial.println(bmpImageoffset, DEC);
    Serial.print("header size:  ");
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) {
      bmpDepth = read16(bmpFile);
      Serial.print("bit depth:  ");
      Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) {
        goodBmp = true;
        Serial.print("image size: ");
        Serial.println(bmpDepth);
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);
        rowSize = (bmpWidth * 3 + 3) & ~3;
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight; flip = false;
        }
        w = bmpWidth; h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;
        tft.setAddrWindow(x, y, x+w-1, y+h-1);
        for (row=0; row<h; row++) {
          if(flip) pos = bmpImageoffset +
              (bmpHeight - 1 - row) * rowSize;
          else pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) {
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer);
          }
          for (col=0; col<w; col++) {
            if (buffidx >= sizeof(sdbuffer)) {
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0;
            }
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r,g,b));
          }
        }
        Serial.print("loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      }
    }
  }
  bmpFile.close();
  if(!goodBmp) Serial.println("format not recognized");
}
uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}
uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
