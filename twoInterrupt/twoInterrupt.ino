// Hall Connection
//Yellow -> Blue
//Orange -> Green
//Red    -> Purple

//Macros
#define MINtime 50    // Minimum time between hall interrupts
#define MAXtime 2500  // Maximum time between hall interrupts
#define NUMPIXELS 40  // Number of Pixels in strip

#define DEGREES 360   // ° of circle to show

#define PIXELPIN 5    // datapin to strip
#define HALL_PIN 13   // datapin hall sensor
//#define LED_PIN 13    // Bultin LED

//INCLUDE NEOPIXEL stuff 
#include <Adafruit_NeoPixel.h>

//Include TimerInterrupt stuff
#include <Adafruit_ASFcore.h>
#include <clock.h>
#include <clock_feature.h>
#include <compiler.h>
#include <gclk.h>
#include <i2s.h>
#include <interrupt.h>
#include <interrupt_sam_nvic.h>
#include <parts.h>
#include <pinmux.h>
#include <power.h>
#include <reset.h>
#include <status_codes.h>
#include <system.h>
#include <system_interrupt.h>
#include <system_interrupt_features.h>
#include <avdweb_SAMDtimer.h>


#ifdef __AVR__
  #include <avr/power.h>
#endif

//Include Sleep stuff
#include <Adafruit_SleepyDog.h>

//INCLUDE SPI stuff
#include <SPI.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_SPIFlash_FatFs.h>


#define FLASH_TYPE     SPIFLASHTYPE_W25Q16BV  // Flash chip type.
                                              // If you change this be
                                              // sure to change the fatfs
                                              // object type below to match.

#if defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
  #define FLASH_SS       SS                    // Flash chip SS pin.
  #define FLASH_SPI_PORT SPI                   // What SPI port is Flash on?
  #define NEOPIN         8
#else
  #define FLASH_SS       SS1                    // Flash chip SS pin.
  #define FLASH_SPI_PORT SPI1                   // What SPI port is Flash on?
  #define NEOPIN         40
#endif

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);

Adafruit_SPIFlash flash(FLASH_SS, &FLASH_SPI_PORT);
Adafruit_W25Q16BV_FatFs fatfs(flash);

//Include read and test stuff
#include "testPeri.h"
#include "testFile.h"
#include "readFrifl.h"
#include "disableDot.h"
//END INCLUDE

DisableDot* disableDot = new DisableDot();

FriflFile frifl;

volatile long lastTime = 0, periodTime = 0;
volatile int stripPosition = 0;
volatile bool reset = LOW;
  double stripNum = 0;

void ISR_Hall();
void ISR_Display(struct tc_module *const module_inst);
void ISR_AnimatedFrameShift(struct tc_module *const module_inst);

SAMDtimer stripTimer = SAMDtimer(4, ISR_Display, 1e6, 0);
SAMDtimer animatedFrameTimer = SAMDtimer(3, ISR_AnimatedFrameShift, 1e6, 0);

grbcol* data[2];
File f;
short dataDisp = 0;
unsigned int frameCounter = 0;
uint16_t frameStartPos = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) ;
  Serial.println("Interrupt Test");
  Serial.println();

  //disableDot = new DisableDot();
  pinMode(HALL_PIN, INPUT);
  
  strip.begin();
  strip.setBrightness(0x0f);
  strip.show();

  TestPeri tp(strip);

  flash.begin(FLASH_TYPE);
  fatfs.begin();
  
  char fileLoc[] = "test.frifl";
  TestFile(strip, fileLoc);

  Serial.println("PeripheryTest finished");

  flash.begin(FLASH_TYPE);
  fatfs.begin();

  f = fatfs.open("test.frifl", FILE_READ);
  frifl.readFriflHeader(f);
  stripNum = DEGREES/frifl.header.res;
  frameStartPos = f.position();
  
  Serial.print("mode: "); Serial.println(frifl.header.mode);
  Serial.print("name: "); Serial.println(frifl.header.name);
  Serial.print("res: "); Serial.println(frifl.header.res);

  
  Serial.print("Frame start Pos: "); Serial.println(frameStartPos);
  
  data[0] = frifl.readFriflFrame(f);
  if(frifl.header.mode == 'S'){
    f.close();
    Serial.println("Read finished");
  }else{
    Serial.print("tot Frame: "); Serial.println(frifl.header2.frame);
    Serial.print("display time: "); Serial.println(frifl.header2.disp_time);
    data[1] = frifl.readFriflFrame(f);
  }
  
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), ISR_HALL, FALLING);
  
  Serial.println("Attached Interrupt");
}


void ISR_HALL() {
  reset = HIGH;
  getTime();
}

unsigned long frameTimeLast = 0;

void getTime(){
  stripTimer.enableInterrupt(false);
  animatedFrameTimer.enableInterrupt(false);

  stripPosition = 0;
  
  long now = millis();          // -> Ändern zu micros() da millis zu ungenau; dafüf muss setPeriodMs geändert werden
  long newTime = now - lastTime;
  if(newTime  > MINtime)
    periodTime = newTime;
  lastTime = now;
  
  if(periodTime < MAXtime){
    Serial.print("Valid Time: "); Serial.println(periodTime);
    reset = LOW;
    stripTimer.setPeriodMs(periodTime/(int)stripNum);  //-> Ändern zur base unit micros
    if(frifl.header.mode == 'A'){
      frameTimeLast = millis();
      animatedFrameTimer.setPeriodMs(frifl.header2.disp_time * 1000);
    }
  }
}

void ISR_Display(struct tc_module *const module_inst){
    actDisplay(reset);
}

void actDisplay(bool _reset){
  Serial.println("entering Display routine");
  if(!_reset){
    //Serial.println("showing strip");
    strip.begin();
    for(int i = 0; i < NUMPIXELS-1; ++i){
      Serial.print("Data[");Serial.print(dataDisp);Serial.print("] ");Serial.print("i[");Serial.print(i);Serial.print("] ");Serial.print(data[dataDisp][i].r);Serial.print(" ");Serial.print(data[dataDisp][i].g);Serial.print(" ");Serial.println(data[dataDisp][i].b);
      //strip.setPixelColor(i, (uint8_t)data[dataDisp][stripPosition*NUMPIXELS + i]->r, (uint8_t)data[dataDisp][stripPosition*NUMPIXELS + i]->g, (uint8_t)data[dataDisp][stripPosition*NUMPIXELS + i]->b);
    }
    strip.show();
    ++stripPosition;
    if(stripPosition >= stripNum){
      reset = HIGH;
    }
  } else {
    stripTimer.enableInterrupt(false);
    animatedFrameTimer.enableTimer(false);
    animatedFrameTimer.enableInterrupt(false);
    Serial.println("RESET");
    strip.clear();
    strip.show();
  }
}

void ISR_AnimatedFrameShift(struct tc_module *const module_inst){
  Serial.println("ISR_AnimatedFrameShift");
  dataDisp = !dataDisp;
  unsigned long now = millis();
  Serial.print("FrameTime: "); Serial.println(now - frameTimeLast);
  frameTimeLast = millis();
  ++frameCounter;
  readNewFrame();
}

void readNewFrame(){
  Serial.println("load new Frame");
  if(frameCounter >= frifl.header2.frame -1){
    Serial.print("start from: ");Serial.println(frameStartPos);
    f.seek(frameStartPos);
    frameCounter = -1;
  }
  delete data[!dataDisp];
  data[!dataDisp] = NULL;
  
  data[!dataDisp] = frifl.readFriflFrame(f);
}


void loop() {
}
