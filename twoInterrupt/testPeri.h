class TestPeri{
  public:
    TestPeri(Adafruit_NeoPixel strip){
      _strip = strip;
      _testPeri();
    }

  private:
    Adafruit_NeoPixel _strip;  
    void _testPeri();
    bool _testLEDStrip(int delayMil, uint8_t r, uint8_t g, uint8_t b);
    bool _testHALLSens();
};

bool TestPeri::_testHALLSens(){
  if(analogRead(HALL_PIN) >= 1000)
    return true;
    
  return false;
}

bool TestPeri::_testLEDStrip(int delayMil, uint8_t r, uint8_t g, uint8_t b){
  for (int i = 0; i < NUMPIXELS; i++)
  {
    _strip.setPixelColor(i, r, g, b);
    for(int j=0; j < 4; j++)
      if(i>j) _strip.setPixelColor(i-j, r/(j*2+1), g/(j*2+1), b/(j*2+1));
    if(i>4) _strip.setPixelColor(i-5, 0,0,0);
    _strip.show();

    delay(delayMil);
  }

  for (int i = NUMPIXELS-1; i >= 0; i--)
  {
    _strip.setPixelColor(i, r, g, b);
    for(int j=0; j < 4; j++)
      if(i<NUMPIXELS-j) _strip.setPixelColor(i+j+1, r/(j*2+1), g/(j*2+1), b/(j*2+1));
    if(i<NUMPIXELS-4) _strip.setPixelColor(i+5, 0, 0, 0);
    _strip.show();
 
    delay(delayMil);
  }
  _strip.clear();
  _strip.show();
  return true;
}

void TestPeri::_testPeri(){
  if(_testLEDStrip(50, 255, 0, 0)){
    /*while(!_testHALLSens()){
      _strip.setPixelColor(0, 0, 127, 0);
      _strip.show();
      delay(200);
      _strip.clear();
      _strip.show();
      delay(200);
    }*/
  }
}
