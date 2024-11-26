#ifndef _AUDIODISPLAY_ST7789_H_
#define _AUDIODISPLAY_ST7789_H_

#include "Adafruit_GFX.h"
#include "Adafruit_ST7789.h"
#include "audiodisplay_bmps.h"

#define ARC_SIGMENT_DEGREES 3
#define ARC_WIDTH 5
// define colors
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_BLACK       0x0000
#define COLOR_YELLOW      0xFFE0
#define COLOR_ORANGE      0xFD20
#define COLOR_GREEN       0x07E0
#define COLOR_LIGHTGREY   0xC618
#define COLOR_BLUE        0x001F

#define TEXT_UNDER_BMP_X  30
#define TEXT_UNDER_BMP_Y  140
#define BMP_LEFT 70
#define BMP_DOWN 50


class AudioDisplay : public Adafruit_ST7789{
public:
  AudioDisplay(int8_t _cs, int8_t _dc, int8_t _mosi, int8_t _sck, int8_t _rst, int8_t _bl, uint16_t _x, uint16_t _y);
  void clear();
  void show_ip(const char* myip);
  void show_bps(const char* mybps);
  void show_vol(uint8_t vol);
  void show_info1(const char* myinfo);
  void show_info2(const char* myinfo);
  void show_time(bool big);
  void show_modus(const char* modusStr);
  void show_jpg(String& jpgFile);
  void select(const char* s0, const char* s1, const char* s2);
  void select(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4);
  void select(const char* s0, uint16_t * pic);


private:
  void fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour);
  int splitStr(const char* inStr, int startPos, int maxLen, char* resultStr);
  void show_text(const char* mytext, int posx, int posy, uint16_t color);
  void show_text_s2(const char* mytext, int posx, int posy, uint16_t color);
  uint16_t tft_x, tft_y, tft_bl;
};

#endif
