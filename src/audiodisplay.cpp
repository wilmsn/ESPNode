#include "config.h"
#ifdef USE_AUDIOMODUL

#include "audiomodul.h"
#include "common.h"

#if defined(CONFIG_IDF_TARGET_ESP32) 
#warning ESP32
#warning "Compiling AudioDisplay with Settings for ESP32"
//TFT Settings
// SCL (Display) => SCK  
// SDA (Display) => MOSI 
#define GC9A01A_TFT_CS                  21
#define GC9A01A_TFT_DC                  5
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S2)
#warning ESP32S2
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
//#warning ESP32S3
#warning "Compiling AudioDisplay with Settings for ESP32-S3"
//#warning "Settings for ESP32-S3"
// Display
// SCL (Display) => SCK                 12
// SDA (Display) => MOSI                11
#define GC9A01A_TFT_CS                  8
#define GC9A01A_TFT_DC                  9
#endif
#define ARC_SIGMENT_DEGREES             3
#define ARC_WIDTH                       5


void AudioDisplay::clear() {
  tft->fillScreen(GC9A01A_BLACK);
}

void AudioDisplay::show_ip(const char* myip) {
  tft->setTextColor(GC9A01A_WHITE);  
  tft->setTextSize(1);
  tft->setCursor(75, 225);
  tft->println(myip);
}

void AudioDisplay::show_bps(const char* mybps) {
  tft->setTextColor(GC9A01A_RED);  
  tft->setTextSize(1);
  tft->setCursor(75, 205);
  tft->println(mybps);
}

void AudioDisplay::show_vol(uint8_t vol) {
  if (vol > 90) vol=90;
  fillArc(119,119,-90,180,120,120,ARC_WIDTH,GC9A01A_BLACK);
  fillArc(119,119,-90,vol*2,120,120,ARC_WIDTH,GC9A01A_YELLOW);
}

void AudioDisplay::show_time(bool big) {
  tft->setTextColor(GC9A01A_WHITE); 
  if (big) {
    clear();
    tft->setTextSize(7);
    if ( timeinfo.tm_hour < 10) {
      tft->setCursor(60,100);
    } else {
      tft->setCursor(20,100);
    }
  } else {
    tft->fillRect(80, 30, 90, 23, GC9A01A_BLACK);
    tft->setTextSize(3);
    if ( timeinfo.tm_hour < 10) {
      tft->setCursor(90,30);
    } else {
      tft->setCursor(80,30);
    }
  }
  if ( timeinfo.tm_min < 10) {
    tft->printf("%d:0%d",timeinfo.tm_hour, timeinfo.tm_min);
  } else {
    tft->printf("%d:%d",timeinfo.tm_hour, timeinfo.tm_min);
  }
}

void AudioDisplay::show_station(const char* mystation) {
  tft->fillRect(25, 65, 190, 55, GC9A01A_BLACK);
  show_text(mystation, 25, 65, GC9A01A_ORANGE);
}

void AudioDisplay::show_title(const char* mytitle) {
  tft->fillRect(0, 130, 240, 60, GC9A01A_BLACK);
  show_text(mytitle, 25, 130, GC9A01A_GREEN);
}

void AudioDisplay::select_station(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4) {
  clear();
  tft->setTextSize(2);
  if (strlen(s0) > 0) {
    tft->setTextColor(GC9A01A_LIGHTGREY);  
    tft->setCursor(70, 50);
    tft->println(s0);
  }
  if (strlen(s1) > 0) {
    tft->setTextColor(GC9A01A_LIGHTGREY);  
    tft->setCursor(40, 80);
    tft->println(s1);
  }
  if (strlen(s2) > 0) {
    tft->setTextColor(GC9A01A_ORANGE);  
    tft->setCursor(10, 120);
    tft->println(s2);
  }
  if (strlen(s3) > 0) {
    tft->setTextColor(GC9A01A_LIGHTGREY);  
    tft->setCursor(40, 150);
    tft->println(s3);
  }
  if (strlen(s4) > 0) {
    tft->setTextColor(GC9A01A_LIGHTGREY);  
    tft->setCursor(70, 180);
    tft->println(s4);
  }
}

void AudioDisplay::show_text(const char* mytext, int posx, int posy, uint16_t color) {
  int mypos = 0;
  int mytxtlength = strlen(mytext);
  int chars_per_line;
  int pixel_to_next_line;
  tft->setTextColor(color);  
  if (mytxtlength > 20) {
    chars_per_line = 16;
    tft->setTextSize(2);
    pixel_to_next_line = 20;
  } else {
    chars_per_line = 10;
    tft->setTextSize(3);
    pixel_to_next_line = 30;
  }
  char mystr[chars_per_line+1];
  mypos = splitStr(mytext,mypos,chars_per_line,mystr);
  if (mypos < strlen(mytext)) {
    tft->setCursor(posx, posy);
    tft->println(mystr);
    mypos = splitStr(mytext,mypos,chars_per_line,mystr);
  }
  tft->setCursor(posx, posy + pixel_to_next_line);
  tft->println(mystr);
}

void AudioDisplay::begin(Adafruit_GC9A01A* mytft) {
  tft = mytft;
  tft->begin();
  tft->fillScreen(GC9A01A_BLACK);
}

int AudioDisplay::splitStr(const char* inStr, int startPos, int maxLen, char* resultStr) {
  int char2cut = 0;
  int retval = 0;
  resultStr[0] = 0;
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
  return retval;
}

void AudioDisplay::fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour)
{

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

    tft->fillTriangle(x0, y0, x1, y1, x2, y2, colour);
    tft->fillTriangle(x1, y1, x2, y2, x3, y3, colour);

    // Copy segment end to sgement start for next segment
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
}


#endif

