#include <Adafruit_DotStar.h>
//#ifndef MOB_H
//#define MOB_H

class DisableDot{
  public:
  DisableDot(){
    Adafruit_DotStar strip = Adafruit_DotStar(1, 41, 40, DOTSTAR_BRG);
    strip.begin(); // Initialize pins for output
    strip.show();
  }
};

//#endif

  
