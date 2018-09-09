/* e-paper display lib */
#include "GxEPD.h"
#include "GxGDEH029A1/GxGDEH029A1.cpp"
#include "GxIO/GxIO_SPI/GxIO_SPI.cpp"
#include "GxIO/GxIO.cpp"
#include "Fonts/FreeMonoBold9pt7b.h"            // Adafruit GFX Library
#include "Fonts/FreeSansBold24pt7b.h"           // Adafruit GFX Library
#include <ESP8266WiFi.h>
#include "ESP8266HTTPClient.h"
#include <WiFiClient.h>
#include "DNSServer.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
#include "ArduinoJson.h"

const int port = 8888;                          // update server for OTA updates
ESP8266WebServer httpServer(port);
ESP8266HTTPUpdateServer httpUpdater;

GxIO_Class io(SPI, SS, 0, 2);                   // Configure display pins
GxEPD_Class display(io);                        // default selection of D4, D2

byte buttonState = 0;                           // single byte button state for debounce
byte lastButtonState = 0;                       // previous input pin reading
unsigned long lastDebounceTime = 0;             // last time output pin toggled
unsigned long debounceDelay = 50;               // debounce time
String quoteAPIURL = "http://quotesondesign.com/wp-json/posts?filter[orderby]=rand&filter[posts_per_page]=1";

bool OTA = false;                               // OTA mode disabled by default

void setup() {  
  display.init();
  pinMode(1,INPUT_PULLUP);          // down
  pinMode(3,INPUT_PULLUP);          // left
  pinMode(5,INPUT_PULLUP);          // center
  pinMode(12,INPUT_PULLUP);         // right
  pinMode(10,INPUT_PULLUP);         // up
  
  WiFiManager wifiManager;                  // WiFi Manager connects w/ saved creds on fail go to AP mode
  wifiManager.autoConnect("Badgy AP");

  if(digitalRead(5) == 0) {                 // On center button on boot enable OTA upload
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    showIP();
  }
  else {                                    // display quote then sleep for one hour
    showQuote();
    ESP.deepSleep(3600e6, WAKE_RF_DEFAULT);
  }
}

void loop() {
  httpServer.handleClient();
   
  byte reading =  (digitalRead(1)  == 0 ? 0 : (1<<0)) |         // down
                  (digitalRead(3)  == 0 ? 0 : (1<<1)) |         // left
                  (digitalRead(5)  == 0 ? 0 : (1<<2)) |         // center
                  (digitalRead(12) == 0 ? 0 : (1<<3)) |         // right
                  (digitalRead(10) == 0 ? 0 : (1<<4));          // up
                  
  if(reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if((millis() - lastDebounceTime) > debounceDelay) {
    if(reading != buttonState){
      buttonState = reading;
      for(int i=0; i<5; i++){
        if(bitRead(buttonState, i) == 0){
          switch(i){
            case 0: break;            // do something on  down
            case 1: break;            // do something on left
            case 2: break;            // do something on center
            case 3: break;            // do something on right
            case 4: break;            // do something on up                     
            default: break;
          }
        }
      }
    }
  }
  lastButtonState = reading;
}

void showQuote() {
  display.setRotation(3);                             // even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,10);
  
  HTTPClient http;                                    // Parse JSON from HTTP GET request
  http.begin(quoteAPIURL);
  int httpCode = http.GET();
  if(httpCode > 0){
    String payload = http.getString();
    DynamicJsonBuffer jsonBuffer;                     // buffer for quote JSON
    JsonArray& root = jsonBuffer.parseArray(payload);
    String quote = root[0]["content"];
    http.end();
    display.println(quote);
    display.update();
  }
}

void showIP() {
  display.setRotation(3);                             // even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,10);

  String ip = WiFi.localIP().toString();
  String url = WiFi.localIP().toString() + ":"+String(port)+"/update";
  byte charArraySize = url.length() + 1;
  char urlCharArray[charArraySize];
  url.toCharArray(urlCharArray, charArraySize);

  display.println("You are now connected!");
  display.println("");  
  display.println("Go to:");
  display.println(urlCharArray);
  display.println("to upload a new sketch.");
  display.update();  
}
