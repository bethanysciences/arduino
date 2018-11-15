/*--------------------------------------------------------------------------------*
  Hoover Beam 5x24 cool-white led grid http://www.hoverlabs.co/beam-basics
  connections   -  3.3v -
                -  gnd  -
                - sync  -
                - reset -
                -  irq  -
                -  scl  -
                -  sda  -
    b.print("text")     load beam memory
    b.setSpeed(3);      set scroll speed fastst 1 - 15 slowest
    b.setLoops(7);      set scroll speed fastst 1 - 15 slowest
    b.play();           scroll text
    b.display();        print text
  Written by Bob Smith bob@bethanysciences.net 2018 under MIT License
  
*--------------------------------------------------------------------------------*/
#include "Wire.h"
#include "beam.h"
#define RSTPIN 6                                            // any digital pin
#define IRQPIN 9                                            // not used
#define BEAMCOUNT 2                                         // number of beams
Beam b = Beam(RSTPIN, IRQPIN, BEAMCOUNT);                   // initialize

void setup() {
    Serial.begin(115200); while (!Serial);
    Serial.print("Start -> ");
    Wire.begin();
    b.begin();    
    b.print("000 000 000F");
    b.display();
}

void loop() { }
