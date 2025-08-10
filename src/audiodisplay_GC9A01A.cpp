#include "config.h"
#ifdef USE_AUDIODISPLAY_GC9A01A
#include "audiodisplay_GC9A01A.h"
//#include "audiomodul.h"
#include "common.h"

#define FONT2_MIN_CHAR     7
#define FONT2_MAX_CHAR    17

AudioDisplay::AudioDisplay(int8_t _cs, int8_t _dc, int8_t _rst, uint8_t _rot ) :
              Adafruit_GC9A01A(_cs, _dc, _rst) {
  begin();
  setRotation(_rot);
  cur_screen = AudioDisplay::screenmode_t::Screen_Off;
  fillScreen(GC9A01A_BLACK);
  setTextColor(GC9A01A_WHITE);
  setTextSize(3);
  setCursor(80,25);
  println("init");
  fillRect(30,50,180,140,GC9A01A_DARKGREEN);
  boot_line = 40;
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

void AudioDisplay::boot_msg(uint8_t txtsize, const char* msg) {
  uint8_t pixeldiff = 16;
  if (txtsize == 1) {
    pixeldiff = 20;
  }
  boot_line += pixeldiff;
  setCursor(35,boot_line);
  setTextSize(txtsize);
  println(msg);
}

void AudioDisplay::clear() {
  fillScreen(GC9A01A_BLACK);
}

void AudioDisplay::vol(uint8_t _vol) {
  cur_vol= _vol;
  if (_vol > 90) cur_vol=90;
  show_vol();
}

void AudioDisplay::show_vol() {
  if (cur_screen == AudioDisplay::screenmode_t::Screen_Radio || cur_screen == AudioDisplay::screenmode_t::Screen_Media) {
    fillArc(119,119,-90,180,120,120,ARC_WIDTH,GC9A01A_BLACK);
    fillArc(119,119,-90,cur_vol*2,120,120,ARC_WIDTH,GC9A01A_YELLOW);
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
  show_radio_bps();
  show_radio_station();
  show_radio_streamtitle();
  show_vol();
}

void AudioDisplay::screen_radio_select() {
  cur_screen = AudioDisplay::screenmode_t::Screen_RadioSel;
  clear();
}

void AudioDisplay::screen_settings() {
  cur_screen = AudioDisplay::screenmode_t::Screen_Settings;
  clear();
}

void AudioDisplay::screen_media_update() {
  cur_screen = AudioDisplay::screenmode_t::Screen_MediaUpdate;
  clear();
}

void AudioDisplay::screen_media() {
  cur_screen = AudioDisplay::screenmode_t::Screen_Media;
  clear();
  show_media_album();
  show_media_song();
  show_vol();
}

void AudioDisplay::clock_big() {
  clear();
  setTextColor(GC9A01A_WHITE); 
  setTextSize(7);
  setCursor(20,100);
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
  cur_bps = String(mybps);
  show_radio_bps();
}

void AudioDisplay::show_radio_bps() {
  setTextColor(GC9A01A_RED);  
  setTextSize(1);
  setCursor(75, 205);
  println(cur_bps);
}

/// @brief Zeigt den Sender auf dem Display an
/// Benutzter Displaybereich 25,65 bis 215,130
/// Mögliche Ausgabe: 2 Zeilen in Schriftgröße 2 a 10 Zeichen
/// @param mystation Der Sendername als Array of Char
void AudioDisplay::radio_station(const char* mystation) {
  cur_station = String(mystation);
  show_radio_station();
}

void AudioDisplay::show_radio_station() {
  fillRect(25, 65, 190, 55, GC9A01A_BLACK);
  int startAt = 0;
  int strLen = cur_station.length();
  int splitAt = 0;
  int lineNo = 0;
  setTextColor(GC9A01A_ORANGE);
  if (strLen < 10) {
    setTextSize(3);
    setCursor(25, 75);
    print(cur_station);
  } else {
    setTextSize(2);
    while (splitAt < strLen && lineNo < 2) {
      splitAt = getPartStringEnd(cur_station, startAt, FONT2_MIN_CHAR, FONT2_MAX_CHAR);
      if (lineNo <= 1) setCursor(25, 75 + (lineNo * 20));
      print(cur_station.substring(startAt,splitAt));
      startAt = splitAt + 1;
      lineNo++;
    }
  }
}

void AudioDisplay::radio_streamtitle(String& myplayinfo) {
  cur_streamtitle = replaceNonAscii(myplayinfo);
  if ( cur_screen == AudioDisplay::screenmode_t::Screen_Radio ) show_radio_streamtitle();
}

void AudioDisplay::show_radio_streamtitle() {
  fillRect(25, 130, 240, 60, GC9A01A_BLACK);
  int startAt = 0;
  int splitAt = 0;
  int lineNo = 0;
  setTextColor(GC9A01A_GREEN);
  if (cur_streamtitle.length() < 10) {
    setTextSize(3);
    setCursor(25, 130);
    print(cur_streamtitle);
  } else {
    setTextSize(2);
    while (splitAt < cur_streamtitle.length() && lineNo < 3) {
      splitAt = getPartStringEnd(cur_streamtitle, startAt, FONT2_MIN_CHAR, FONT2_MAX_CHAR);
      setCursor(25, 130 + (lineNo * 20));
      print(cur_streamtitle.substring(startAt,splitAt));
      startAt = splitAt + 1;
      lineNo++;
    }
  }
}

void AudioDisplay::radio_select_station(const char* s0, const char* s1, const char* s2) {
  clear();
  String tmpstr;
  setTextSize(2);
  tmpstr = String(s0);
  int startAt = 0;
  int splitAt = 0;
  int lineNo = 0;
  setTextColor(GC9A01A_LIGHTGREY);
  while (splitAt < tmpstr.length() && lineNo < 2) {
    splitAt = getPartStringEnd(tmpstr, startAt, 4, 15);
    if (lineNo <= 1) setCursor(40, 50 + (lineNo * 20));
    print(tmpstr.substring(startAt,splitAt));
    startAt = splitAt + 1;
    lineNo++;
  }
  tmpstr = String(s1);
  startAt = 0;
  splitAt = 0;
  lineNo = 0;
  setTextColor(GC9A01A_ORANGE);
  while (splitAt < tmpstr.length() && lineNo < 2) {
    splitAt = getPartStringEnd(tmpstr, startAt, 4, 15);
    if (lineNo <= 1) setCursor(10, 110 + (lineNo * 20));
    print(tmpstr.substring(startAt,splitAt));
    startAt = splitAt + 1;
    lineNo++;
  }
  tmpstr = String(s2);
  startAt = 0;
  splitAt = 0;
  lineNo = 0;
  setTextColor(GC9A01A_LIGHTGREY);
  while (splitAt < tmpstr.length() && lineNo < 2) {
    splitAt = getPartStringEnd(tmpstr, startAt, 4, 15);
    if (lineNo <= 1) setCursor(40, 170 + (lineNo * 20));
    print(tmpstr.substring(startAt,splitAt));
    startAt = splitAt + 1;
    lineNo++;
  }
//  if (strlen(s0) > 0) show_text_s2(s0,40,50,GC9A01A_LIGHTGREY);
//  if (strlen(s1) > 0) show_text_s2(s1,10,110,GC9A01A_ORANGE);
//  if (strlen(s2) > 0) show_text_s2(s2,40,170,GC9A01A_LIGHTGREY);
}

void AudioDisplay::media_select_album(String& album, const uint16_t * pic) {
  clear();
  drawRGBBitmap(80,20,pic,70,70);
  setTextSize(2);
  setTextColor(GC9A01A_ORANGE);
  setCursor(10, 110);
  println(album);
}

void AudioDisplay::media_select_song(String& album, String& song, const uint16_t * pic) {
  clear();
  drawRGBBitmap(80,20,pic,70,70);
  setTextSize(2);
  setTextColor(GC9A01A_ORANGE);
  setCursor(10, 110);
  println(album);
  setTextColor(GC9A01A_LIGHTGREY);
  setCursor(40, 170);
  println(song);
}

void AudioDisplay::media_album(String& albumName) {
  cur_album = albumName;
}

void AudioDisplay::media_song(String& songName) {
  cur_song = songName;
}

void AudioDisplay::show_media_album() {
  fillRect(25, 85, 190, 55, GC9A01A_BLACK);
  int startAt = 0;
  int strLen = cur_album.length();
  int splitAt = 0;
  int lineNo = 0;
  setTextColor(GC9A01A_ORANGE);
  if (strLen < 10) {
    setTextSize(3);
    setCursor(25, 75);
    print(cur_album);
  } else {
    setTextSize(2);
    while (splitAt < strLen && lineNo < 2) {
      splitAt = getPartStringEnd(cur_album, startAt, FONT2_MIN_CHAR, FONT2_MAX_CHAR);
      if (lineNo <= 1) setCursor(25, 75 + (lineNo * 20));
      print(cur_album.substring(startAt,splitAt));
      startAt = splitAt + 1;
      lineNo++;
    }
  }
}

void AudioDisplay::show_media_song() {
  fillRect(25, 140, 240, 60, GC9A01A_BLACK);
  int startAt = 0;
  int splitAt = 0;
  int lineNo = 0;
  setTextColor(GC9A01A_GREEN);
  if (cur_song.length() < 10) {
    setTextSize(3);
    setCursor(25, 130);
    print(cur_song);
  } else {
    setTextSize(2);
    while (splitAt < cur_song.length() && lineNo < 3) {
      splitAt = getPartStringEnd(cur_song, startAt, FONT2_MIN_CHAR, FONT2_MAX_CHAR);
      setCursor(25, 130 + (lineNo * 20));
      print(cur_song.substring(startAt,splitAt));
      startAt = splitAt + 1;
      lineNo++;
    }
  }
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
/*
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
*/
/*
void AudioDisplay::show_text(String& in_text, int posx, int posy, uint16_t color) {
  int start_pos = 0;
  int chars_per_line;
  int pixel_to_next_line;
  int linecnt = 0;
  setTextColor(color);
  if (in_text.length() > 20) {
    chars_per_line = 18;
    setTextSize(2);
    pixel_to_next_line = 20;
  } else {
    chars_per_line = 10;
    setTextSize(3);
    pixel_to_next_line = 30;
  }
  setCursor(posx, posy);
//  char result_str[chars_per_line+3];
//  do {
//    start_pos = splitStr(in_text,start_pos,chars_per_line,result_str);
    if (start_pos >= 0) {
      setCursor(posx, posy);
      if (linecnt < 2) println(result_str);
      posy += pixel_to_next_line;
      linecnt++;
    }
  } while (start_pos < in_text.length());
}
*/

/*
void AudioDisplay::show_jpg(String& jpgFile) {
// todo
}
*/
/*
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
*/
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

/* Muster aus Internet
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
*/

/// @brief Teilt einen String in Teilstrings auf
/// Bei der Aufteilung des Strings gibt es folgende Regeln:
/// Der String wird - wenn möglich - bei einem Leerzeichen aufgetrennt.
/// Das Ergebnis hat eine Länge von mindestens <minLen>
/// Das Ergebnis hat maximal eine Länge von <maxLen>
/// Das Ergebnis startet an Zeichen <StartAT>
/// @param data Der Quellstring
/// @param startAt Die Position des ersten Zeichens vom Ergebnisstring im Quellstring
/// @param minLen Minimale Länge des Ergebnisstrings
/// @param maxLen Maximale Länge des Ergebnisstrings
/// @return Die Position des letzten benutzten Zeichens im Quellstring
int AudioDisplay::getPartStringEnd(String data, int startAt, int minLen, int maxLen) {
  int retval = -1;
  int maxLenData = data.length()-1;
  for ( int i = startAt + minLen; i <= startAt + maxLen && i < maxLenData; i++ ) {
    if ( data.charAt(i) == ' ' ) {
      retval = i;
    }
  }
  if (retval == -1) retval = startAt + maxLen - 1;
  return retval;
}

String AudioDisplay::replaceNonAscii(String inputString) {
  String result = "";
  for (size_t i = 0; i < inputString.length(); i++) {
    char c = inputString.charAt(i);
    if (c >= 0 && c <= 127) {
      result += c;
    } else {
      // Hier können Ersetzungen für bestimmte nicht-ASCII-Zeichen vorgenommen werden
      String cmpstr;
      bool replaced = false;
      cmpstr = String("ä");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("ae");
        replaced = true;
      } 
      cmpstr = String("ü");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("ue");
        replaced = true;
      } 
      cmpstr = String("ö");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("oe");
        replaced = true;
      } 
      cmpstr = String("Ä");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("Ae");
        replaced = true;
      } 
      cmpstr = String("Ü");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("ue");
        replaced = true;
      } 
      cmpstr = String("Ö");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("oe");
        replaced = true;
      } 
      cmpstr = String("ß");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("ss");
        replaced = true;
      } 
      // Füge weitere Ersetzungen hinzu, falls nötig
      if ( ! replaced ) result += '?'; // Oder ein anderes Ersatzzeichen
    }
  }
  return result;
}

#endif
