struct grbcol {
  grbcol(){r=0;g=0;b=0;}
  grbcol(char _r, char _g, char _b)
  {
    r = _r;
    g = _g;
    b = _b;
    }
  unsigned char r;
  unsigned char g;
  unsigned char b;
};
struct FriflFileHeader 
{
  FriflFileHeader() : mode(), name(), res(), reserve(){}
  char mode;
  char name[7];
  uint16_t res;
  char reserve[6];
};

struct FriflFileHeader2
{
  FriflFileHeader2() : frame(), disp_time(), reserve2(){}
  uint16_t frame;
  uint16_t disp_time;
  char reserve2[4];
};

struct FriflFile 
{
  FriflFile() : header(){FriflFileHeader();}
 // FriflFile() {}
  void readFriflHeader(File& p_file);
  grbcol* readFriflFrame(File& p_file);
  FriflFileHeader header;
  FriflFileHeader2 header2;
  grbcol* data;
};

void FriflFile::readFriflHeader(File& p_file)
{  
  p_file.read((char*)&header, sizeof(header));
  
  /*Serial.println(header.mode);
  Serial.println(header.name); 
  Serial.println(header.res);
  Serial.println(header.reserve);*/

  if(header.mode == 'A'){
    p_file.read((char*)&header2, sizeof(header2));
    /*Serial.println(header2.frame);
    Serial.println(header2.disp_time); 
    Serial.println(header2.reserve2);*/
  }

  //Serial.print("size: "); Serial.println(40*360/header.res);
};

grbcol* FriflFile::readFriflFrame(File& p_file){
  delete data;
  data = NULL;

  data = new grbcol[(int)(40*360/header.res)];
  
  for(int i = 0; i < (40*360/header.res); i++){
      char p[3];
      p_file.read((char*)&p, sizeof(p));
      data[i] = grbcol(p[0], p[1], p[2]); //variable überladen?!
  }
  return data;
}

extern FriflFile frifl;
