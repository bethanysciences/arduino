#include <SPI.h>
#include <WiFi101.h>
#include <driver/source/nmasic.h>
void setup() {
    Serial.begin(115200);     while (!Serial);
//    WiFi.setPins(8,7,4,2);      //Configure pins for Adafruit ATWINC1500 Feather
    Serial.println("WiFi101 firmware check.");
    Serial.print("WiFi101 shield: ");
    String fv = WiFi.firmwareVersion();
    String latestFv;
    Serial.print("Firmware version installed: "); Serial.println(fv);
    if (REV(GET_CHIPID()) >= REV_3A0) latestFv = WIFI_FIRMWARE_LATEST_MODEL_B;
    else latestFv = WIFI_FIRMWARE_LATEST_MODEL_A;
    Serial.print("Latest firmware version available: "); Serial.println(latestFv);
    Serial.println();
    if (fv == latestFv) Serial.println("Check result: PASSED");
    else Serial.println("Check result: NOT PASSED");
}

void loop() {  }
