#ifdef USE_AUDIODISPLAY_GC9A01A

#ifndef _AUDIODISPLAY_GC9A01A_H_
#define _AUDIODISPLAY_GC9A01A_H_

#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include "audiodisplay_bmps.h"

#define ARC_SIGMENT_DEGREES 3
#define ARC_WIDTH 5

#define TEXT_UNDER_BMP_X  30
#define TEXT_UNDER_BMP_Y  140
#define BMP_LEFT 70
#define BMP_DOWN 50

/// @brief Ein ENUM für die verschiedenen Modi
typedef enum { 
  Screen_Off = 0, 
  Screen_Radio, 
  Screen_Media, 
  Screen_Speaker, 
  Screen_Settings, 
  Screen_MusicUpdate, 
  Screen_Last 
} screenmode_t;

class AudioDisplay : public Adafruit_GC9A01A {
public:
  // Übergrefend alle Modes
  AudioDisplay(int8_t _cs, int8_t _dc, uint8_t _rot);
  void loop(time_t now);
  void screen(screenmode_t _screen);
  void clear();
  void offscreen();
  void ip();
  void show_bps(const char* mybps);
  void vol(uint8_t vol);
  // App Radio
  void show_info1(const char* myinfo);
  void show_info2(const char* myinfo);
  void show_time(bool big);
  // App Auswahl
  void show_modus(const char* modusStr);
  // App Mediaplayer
  void show_jpg(String& jpgFile);
  void select(const char* s0, const char* s1, const char* s2);
  void select(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4);
  void select(const char* s0, uint16_t * pic);


private:
  void fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour);
  int splitStr(const char* inStr, int startPos, int maxLen, char* resultStr);
  void show_text(const char* mytext, int posx, int posy, uint16_t color);
  void show_text_s2(const char* mytext, int posx, int posy, uint16_t color);
  screenmode_t  cur_screen;
  int last_min;
};

#endif
#endif