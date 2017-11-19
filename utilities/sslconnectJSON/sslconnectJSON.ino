dtostrf#include "WiFi101.h"

char ssid[] = "iotworld";
char pass[] = "iotworld";
int status = WL_IDLE_STATUS;

WiFiSSLClient client;

char server[] = "api.weather.gov";
char locForecast[] = "/points/33.8774,-84.3046";
char prodForecast[] = "/forecast";
char locCurrent[] = "/stations/KPDK";
char prodCurrent[] = "/observations/current";

void setup() {
  Serial.begin(115200);
  while (!Serial);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    Serial.print(">");
    delay(500);
  }
  Serial.println();
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Wifi Connected");

  if (client.connectSSL(server, 443)) {
    Serial.print("connect");    Serial.println(server);
    client.println("GET /points/33.8774,-84.3046/forecast HTTP/1.1");
    client.println("Host: api.weather.gov");
    client.println("User-Agent: bob@bethanysciences.net/arduinowx01/");
    client.println("Accept: application/ld+json");
    client.println("Connection: close");
    client.println();
  }
  else Serial.println("connection failed");
}

void loop() {

  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  if (!client.connected()) {
    Serial.println("disconnecting.");
    Serial.println("-------------------------------");
    client.stop();
    while (true);
  }

}
