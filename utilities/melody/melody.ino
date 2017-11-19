// http://www.arduino.cc/en/Tutorial/Tone
void setup() {
    chime(10);
}

void loop() { }

void chime(int times) {
    for (int i = 0; i < times; i++) {
        for (int ii = 0; ii < 3; ii++) {
            int melody[] = {506, 1911};
            tone(A1, melody[ii], 300);
            delay(300);
        }
    }
}
