#define button3 3
#define button4 4            
#define button5 5
#define button6 6

void setup() {
    Serial.begin(115200);
    Serial.println(">>>> Start");  
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
    pinMode(button5, INPUT_PULLUP);
    pinMode(button6, INPUT_PULLUP);
}
   

void loop() {
    if (!digitalRead(button3)) Serial.println("button3");
    if (!digitalRead(button4)) Serial.println("button4");
    if (!digitalRead(button5)) Serial.println("button5");
    if (!digitalRead(button6)) Serial.println("button6");

}

