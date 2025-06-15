#include "config.h"
#ifdef USE_AUDIODISPLAY_GC9A01A
#include "audiodisplay_GC9A01A.h"
//#include "audiomodul.h"
#include "common.h"

AudioDisplay::AudioDisplay(int8_t _cs, int8_t _dc, uint8_t _rot ) :
              Adafruit_GC9A01A(_cs, _dc) {
  begin();
  setRotation(_rot);
  cur_screen = AudioDisplay::screenmode_t::Screen_Off;
  fillScreen(GC9A01A_BLACK);
  setTextColor(GC9A01A_WHITE); 
  setTextSize(7);
  setCursor(30,100);
  html_info = String(",\"tab_head_display\":\"Display: GC9A01A\"") +
              String(",\"tab_line1_display\":\"SCK:#GPIO: ") + String(TFT_SCK)+ String("\"") +
              String(",\"tab_line2_display\":\"MOSI:#GPIO: ") + String(TFT_MOSI)+ String("\"") +
              String(",\"tab_line3_display\":\"CS:#GPIO: ") + String(TFT_CS)+ String("\"") +
              String(",\"tab_line4_display\":\"DC:#GPIO: ") + String(TFT_DC)+ String("\"");
}

void AudioDisplay::loop(time_t now) {
  if (timeinfo.tm_min != last_min) {
    last_min = timeinfo.tm_min;
    switch(cur_screen) {
      case AudioDisplay::screenmode_t::Screen_Off:
        clear();
        clock_big();
      break;
      case AudioDisplay::screenmode_t::Screen_Radio:
        clock_small();
      break;
      default:
      // nothing to do
      break;
    }
  }
}

void AudioDisplay::clear() {
  fillScreen(GC9A01A_BLACK);
}

void AudioDisplay::vol(uint8_t vol) {
  if (vol > 90) vol=90;
  if (cur_screen == AudioDisplay::screenmode_t::Screen_Radio || cur_screen == AudioDisplay::screenmode_t::Screen_Media) {
    fillArc(119,119,-90,180,120,120,ARC_WIDTH,GC9A01A_BLACK);
    fillArc(119,119,-90,vol*2,120,120,ARC_WIDTH,GC9A01A_YELLOW);
  }
}

void AudioDisplay::screen_off() {
  cur_screen = AudioDisplay::screenmode_t::Screen_Off;
  clear();
  clock_big();
}

void AudioDisplay::screen_radio() {
  cur_screen = AudioDisplay::screenmode_t::Screen_Radio;
  clear();
  clock_small();
  ip();
}

void AudioDisplay::screen_settings() {
  cur_screen = AudioDisplay::screenmode_t::Screen_Settings;
  clear();
  clock_small();
}

void AudioDisplay::screen_media_update() {
  cur_screen = AudioDisplay::screenmode_t::Screen_MediaUpdate;
  clear();
}

void AudioDisplay::screen_media() {
  cur_screen = AudioDisplay::screenmode_t::Screen_Media;
  clear();
}

void AudioDisplay::clock_big() {
  clear();
  setTextColor(GC9A01A_WHITE); 
  setTextSize(7);
  setCursor(30,100);
  clock_print();
}

void AudioDisplay::clock_small() {
  fillRect(80, 30, 90, 23, GC9A01A_BLACK);
  setTextColor(GC9A01A_WHITE); 
  setTextSize(3);
  setCursor(80,30);
  clock_print();
}

void AudioDisplay::clock_print() {
  if ( timeinfo.tm_hour < 10) printf(" ");
  printf("%d:",timeinfo.tm_hour);
  if ( timeinfo.tm_min < 10) printf("0");
  printf("%d",timeinfo.tm_min);
}

void AudioDisplay::ip() {
  setTextColor(GC9A01A_WHITE);  
  setTextSize(IP_FONTSIZE);
  setCursor(IP_POS_X,IP_POS_Y);
  print(WiFi.localIP().toString());
}

















void AudioDisplay::radio_bps(const char* mybps) {
  setTextColor(GC9A01A_RED);  
  setTextSize(1);
  setCursor(75, 205);
  println(mybps);
}

void AudioDisplay::radio_station(const char* mystation) {
  fillRect(25, 65, 190, 55, GC9A01A_BLACK);
  show_text(mystation, 25, 65, GC9A01A_ORANGE);
}

void AudioDisplay::radio_streamtitle(const char* myplayinfo) {
  fillRect(0, 130, 240, 60, GC9A01A_BLACK);
  show_text(myplayinfo, 25, 130, GC9A01A_GREEN);
}

void AudioDisplay::radio_select_station(const char* s0, const char* s1, const char* s2) {
  clear();
  if (strlen(s0) > 0) show_text_s2(s0,40,50,GC9A01A_LIGHTGREY);
  if (strlen(s1) > 0) show_text_s2(s1,10,110,GC9A01A_ORANGE);
  if (strlen(s2) > 0) show_text_s2(s2,40,170,GC9A01A_LIGHTGREY);
}
/*
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
*/

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


/*
void AudioDisplay::show_jpg(String& jpgFile) {
// todo
}
*/
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
