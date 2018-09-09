/***********************************
MKR1000 https://store.arduino.cc/usa/arduino-mkr1000
- aref       5v -
- dac0/a0   vin -
- a1        vcc - 
- a2        gnd -
- a3      reset - 
- a4      14 tx -
- a5      13 rx - 
- a6     12 scl - 
- 0      11 sda - 
- 1     10 miso - 
- ~2      9 sck -
- ~3     8 mosi -
- ~4          7 - 
- ~5          6 - 

Hoover Beam 5x24 cool-white led grid http://www.hoverlabs.co/beam-basics
-  3.3v -
-  gnd  -
- sync  -
- reset -
-  irq  -
-  scl  -
-  sda  -
b.setSpeed(3);
b.setLoops(7);
b.play();

***********************************/ 

#include "Arduino.h"
#include "Wire.h"
#include "stdint.h"
#include "beam.h"
#define RSTPIN 6                            // any digital pin
#define IRQPIN 9                            // not used
#define BEAMCOUNT 2                         // number of beams
Beam b = Beam(RSTPIN, IRQPIN, BEAMCOUNT);   // initialize

void setup() {
    Wire.begin();
    b.begin();
    b.print("I Love Rachel FEAL");
    b.play();
}

void loop() {

}
