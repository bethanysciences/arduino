#include "RTCZero.h"              // arduino.cc/en/Reference/RTC
#include "SimpleTimer.h"          // playground.arduino.cc/Code/SimpleTimer
#include "WiFi101.h"              // www.arduino.cc/en/Reference/WiFi101
#include "Adafruit_HX8357.h"      // github.com/adafruit/Adafruit_HX8357_Library
#include "Adafruit_LEDBackpack.h" // github.com/adafruit/Adafruit-LED-Backpack-Library
#include "Adafruit_GFX.h"         // github.com/adafruit/Adafruit-GFX-Library
#include "Fonts/FreeSans9pt7b.h"  // font included with Adafruit-GFX-Library
#include "Fonts/FreeSans12pt7b.h" // font included with Adafruit-GFX-Library
#include "ArialRoundedMTBold_14.h"
#include "ArialRoundedMTBold_36.h"
#include "GfxUi.h"                // Additional UI functions
#include "settings.h"             // WiFi credentials and other geo specific settings
#include "convertTime.h"          // timezone and 12/24 hr conversion
#include "dtostrf.h"              // Convert float to string
#include "ArduinoHttpClient.h"
#include "wxConversions.h"
#include "xmlTakeParam.h"         // parse XML elements
#include "ArduinoJson.h"          // https://bblanchon.github.io/ArduinoJson/

#define SERIAL_DEBUG     // used for serial debugging

// -------------------------- Defines for WIFI Operations ------------------------- //

int status  = WL_IDLE_STATUS;
const char* CONNSTAT[2] = {"DISC", "CONN"};      // Connection preferred lables

// ------------------  Defines for Adafruit HX8357 TFT display -------------------- //

#define TFT_CS    9                         // HX8357 3.5" Feather Wing
#define TFT_DC    10                        // HX8357 3.5" Feather Wing
#define TFT_RST   5                         // HX8357 3.5" Feather Wing
#define TFT_MISO  5                         // HX8357 3.5" Feather Wing
#define SD_CS     2                         // HX8357 3.5" Feather Wing
#define FGROUND   HX8357_WHITE              // primary text color
#define BGROUND   HX8357_BLUE               // primary background color
#define GREEN     HX8357_GREEN
#define BLACK     HX8357_BLACK
#define RED       HX8357_RED
#define CYAN      HX8357_CYAN
#define MAGENTA   HX8357_MAGENTA
#define YELLOW    HX8357_YELLOW
int LineNu[80];                              // line structure
int firstl = 1;
#define MARGIN    8                          // margin (pixels)
#define SLINE     16                         // base line height (pixels)
#define LINE      25                         // line height (pixels)
#define TAB       130                        // tab width (pixels)
int CENTER;                                  // center (pixels)
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
GfxUi ui = GfxUi(&tft);

// ------------------------- Defines for Time Displays ---------------------------- //

#define UTCOFFSET       -7                // EDT 2nd sun mar | EST 1st sun NOV
int UTCEPOCH          = -7 * 3600;        // epoch timezone offset
const char* TIMEZONE  = "MST";            // timezone lables
const char* AMPM[2]   = {"am", "pm"};     // AM/PM preferred lables
bool TIME24           = false;            // false = use 12 hour time
int MIN               = 60000;            // 1 minute of microseconds
int SEC               = 1000;             // 1 second of microseconds
bool BLINKCOLON       = false;
RTCZero rtc;
SimpleTimer timer;

// --------------- Defines for Adafruit LED Backpack i2c LED displays ------------- //

#define hTemp_I2C   0x71    // red
#define cTemp_I2C   0x72    // grn
#define lTemp_I2C   0x70    // blu
Adafruit_7segment HTempDisp = Adafruit_7segment();
Adafruit_7segment LTempDisp = Adafruit_7segment();
Adafruit_7segment CTempDisp = Adafruit_7segment();

// ------------------------------ Defines NOAA METAR ------------------------------ //

WiFiClient wifi;
  int currPort   = 80;
  String currServer = "aviationweather.gov";
  HttpClient xml    = HttpClient(wifi, currServer, currPort);
  struct c_rent {
  String metar;
  String obstimeUTC;
  int tempC;
  int tempF;
  int dewpointC;
  int dewpointF;
  int winddirDeg;
  int windspeedKTS;
  int visibilitySM;
  char altSettingHG[6];
  int year;
  int month;
  int date;
  int hour;
  int minute;
  bool pm;
};
typedef struct c_rent Curr;
Curr Current;

// -------------------------- Defines NOAA FORECAST ------------------------------- //

WiFiSSLClient client;                                 // client for NOAAForecast
  const char* FcstServer = "api.weather.gov";
  int FcstPort = 443;
  const unsigned long HTTPTimeout = 10000;
  struct f_cast {
  const char* generated;
  const char* p0;
  const char* p1;
  const char* p3;
  const char* p4;
  const char* p5;
  const char* p0name;
  const char* p1name;
  const char* p2name;
  const char* p3name;
  const char* p4name;
  const char* p5name;
  int p0tempC;
  int p1tempC;
  int p2tempC;
  int p3tempC;
  int p4tempC;
  int p5tempC;
  const char* p0icon;
  const char* p1icon;
  const char* p2icon;
  const char* p3icon;
  const char* p4icon;
  const char* p5icon;
  const char* p0short;
  const char* p1short;
  const char* p2short;
  const char* p3short;
  const char* p4short;
  const char* p5short;
  int year;
  int month;
  int date;
  int hour;
  int minute;
  bool pm;
};
typedef struct f_cast Fcst;
Fcst Forecast;

//========================
void updateData();
void drawProgress(uint8_t percentage, String text);
void drawTime();
void drawCurrentWeather();
void drawForecast();
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex);
String getMeteoconIcon(String iconText);
void drawAstronomy();
void drawSeparator(uint16_t y);
void sleepNow(int wakeup);
long lastDownloadUpdate = millis();
//========================

void setup() {            // ===================== SETUP ========================== //
  pinMode(LED_BUILTIN, OUTPUT);                   // setup onboard LED

  WiFi.setPins(8,7,4,2);                          // for Adafruit Feather M0 WiFi

  #ifdef SERIAL_DEBUG   // ----------------------- DEBUG -------------------------- //
    Serial.begin(115200);
    while (!Serial);                              // wait for serial to open
    Serial.print("START UP");
  #endif

  // ------------------------------------- LEDS SETUP ----------------------------- //
  HTempDisp.begin(hTemp_I2C);
  LTempDisp.begin(lTemp_I2C);
  CTempDisp.begin(cTemp_I2C);

  // ------------------------------------- TFT SETUP ------------------------------ //
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.setTextWrap(0);
  tft.fillScreen(BGROUND);

  int l = 0;                      // counts display lines based on LINE height pixels
  for (int i = 0; i <= tft.width()/LINE; i++) {
    LineNu[i] = l; l += LINE;
  }

  CENTER = tft.width()/2;
  tft.setFont(&FreeSans9pt7b);
  // tft.setFont(&ArialRoundedMTBold_14);
  tft.setTextColor(FGROUND);
  tft.setCursor(0, LineNu[firstl]);

  tft.print("TFT width ");     tft.println(tft.width());
  tft.print("TFT height ");    tft.println(tft.height());
  tft.print("TFT Lines ");     tft.println(tft.height() / LINE);

  #ifdef SERIAL_DEBUG   // ----------------------- DEBUG -------------------------- //
    Serial.print("TFT width ");  Serial.println(tft.width());
    Serial.print("TFT height "); Serial.println(tft.height());
    Serial.print("TFT Lines ");  Serial.println(tft.height() / LINE);
  #endif

  // ------------------------------------- WIFI SETUP ----------------------------- //

  tft.print("connect to >");    tft.print(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    tft.print(">");
    delay(500);
  }
  tft.print("> connected ");    tft.println();

  digitalWrite(LED_BUILTIN, HIGH);         // lite onboard LED to indicate connection

  // ------------------------------------- SET CLOCK ------------------------------ //

  rtc.begin();
  unsigned long epoch;

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.print("Time (epoch) ");       tft.println(epoch);

  while (epoch == 0) {
    epoch = WiFi.getTime();
    tft.print(".");
    delay(500);
  }

  epoch = epoch + UTCEPOCH;

  rtc.setEpoch(epoch);

  #ifdef SERIAL_DEBUG   // --:--:--:--:--:--:--:--DEBUG :--:--:--:--:--:--:--:--:-- //
    int hour12;
    bool pm;
    Serial.println();
    Serial.print("Epoch timezone adjusted ");  Serial.println(epoch);
    convertTime(rtc.getHours(), TIME24, &hour12, &pm);
    char stat[80];
    IPAddress ip = WiFi.localIP();
    sprintf(stat, "%s ip addr %d.%d.%d.%d %ddBm %02d/%02d/%02d %d:%02d%s %s",
          WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI(),
          rtc.getMonth(), rtc.getDay(), rtc.getYear(),
          hour12, rtc.getMinutes(), AMPM[pm], TIMEZONE);
    Serial.println(stat);
  #endif

  downloadResources();

  // ------------------------------------- SET TIMERS ----------------------------- //

  timer.setInterval(SEC * 5, statusLine);
  timer.setInterval(MIN * 10, NOAAmetar);
  timer.setInterval(MIN * 30, NOAAForecast);
  statusLine();
  NOAAmetar();
  NOAAForecast();
}

long lastDrew = 0;

void loop() {             // ======================== LOOP ======================== //
  timer.run();
}

void drawTime() {
  ui.setTextAlignment(CENTER);
  ui.setTextColor(WHITE, BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  ui.drawString(120, 20, date);
  tft.setFont(&ArialRoundedMTBold_36);
  ui.drawString(120, 56, time);
  drawSeparator(65);
}

void drawAstronomy() {
  int moonAgeImage = 24 * wunderground.getMoonAge().toInt() / 30.0;
  ui.drawBmp("/moon" + String(moonAgeImage) + ".bmp", 120 - 30, 255);
  ui.setTextColor(WHITE, BLACK);
  tft.setFont(&ArialRoundedMTBold_14);
  ui.setTextAlignment(LEFT);
  ui.setTextColor(CYAN, BLACK);
  ui.drawString(20, 270, "Sun");
  ui.setTextColor(WHITE, BLACK);
  ui.drawString(20, 285, SunriseTime());
  ui.drawString(20, 300, SunsetTime());
  ui.setTextAlignment(RIGHT);
  ui.setTextColor(CYAN, BLACK);
  ui.drawString(220, 270, "Moon");
  ui.setTextColor(WHITE, BLACK);
  ui.drawString(220, 285, MoonriseTime());
  ui.drawString(220, 300, MoonsetTime());
}

String getMeteoconIcon(String iconText) {
  if (iconText == "F") return "chanceflurries";
  if (iconText == "Q") return "chancerain";
  if (iconText == "W") return "chancesleet";
  if (iconText == "V") return "chancesnow";
  if (iconText == "S") return "chancetstorms";
  if (iconText == "B") return "clear";
  if (iconText == "Y") return "cloudy";
  if (iconText == "F") return "flurries";
  if (iconText == "M") return "fog";
  if (iconText == "E") return "hazy";
  if (iconText == "Y") return "mostlycloudy";
  if (iconText == "H") return "mostlysunny";
  if (iconText == "H") return "partlycloudy";
  if (iconText == "J") return "partlysunny";
  if (iconText == "W") return "sleet";
  if (iconText == "R") return "rain";
  if (iconText == "W") return "snow";
  if (iconText == "B") return "sunny";
  if (iconText == "0") return "tstorms";
  return "unknown";
}

void drawSeparator(uint16_t y) tft.drawFastHLine(10, y, 240 - 2 * 10, 0x4228);
