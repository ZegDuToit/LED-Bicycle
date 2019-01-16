class TestFile {
  public:
    TestFile(Adafruit_NeoPixel strip, char fileLoc[]){
      _strip = strip;
      _fileLoc = fileLoc;
      _testFile();
    }

  private:
    Adafruit_NeoPixel _strip;
    char *_fileLoc;
    void _testFile();
    
};

void TestFile::_testFile(){
  while(!fatfs.exists(_fileLoc)){
      _strip.setPixelColor(0, 0, 0, 127);
      _strip.show();
      delay(500);
      _strip.clear();
      _strip.show();
      delay(500);
  }
}
