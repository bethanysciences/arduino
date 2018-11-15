/*---------------------------------------------------------------------------------*
  Hoover Beam 5x24 cool-white led grid http://www.hoverlabs.co/beam-basics
  Written by Emran Mahbub and Jonathan Li for 
  BSD license, all text above must be included in any redistribution
  
  Modified to accomodate Particle version static display() function
  Modified to work with RTCZero library conflict variable CTRL (changed to CRTL1)
  by Bob Smith bob@bethanysciences.net 2018 under MIT License
*---------------------------------------------------------------------------------*/
#ifndef _BEAM
#define _BEAM
#define BEAMA 0x36
#define BEAMB 0x34
#define BEAMC 0x30
#define BEAMD 0x37
#define MAXFRAME 36
#define SPACE 3
#define KERNING 1
#define REGSEL 0xFD
#define CTRL1 0xC0           // RAM section address  CHANGED to prevent RTCZero conflict
#define PIC 0x00             // Sub Register address
#define MOV 0x01
#define MOVMODE 0x02
#define FRAMETIME 0x03
#define DISPLAYO 0x04
#define CURSRC 0x05
#define CFG 0x06
#define IRQMASK 0x07
#define IRQFRAME 0x08
#define SHDN 0x09
#define CLKSYNC 0x0B
#define PICTURE 0x01            // User modes
#define MOVIE 0x02
#define SCROLL 0x03
#define RIGHT 0
#define LEFT 1
#define FADEON 1
#define FADEOFF 0

class Beam {
    public:
        Beam(int rstpin, int numberOfBeams);    // ALT w/o irapin
        // Beam(int rstpin, int irqpin, int numberOfBeams);
        Beam(int rstpin, int irqpin, uint8_t syncMode, uint8_t beamAddress);
        bool begin(void);
        void initBeam();
        void print(const char* text);
        void printFrame(uint8_t frameToPrint, const char * text);
        void play();
        void draw();
        void display();
        // void display(int frameNum);
        void setScroll(uint8_t direction, uint8_t fade);
        void setSpeed(uint8_t speed);
        void setLoops (uint8_t loops);
        void setMode (uint8_t mode);
        void loadFrameFromRAM(int beam, uint8_t frameNum, uint8_t *pFrameData);
        volatile int beamNumber;
        int checkStatus();
        int status();
    private:
        uint16_t cs[12], segmentmask[8];
        uint8_t cscolumn[25];
        uint8_t _gblMode, _currBeam, _syncMode, _lastFrameWrite, _scrollMode;
        uint8_t _scrollDir, _fadeMode, _frameDelay, _beamMode, _numLoops;
        int _rst, _irq, _beamCount, activeBeams;
        void startNextBeam();
        void initializeBeam(uint8_t b);
        void setPrintDefaults(uint8_t mode, uint8_t startFrame, uint8_t numFrames, 
         uint8_t numLoops, uint8_t frameDelay, uint8_t scrollDir, uint8_t fadeMode);
        void writeFrame(uint8_t addr, uint8_t f);
        // void convertFrame(uint8_t * currentFrame);
        void convertFrame(uint16_t currentFrame);
        unsigned int setSyncTimer();
        void sendWriteCmd(uint8_t addr, uint8_t ramsection, uint8_t subreg, 
         uint8_t subregdata);
        uint8_t sendReadCmd(uint8_t addr, uint8_t ramsection, uint8_t subreg);
        uint8_t i2cwrite(uint8_t address, uint8_t cmdbyte, uint8_t databyte);
        // void convertFrameFromRAM(uint8_t *pFrameData);
};
#endif
