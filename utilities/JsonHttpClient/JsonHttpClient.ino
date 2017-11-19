#include <ArduinoJson.h>
#include "WiFi101.h"
char ssid[] = "iotworld";                             
char pass[] = "iotworld";                             
int status = WL_IDLE_STATUS;
WiFiClient client;
// const char* server = "jsonplaceholder.typicode.com";
// const char* resource = "/users/1";
const char* server = "api.weather.gov";
const char* resource = "/points/33.8774,-84.3046/forecast";
// const char* header = "User-Agent: bob@bethanysciences.net/arduinowx01";
// const char* resource = "Accept: application/ld+json";

struct UserData {
  char name[32];
  char company[32];
};

void setup() {
  Serial.begin(115200);
  while (!Serial) { }
  while ( status != WL_CONNECTED) status = WiFi.begin(ssid, pass);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Wifi Connected");
}

void loop() {
  if (connect(server)) {
    if (sendRequest(server, resource) && skipResponseHeaders()) {
      UserData userData;
      if (readReponseContent(&userData)) printUserData(&userData);
    }
  }
  client.stop();    Serial.println("Disconnect");
  delay(60000);     Serial.println("Wait 60 seconds");
}

bool connect(const char* hostName) {
  Serial.print("Connect to "); Serial.println(hostName);
  bool ok = client.connect(hostName, 80);
  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}

bool sendRequest(const char* host, const char* resource) {
  Serial.print("GET "); Serial.println(resource);
  client.print("GET ");
  client.print(resource);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();
  client.flush();
  return true;
}

bool skipResponseHeaders() {
  char endOfHeaders[] = "\r\n\r\n";
  client.setTimeout(10000);
  bool ok = client.find(endOfHeaders);
  if (!ok) Serial.println("No or invalid response");
  return ok;
}

bool readReponseContent(struct UserData* userData) {
  const size_t BUFFER_SIZE = 2048;
  DynamicJsonBuffer jsonBuffer(BUFFER_SIZE);
  JsonObject& root = jsonBuffer.parseObject(client);
  Serial.println("Buffer= "); root.prettyPrintTo(Serial); Serial.println();
  if (!root.success()) {
    Serial.println("JSON parse fail");
    return false;
  }
  strcpy(userData->name, root["name"]);
  strcpy(userData->company, root["company"]["name"]);
  return true;
}

void printUserData(const struct UserData* userData) {
  Serial.print("Name = ");      Serial.println(userData->name);
  Serial.print("Company = ");   Serial.println(userData->company);
}

