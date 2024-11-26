#include "config.h"
#ifdef USE_AUDIODISPLAY_GC9A01A
#include "audiodisplay_GC9A01A.h"
#include "audiomodul.h"
#include "common.h"

AudioDisplay::AudioDisplay(int8_t _cs, int8_t _dc ) :
              Adafruit_GC9A01A(_cs, _dc) {
}

void AudioDisplay::clear() {
  fillScreen(GC9A01A_BLACK);
}

void AudioDisplay::show_ip(const char* myip) {
  setTextColor(GC9A01A_WHITE);  
  setTextSize(1);
  setCursor(75, 225);
  println(myip);
}

void AudioDisplay::show_bps(const char* mybps) {
  setTextColor(GC9A01A_RED);  
  setTextSize(1);
  setCursor(75, 205);
  println(mybps);
}

void AudioDisplay::show_vol(uint8_t vol) {
  if (vol > 90) vol=90;
  fillArc(119,119,-90,180,120,120,ARC_WIDTH,GC9A01A_BLACK);
  fillArc(119,119,-90,vol*2,120,120,ARC_WIDTH,GC9A01A_YELLOW);
}

void AudioDisplay::show_time(bool big) {
  setTextColor(GC9A01A_WHITE); 
  if (big) {
    clear();
    setTextSize(7);
    if ( timeinfo.tm_hour < 10) {
      setCursor(60,100);
    } else {
      setCursor(20,100);
    }
  } else {
    fillRect(80, 30, 90, 23, GC9A01A_BLACK);
    setTextSize(3);
    if ( timeinfo.tm_hour < 10) {
      setCursor(90,30);
    } else {
      setCursor(80,30);
    }
  }
  if ( timeinfo.tm_min < 10) {
    printf("%d:0%d",timeinfo.tm_hour, timeinfo.tm_min);
  } else {
    printf("%d:%d",timeinfo.tm_hour, timeinfo.tm_min);
  }
}

void AudioDisplay::show_info1(const char* myinfo) {
  fillRect(25, 65, 190, 55, GC9A01A_BLACK);
  show_text(myinfo, 25, 65, GC9A01A_ORANGE);
}

void AudioDisplay::show_info2(const char* myinfo) {
  fillRect(0, 130, 240, 60, GC9A01A_BLACK);
  show_text(myinfo, 25, 130, GC9A01A_GREEN);
}

void AudioDisplay::select(const char* s0, const char* s1, const char* s2) {
  clear();
  if (strlen(s0) > 0) show_text_s2(s0,40,50,GC9A01A_LIGHTGREY);
  if (strlen(s1) > 0) show_text_s2(s1,10,110,GC9A01A_ORANGE);
  if (strlen(s2) > 0) show_text_s2(s2,40,170,GC9A01A_LIGHTGREY);
}

void AudioDisplay::select(const char* s0, uint16_t * pic) {
  clear();
  if (strlen(s0) > 0) show_text_s2(s0,10,110,GC9A01A_ORANGE);
  drawRGBBitmap(80,20,pic,80,80);
}

void AudioDisplay::select(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4) {
  clear();
  setTextSize(2);
  if (strlen(s0) > 0) {
    setTextColor(GC9A01A_LIGHTGREY);
    setCursor(70, 50);
    println(s0);
  }
  if (strlen(s1) > 0) {
    setTextColor(GC9A01A_LIGHTGREY);  
    setCursor(40, 80);
    println(s1);
  }
  if (strlen(s2) > 0) {
    setTextColor(GC9A01A_ORANGE);  
    setCursor(10, 120);
    println(s2);
  }
  if (strlen(s3) > 0) {
    setTextColor(GC9A01A_LIGHTGREY);  
    setCursor(40, 150);
    println(s3);
  }
  if (strlen(s4) > 0) {
    setTextColor(GC9A01A_LIGHTGREY);  
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
    setTextSize(2);
    pixel_to_next_line = 20;
  } else {
    chars_per_line = 10;
    setTextSize(3);
    pixel_to_next_line = 30;
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
      setTextColor(GC9A01A_RED);  
      println(modusStr);
  }
  if (modusStr == "Radio") {
      drawRGBBitmap(BMP_LEFT,BMP_DOWN,radio_bmp,RADIO_BMP_HEIGHT,RADIO_BMP_WIDTH);
      setTextColor(GC9A01A_ORANGE);  
      println(modusStr);
  }
  if (modusStr == "Media") {
      drawRGBBitmap(BMP_LEFT,BMP_DOWN,media_bmp,MEDIA_BMP_HEIGHT,MEDIA_BMP_WIDTH);
      setTextColor(GC9A01A_GREEN);  
      println(modusStr);
  }
  if (modusStr == "Speaker") {
      drawRGBBitmap(BMP_LEFT,BMP_DOWN,speaker_bmp,SPEAKER_BMP_HEIGHT,SPEAKER_BMP_WIDTH);
      setTextColor(GC9A01A_BLUE);  
      println(modusStr);
  }
  if (modusStr == "Settings") {
      drawRGBBitmap(BMP_LEFT,BMP_DOWN,settings_bmp,SETTINGS_BMP_HEIGHT,SETTINGS_BMP_WIDTH);
      setTextColor(GC9A01A_RED);  
      println(modusStr);
  }
}

void AudioDisplay::show_jpg(String& jpgFile) {
// todo
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

void AudioDisplay::fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour) {

  byte seg = ARC_SIGMENT_DEGREES; // Segments are 3 degrees wide = 120 segments for 360 degrees
  byte inc = ARC_SIGMENT_DEGREES; // Draw segments every 3 degrees, increase to 6 for segmented ring

  // Calculate first pair of coordinates for segment start
  float sx = cos((start_angle - 90) * DEG_TO_RAD);
  float sy = sin((start_angle - 90) * DEG_TO_RAD);
  uint16_t x0 = sx * (rx - w) + x;
  uint16_t y0 = sy * (ry - w) + y;
  uint16_t x1 = sx * rx + x;
  uint16_t y1 = sy * ry + y;

  // Draw colour blocks every inc degrees
  for (int i = start_angle; i < start_angle + degree; i += inc) {

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * DEG_TO_RAD);
    float sy2 = sin((i + seg - 90) * DEG_TO_RAD);
    int x2 = sx2 * (rx - w) + x;
    int y2 = sy2 * (ry - w) + y;
    int x3 = sx2 * rx + x;
    int y3 = sy2 * ry + y;

    fillTriangle(x0, y0, x1, y1, x2, y2, colour);
    fillTriangle(x1, y1, x2, y2, x3, y3, colour);

    // Copy segment end to sgement start for next segment
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
}

#endif
