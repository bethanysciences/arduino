char ssid[] = "FWI-Guest";     // your network SSID (name) iotworld
char pass[] = "V1sualBr@nd";   // your network password    iotworld
const int UPDATE_INTERVAL_SECS = 10 * 60;  // Update every 10 minutes
boolean USE_TOUCHSCREEN_WAKE = true;       // touchscreen wake up, ~90mA
boolean DEEP_SLEEP = false;                // touchscreen zzz ~10mA doesnt work
int AWAKE_TIME = 5;                        // seconds to stay awake before zzz
#define TFT_DC 15                          // ILI9341 pins
#define TFT_CS 0
#define STMPE_CS 16                        // touchscreen pins
#define STMPE_IRQ 4
const float UTC_OFFSET = -7;               // TimeClient settings
const boolean IS_METRIC = false;
const String THINGSPEAK_CHANNEL_ID = "67284";  //Thingspeak Settings
const String THINGSPEAK_API_READ_KEY = "L2VIW20QVNZJBLAK";
String wundergroundIcons [] = {"chanceflurries","chancerain","chancesleet","chancesnow",
                               "clear","cloudy","flurries","fog","hazy","mostlycloudy",
                               "mostlysunny","partlycloudy","partlysunny","rain","sleet",
                               "snow","sunny","tstorms","unknown"};
