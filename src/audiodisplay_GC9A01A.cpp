#include "config.h"
#ifdef USE_DISPLAY_GC9A01A
#include "audiodisplay_GC9A01A.h"
#include "audiodisplay_bmps.h"
#include "audiomodul.h"
#include "common.h"

extern AudioModul* audiomodul_ptr;

AudioDisplay::AudioDisplay(int8_t _cs, int8_t _dc, int8_t _rst, uint8_t _rot ) :
              Adafruit_GC9A01A(_cs, _dc, _rst) {
  rotation = _rot;
}

void AudioDisplay::begin() {
  Adafruit_GC9A01A::begin();
  setRotation(rotation);
  fillScreen(GC9A01A_BLACK);
  setTextColor(GC9A01A_WHITE);
  setTextSize(3);
  setCursor(80,25);
  println("init");
  fillRect(30,50,180,140,GC9A01A_DARKGREY);
  setCursor(35,60);
  setTextSize(1);
}

void AudioDisplay::html_info(String& _html_info) {
  _html_info += String("\"tab_head_display\":\"Display: GC9A01A\"") +
                String(",\"tab_line1_display\":\"SCK:#GPIO: ") + String(TFT_SCK)+ String("\"") +
                String(",\"tab_line2_display\":\"MOSI:#GPIO: ") + String(TFT_MOSI)+ String("\"") +
                String(",\"tab_line3_display\":\"CS:#GPIO: ") + String(TFT_CS)+ String("\"") +
                String(",\"tab_line4_display\":\"DC:#GPIO: ") + String(TFT_DC)+ String("\"");
}

void AudioDisplay::update_display() {
  audiomodul_ptr->display_update_set = false;
  switch (audiomodul_ptr->mode) {
    case MODE_OFF: {
      clear();
      clock_big();
    }
    break;
    case MODE_RADIO: {
      uint8_t num_lines = 0;
      clear();
      clock_small();
      show_vol(audiomodul_ptr->vol);
      // set ip address
      setTextColor(GC9A01A_WHITE);  
      setTextSize(IP_FONTSIZE);
      setCursor(IP_POS_X,IP_POS_Y);
      print(WiFi.localIP().toString());
      // end set ip address
      // set bps
      setTextColor(GC9A01A_RED);  
      setTextSize(1);
      setCursor(75, 205);
      println(audiomodul_ptr->bps);
      // end set bps
      // set station
      num_lines = split4display(audiomodul_ptr->radio_stationname);
      setTextColor(GC9A01A_ORANGE);
      if (num_lines == 1) {
        setTextSize(3);
        setCursor(25, 75);
        print(displaystr[0]);
      } else {
        setTextSize(2);
        uint8_t thisline = 0;
        while (thisline < num_lines && thisline < 2) {
          if (thisline <= 1) setCursor(25, 75 + (thisline * 20));
          print(displaystr[thisline]);
          thisline++;
        }
      }
      // end set station
      // set streamtitle
      num_lines = split4display(audiomodul_ptr->radio_streamtitle);
      setTextColor(GC9A01A_GREEN);
      if (num_lines == 1) {
        setTextSize(3);
        setCursor(25, 130);
        print(displaystr[0]);
      } else {
        setTextSize(2);
        uint8_t thisline = 0;
        while (thisline < num_lines && thisline < 3) {
          setCursor(25, 130 + (thisline * 20));
          print(displaystr[thisline]);
          thisline++;
        }
      }
      // end set streamtitle
    }
    break;
    case MODE_RADIO_SELECT:
      clear();
      break;
    case MODE_SETTINGS:
      clear();
      switch (audiomodul_ptr->new_mode) {
        case MODE_OFF:
          drawRGBBitmap(80,80,off_bmp,OFF_BMP_HEIGHT,OFF_BMP_WIDTH);
        break;
        case MODE_RADIO:
          drawRGBBitmap(80,80,radio_bmp,RADIO_BMP_HEIGHT,RADIO_BMP_WIDTH);
        break;
        case MODE_MEDIA:
          drawRGBBitmap(80,80,media_bmp,MEDIA_BMP_HEIGHT,MEDIA_BMP_WIDTH);
        break;
        case MODE_SPEAKER:
          drawRGBBitmap(80,80,speaker_bmp,SPEAKER_BMP_HEIGHT,SPEAKER_BMP_WIDTH);
        break;
//        case MODE_SETTINGS:
//          drawRGBBitmap(80,80,settings_bmp,SETTINGS_BMP_HEIGHT,SETTINGS_BMP_WIDTH);
//        break;
        case MODE_MUSIC_UPDATE:
          drawRGBBitmap(80,80,music_update_bmp,MUSIC_UPDATE_BMP_HEIGHT,MUSIC_UPDATE_BMP_WIDTH);
        break;
      }

      break;
    default:
      clear();
      break;
  }
}

void AudioDisplay::loop(time_t now) {
  localtime_r(&now, &timeinfo);
  if (audiomodul_ptr->display_update_vol) {
    wipe_vol();
    show_vol(audiomodul_ptr->vol);
    audiomodul_ptr->display_update_vol = false;
  }
  if (audiomodul_ptr->display_update_now) {
    update_display();
    audiomodul_ptr->display_update_now = false;
  }
  if (audiomodul_ptr->display_update_set &&
     (timeinfo.tm_sec == 10 || 
      timeinfo.tm_sec == 20 ||
      timeinfo.tm_sec == 30 ||
      timeinfo.tm_sec == 40 ||
      timeinfo.tm_sec == 50)) {
    update_display();
  }
  if (timeinfo.tm_min != last_min ) {
    last_min = timeinfo.tm_min;
    update_display();
  }
}

uint8_t AudioDisplay::split4display(String& in_str) {
  uint8_t startAt = 0;
  uint8_t splitAt = 0;
  uint8_t lineNo = 0;
  uint8_t strLen = in_str.length();
  uint8_t retval;
  Serial.println(String("split4display: #") + String(in_str) + String("# ") + String(strLen));
  displaystr[0] = "";
  displaystr[1] = "";
  displaystr[2] = "";
  if (strLen < FONT1_MAX_CHAR) {
    displaystr[0] = in_str;
    retval = 1;
  } else {
    while (splitAt < strLen && lineNo < 3) {
      splitAt = getPartStringEnd(in_str, startAt, FONT2_MIN_CHAR, FONT2_MAX_CHAR);
      displaystr[lineNo] = in_str.substring(startAt,splitAt);
      startAt = splitAt + 1;
      if (lineNo < 2) {
        retval = 2;
      } else{
        retval = lineNo+1;
      } 
      lineNo++;
    }
  }
  Serial.println(displaystr[0]);
  Serial.println(displaystr[1]);
  Serial.println(displaystr[2]);
  Serial.println(String("split4display: retval ") + String(retval));
  return retval;
}

// - txtcolor: 0 = weiss, 1 = grün, 2 = rot
void AudioDisplay::bootMessage(uint8_t txtcolor, const char* msg, bool newline) {
  switch (txtcolor) {
  case 0:
    setTextColor(GC9A01A_WHITE);
    break;
  case 1:
    setTextColor(GC9A01A_GREEN);
    break;
  case 2:
    setTextColor(GC9A01A_RED);
    break;
  }
  if ( ! boot_last_nl ) {
    if (newline) {
      setCursor(190 - (strlen(msg) * 6), (boot_line * 10) + 60);
    } else {
      print("  ");
    }
  }
  print(msg);
  if (newline) {
    boot_line++;
    setCursor(35, boot_line * 10 + 60);
  }
  boot_last_nl = newline;
}

void AudioDisplay::clear() {
  fillScreen(GC9A01A_BLACK);
}
  
void AudioDisplay::wipe_vol() {
  fillArc(119,119,-90,180,120,120,ARC_WIDTH,GC9A01A_BLACK);
}

void AudioDisplay::show_vol(uint8_t cur_vol) {
  fillArc(119,119,-90,cur_vol*2,120,120,ARC_WIDTH,GC9A01A_YELLOW);
}

/*
void AudioDisplay::screen_off() {
  cur_screen = AudioDisplay::screenmode_t::Screen_Off;
  clear();
  clock_big();
}
*/
/*
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
*/
/*
void AudioDisplay::screen_radio_select() {
  cur_screen = AudioDisplay::screenmode_t::Screen_RadioSel;
  clear();
}
*/
/*
void AudioDisplay::screen_settings() {
  cur_screen = AudioDisplay::screenmode_t::Screen_Settings;
  clear();
}
*/
/*
void AudioDisplay::screen_media_update() {
  cur_screen = AudioDisplay::screenmode_t::Screen_MediaUpdate;
  clear();
}
*/
/*
void AudioDisplay::screen_media() {
  cur_screen = AudioDisplay::screenmode_t::Screen_Media;
  clear();
//  drawRGBBitmap(80,20,bmpBuffer,80,80);
  show_media_artist();
  show_media_song();
  show_media_bps();
  show_vol();
  ip();
}
*/

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

/*
void AudioDisplay::radio_bps(const char* mybps) {
  cur_bps = String(mybps);
  show_radio_bps();
}
*/
/*
void AudioDisplay::show_radio_bps() {
  setTextColor(GC9A01A_RED);  
  setTextSize(1);
  setCursor(75, 205);
  println(cur_bps);
}
*/
/// @brief Zeigt den Sender auf dem Display an
/// Benutzter Displaybereich 25,65 bis 215,130
/// Mögliche Ausgabe: 2 Zeilen in Schriftgröße 2 a 10 Zeichen
/// @param mystation Der Sendername als Array of Char
/*
void AudioDisplay::radio_station(const char* mystation) {
  cur_station = String(mystation);
  show_radio_station();
}
*/
/*
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
*/
/*
void AudioDisplay::radio_streamtitle(String& myplayinfo) {
  cur_streamtitle = replaceNonAscii(myplayinfo);
  if ( cur_screen == AudioDisplay::screenmode_t::Screen_Radio ) show_radio_streamtitle();
}
*/
/*
void AudioDisplay::show_radio_streamtitle() {
  fillRect(25, 130, 240, 60, GC9A01A_BLACK);
  int startAt = 0;
  int splitAt = 0;
  unsigned int lineNo = 0;
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
*/
/*
void AudioDisplay::radio_select_station(const char* s0, const char* s1, const char* s2) {
  clear();
  String tmpstr;
  setTextSize(2);
  tmpstr = String(s0);
  int startAt = 0;
  int splitAt = 0;
  unsigned int lineNo = 0;
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
*/
/*
void AudioDisplay::media_select_album(String& album, const uint16_t * pic) {
  clear();
  drawRGBBitmap(80,20,pic,70,70);
  setTextSize(2);
  setTextColor(GC9A01A_ORANGE);
  setCursor(10, 110);
  println(album);
}
*/
/*
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
*/
/*
void AudioDisplay::media_bps(String& mybps) {
  cur_bps = mybps;
} 
*/
/*
void AudioDisplay::media_album(String& albumName) {
  cur_album = albumName;
  show_media_album();
}
*/
/*
void AudioDisplay::media_artist(String& artistName) {
  cur_artist = artistName;
  show_media_artist();
}
*/
/*
void AudioDisplay::media_song(String& songName) {
  cur_song = songName;
  show_media_song();
}
*/
/*
void AudioDisplay::show_media_album() {
  int startAt = 0;
  int strLen = cur_album.length();
  int splitAt = 0;
  int lineNo = 0;
  setTextColor(GC9A01A_LIGHTGREY);
  if (strLen < 10) {
    setTextSize(3);
    setCursor(25, 80);
    print(cur_album);
  } else {
    setTextSize(2);
    while (splitAt < strLen && lineNo < 2) {
      splitAt = getPartStringEnd(cur_album, startAt, FONT2_MIN_CHAR, FONT2_MAX_CHAR);
      if (lineNo <= 1) setCursor(25, 80 + (lineNo * 20));
      print(cur_album.substring(startAt,splitAt));
      startAt = splitAt + 1;
      lineNo++;
    }
  }
}
*/
/*
void AudioDisplay::show_media_artist() {
  int startAt = 0;
  int strLen = cur_artist.length();
  int splitAt = 0;
  int lineNo = 0;
  setTextColor(GC9A01A_ORANGE);
  if (strLen < 10) {
    setTextSize(3);
    setCursor(25, 120);
    print(cur_artist);
  } else {
    setTextSize(2);
    while (splitAt < strLen && lineNo < 2) {
      splitAt = getPartStringEnd(cur_album, startAt, FONT2_MIN_CHAR, FONT2_MAX_CHAR);
      if (lineNo <= 1) setCursor(25, 120 + (lineNo * 20));
      print(cur_artist.substring(startAt,splitAt));
      startAt = splitAt + 1;
      lineNo++;
    }
  }
}
*/
/*
void AudioDisplay::show_media_song() {
  int startAt = 0;
  int splitAt = 0;
  int lineNo = 0;
  setTextColor(GC9A01A_GREEN);
  if (cur_song.length() < 10) {
    setTextSize(3);
    setCursor(25, 160);
    print(cur_song);
  } else {
    setTextSize(2);
    while (splitAt < cur_song.length() && lineNo < 3) {
      splitAt = getPartStringEnd(cur_song, startAt, FONT2_MIN_CHAR, FONT2_MAX_CHAR);
      setCursor(25, 160 + (lineNo * 20));
      print(cur_song.substring(startAt,splitAt));
      startAt = splitAt + 1;
      lineNo++;
    }
  }
}
*/
/*
void AudioDisplay::show_media_bps() {
  setTextColor(GC9A01A_RED);  
  setTextSize(1);
  setCursor(75, 205);
  println(cur_bps);
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
