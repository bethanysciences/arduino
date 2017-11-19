#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    while (!Serial);
}
void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("HIGH");

//    int sensorValue = analogRead(A0);
//    float voltage = sensorValue * (5.0 / 1023.0);
//    Serial.println(voltage);
    delay(2000);

    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LOW");
    
    delay(2000);
}
