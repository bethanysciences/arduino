void loop() {
  if (connect(server)) {
    if (sendRequest(server, resource) && skipResponseHeaders()) {
      UserData userData;
      if (readReponseContent(&userData)) {
        printUserData(&userData);
      }
    }
  }
  Serial.println("Disconnect");
  client.stop();
  Serial.println("Wait 60 seconds");
  delay(60000);
}

bool connect(const char* hostName) {
  Serial.print("Connect to ");
  Serial.println(hostName);
  bool ok = client.connect(hostName, 80);
  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}
bool sendRequest(const char* host, const char* resource) {
  Serial.print("GET ");
  Serial.println(resource);
  client.print("GET ");
  client.print(resource);
  client.println(" HTTP/1.0");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();
  return true;
}

bool skipResponseHeaders() {
  char endOfHeaders[] = "\r\n\r\n";
  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);
  if (!ok) Serial.println("No response or invalid response!");
  return ok;
}

bool readReponseContent(struct UserData* userData) {
  const size_t BUFFER_SIZE =
      JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 1024;
  DynamicJsonBuffer jsonBuffer(BUFFER_SIZE);
  JsonObject& root = jsonBuffer.parseObject(client);
  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }
  strcpy(userData->name, root["name"]);
  strcpy(userData->company, root["company"]["name"]);
  return true;
}

void printUserData(const struct UserData* userData) {
  Serial.print("Name = ");
  Serial.println(userData->name);
  Serial.print("Company = ");
  Serial.println(userData->company);
}
