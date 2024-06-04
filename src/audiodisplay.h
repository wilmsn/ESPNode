#ifdef USE_AUDIODISPLAY

#ifndef _AUDIODISPLAY_H_
#define _AUDIODISPLAY_H_

#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define ARC_SIGMENT_DEGREES 3
#define ARC_WIDTH 5

class AudioDisplay {
public:
  void begin(Adafruit_GC9A01A* mytft);
  void clear();
  void show_ip(const char* myip);
  void show_bps(const char* mybps);
  void show_vol(uint8_t vol);
  void show_station(const char* mystation);
  void show_title(const char* mytitle);
  void show_time(bool big);
  void select_station(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4);

private:
  void fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour);
  int splitStr(const char* inStr, int startPos, int maxLen, char* resultStr);
  void show_text(const char* mytext, int posx, int posy, uint16_t color);
  
  Adafruit_GC9A01A* tft;
};

#endif
#endif