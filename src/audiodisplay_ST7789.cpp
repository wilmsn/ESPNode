#include "config.h"
#ifdef USE_AUDIODISPLAY_ST7789
#include "audiodisplay_ST7789.h"
//#include "audiomodul.h"
#include "common.h"


AudioDisplay::AudioDisplay(int8_t _cs, int8_t _dc, int8_t _mosi, int8_t _sck, int8_t _rst, int8_t _bluint16_t _x, uint16_t _y ) :
              Adafruit_ST7789(_cs, _dc, _mosi, _sck, _rst) {
  tft_bl = _bl;
  tft_x  = _x;
  tft_y  = _Y;
  begin(tft_x, tft_y);
}

void AudioDisplay::clear() {
  fillScreen(COLOR_BLACK);
}

void AudioDisplay::show_ip(const char* myip) {
  setTextColor(COLOR_WHITE);  
  setTextSize(1);
  setCursor(0, tft_y-7);
  println(myip);
}

void AudioDisplay::show_bps(const char* mybps) {
  setTextColor(COLOR_RED);  
  setTextSize(1);
  setCursor(75, 205);
  println(mybps);
}

void AudioDisplay::show_vol(uint8_t vol) {
  if (vol > 90) vol=90;
}

void AudioDisplay::show_time(bool big) {
  setTextColor(COLOR_WHITE); 
  if (big) {
    clear();
    setTextSize(6);
    if ( timeinfo.tm_hour < 10) {
      setCursor(60,40);
    } else {
      setCursor(40,40);
    }
  } else {
    fillRect(0, 0, tft_x, 10, COLOR_BLACK);
    setTextSize(2);
    if ( timeinfo.tm_hour < 10) {
      setCursor(90,0);
    } else {
      setCursor(80,0);
    }
  }
  if ( timeinfo.tm_min < 10) {
    printf("%d:0%d",timeinfo.tm_hour, timeinfo.tm_min);
  } else {
    printf("%d:%d",timeinfo.tm_hour, timeinfo.tm_min);
  }
}

void AudioDisplay::show_info1(const char* myinfo) {
  fillRect(0, 10, tft_x, tft_y/2, COLOR_BLACK);
  show_text(myinfo, 5, 25, COLOR_ORANGE);
}

void AudioDisplay::show_info2(const char* myinfo) {
  fillRect(0, tft_y/2, tft_x, tft_y - 10, COLOR_BLACK);
  show_text(myinfo, 5, tft_y/2+5, COLOR_GREEN);
}

void AudioDisplay::select(const char* s0, const char* s1, const char* s2) {
  clear();
  if (strlen(s0) > 0) show_text_s2(s0,40,50,COLOR_LIGHTGREY);
  if (strlen(s1) > 0) show_text_s2(s1,10,110,COLOR_ORANGE);
  if (strlen(s2) > 0) show_text_s2(s2,40,170,COLOR_LIGHTGREY);
}

void AudioDisplay::select(const char* s0, uint16_t * pic) {
  clear();
  if (strlen(s0) > 0) show_text_s2(s0,10,110,COLOR_ORANGE);
  drawRGBBitmap(80,20,pic,80,80);
}

void AudioDisplay::select(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4) {
  clear();
  setTextSize(2);
  if (strlen(s0) > 0) {
    setTextColor(COLOR_LIGHTGREY);  
    setCursor(70, 50);
    println(s0);
  }
  if (strlen(s1) > 0) {
    setTextColor(COLOR_LIGHTGREY);  
    setCursor(40, 80);
    println(s1);
  }
  if (strlen(s2) > 0) {
    setTextColor(COLOR_ORANGE);  
    setCursor(10, 120);
    println(s2);
  }
  if (strlen(s3) > 0) {
    setTextColor(COLOR_LIGHTGREY);  
    setCursor(40, 150);
    println(s3);
  }
  if (strlen(s4) > 0) {
    setTextColor(COLOR_LIGHTGREY);  
    setCursor(70, 180);
    println(s4);
  }
}

void AudioDisplay::show_text_s2(const char* mytext, int posx, int posy, uint16_t color) {
  int mypos = 0;
  int mytxtlength = strlen(mytext);
  int chars_per_line = 20;
  int pixel_to_next_line;
  setTextColor(color);  
  setTextSize(2);
  pixel_to_next_line = 20;
  char mystr[chars_per_line+1];
  mypos = splitStr(mytext,mypos,chars_per_line,mystr);
  if (mypos < strlen(mytext)) {
    setCursor(posx, posy);
    println(mystr);
    mypos = splitStr(mytext,mypos,chars_per_line,mystr);
  }
  setCursor(posx, posy + pixel_to_next_line);
  println(mystr);
}

void AudioDisplay::show_text(const char* in_text, int posx, int posy, uint16_t color) {
  int start_pos = 0;
  int chars_per_line;
  int pixel_to_next_line;
  int linecnt = 0;
  setTextColor(color);
  if (strlen(in_text) > 20) {
    chars_per_line = 18;
    setTextSize(1);
    pixel_to_next_line = 15;
  } else {
    chars_per_line = 10;
    setTextSize(2);
    pixel_to_next_line = 20;
  }
  setCursor(posx, posy);
  char result_str[chars_per_line+3];
  do {
    start_pos = splitStr(in_text,start_pos,chars_per_line,result_str);
    if (start_pos >= 0) {
      setCursor(posx, posy);
      if (linecnt < 2) println(result_str);
      posy += pixel_to_next_line;
      linecnt++;
    }
  } while (start_pos < strlen(in_text));
}

void AudioDisplay::show_modus(const char* _modusStr) {
  String modusStr = String(_modusStr);
  clear();
  setTextSize(3);
  setCursor(TEXT_UNDER_BMP_X, TEXT_UNDER_BMP_Y);
  if (modusStr == "Off") {
    drawRGBBitmap(BMP_LEFT,BMP_DOWN,off_bmp,OFF_BMP_HEIGHT,OFF_BMP_WIDTH);
    setTextColor(COLOR_RED);  
    println(modusStr);
  }
  if (modusStr == "Radio") {
    drawRGBBitmap(BMP_LEFT,BMP_DOWN,radio_bmp,RADIO_BMP_HEIGHT,RADIO_BMP_WIDTH);
    setTextColor(COLOR_ORANGE);  
    println(modusStr);
  }
  if (modusStr == "Media") {
    drawRGBBitmap(BMP_LEFT,BMP_DOWN,media_bmp,MEDIA_BMP_HEIGHT,MEDIA_BMP_WIDTH);
    setTextColor(COLOR_GREEN);  
    println(modusStr);
  }
  if (modusStr == "Speaker") {
    drawRGBBitmap(BMP_LEFT,BMP_DOWN,speaker_bmp,SPEAKER_BMP_HEIGHT,SPEAKER_BMP_WIDTH);
    setTextColor(COLOR_BLUE);  
    println(modusStr);
  }
  if (modusStr == "Settings") {
    drawRGBBitmap(BMP_LEFT,BMP_DOWN,settings_bmp,SETTINGS_BMP_HEIGHT,SETTINGS_BMP_WIDTH);
    setTextColor(COLOR_RED);  
    println(modusStr);
  }
}

void AudioDisplay::show_jpg(String& jpgFile) {
// todo
}

void AudioDisplay::begin(uint16_t _x, uint16_t _y) {
  tft_x = _x;
  tft_y = _y;
  pinMode(tft_bl, OUTPUT);
  digitalWrite(tft_bl, HIGH);
  init(135,240); 
//  fillScreen(ST77XX_RED);
  setRotation(3);
//  setCursor(20,20);
//  setTextColor(ST77XX_BLACK);
//  println("Hello World!");
}

int AudioDisplay::splitStr(const char* inStr, int startPos, int maxLen, char* resultStr) {
  int char2cut = 0;
  int retval = 0;
  resultStr[0] = 0;
  if (strlen(inStr) > startPos) {
    if (strlen(inStr) > maxLen + startPos) {
      for(int i=startPos; i<maxLen+startPos; i++) {
        if ( inStr[i] == ' ') char2cut = i;
      }
      if (char2cut > 0) {
        retval = char2cut + 1;
      } else {
        retval = startPos + maxLen -1;
        char2cut = startPos + maxLen -1;
      }
    } else {
      char2cut = strlen(inStr);
      retval = char2cut;
    }
    for(int i=startPos; i<char2cut; i++) {
      resultStr[i-startPos] = inStr[i];
    }
    resultStr[char2cut-startPos] = 0;
  } else {
    retval = -1;
  }
  return retval;
}


#endif
