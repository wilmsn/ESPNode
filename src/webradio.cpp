#include "config.h"
#include "common.h"

#ifdef WEBRADIO
//#include "webradio.h"
#include "Audio.h"
#include "AiEsp32RotaryEncoder.h"
#include <Preferences.h>

#ifdef CONFIG_ESP32S3
#warning "Compiling Webradio with Settings for ESP32-S3"
#endif
#ifdef CONFIG_ESP32
#warning "Compiling Webradio with Settings for ESP32"
#endif

/// @brief Eine Instanz für das Audio 
Audio audio;
/// @brief Die Variable timeinfo wird im Rumpfprogramm verwaltet, hier nur für die Anzeige der Zeit genutzt.
extern tm timeinfo;


/// @brief Level definitions:
/// @brief Level beginnen bei 0 und enden bei MAXLEVEL-1
/// @brief 
#define MAXLEVEL    3
/// @brief Level 0 ist die Lautst#rke
#define LEVEL0_MIN  0
#define LEVEL0_MAX  100
/// @brief Level 1 ist die Senderauswahl
#define LEVEL1_MIN  0
#define LEVEL1_MAX  9
/// @brief Level 2 ist ???
#define LEVEL2_MIN  0
#define LEVEL2_MIN  0
// Start definitions
#define ACTOR_START_VALUE     true
#define ACTOR_ON_VALUE        true


/// The settings for each level for the rotary encoder
/// 0 => Radio playing level
/// 1 => station select level
/// 2 => ????
level_t level[MAXLEVEL+1];
/// @brief Das aktuelle Level des Rotaryencoders
uint8_t cur_level;
/// @brief Die aktuelle Position des Rotaryencoders
uint8_t cur_position = 0;
/// @brief Die aktuelle Station aus der Senderliste
uint8_t cur_station = 0;
/// @brief Array to take all the Stations (URL and Name)
station_t station[MAXSTATION];
/// @brief Zeitmessung Rotary Schalter
unsigned long millis_Button_pressed = 0;
/// @brief Letzter Zustand Rotary Schalter
bool wasButtonDown = false;
/// @brief Alte Minute u den Minutenwechsel sicher zu stellen
int last_min = 0;
/// @brief Titleinfos to display
char title[128];
/// @brief Current state of the Radio
bool radio_is_on;


/// @brief Instance for rotary encoder
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);
/// @brief Instance for TFT Display
Adafruit_GC9A01A tftx(GC9A01A_TFT_CS, GC9A01A_TFT_DC);
/// @brief Instance for displayhandling
RadioDisplay mydisplay;
/// @brief Instances to save the Preferences
/// @brief For stations and other settings
//Preferences prefs;
extern Preferences preferences;
extern String cons_str;
extern AsyncWebSocket ws;

void IRAM_ATTR readRotaryISR() {
  rotary.readEncoder_ISR();
}

void Webradio::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword) {
  if ( !preferences.isKey("cur_vol") ) {
    preferences.putUChar("cur_vol", 5);
    preferences.putUChar("cur_station", 0);
  }
  cur_vol = preferences.getUChar("cur_vol");
  cur_level = 0;
  cur_station = preferences.getUChar("cur_station");
  uint8_t mylevel = 0;
  level[mylevel].min = 0;
  level[mylevel].max = 100;
  level[mylevel].cur = cur_vol;
  mylevel++;
  level[mylevel].min = 0;
  level[mylevel].max = MAXSTATION-1;
  level[mylevel].cur = cur_station;
  mylevel++;
  level[mylevel].min = 0;
  level[mylevel].max = 1;
  level[mylevel].cur = 0;
  File f = LittleFS.open( "/sender.txt", "r" );
  if (f) {
    for (int i=0; i<MAXSTATION; i++) {
      snprintf(station[i].name,STATION_NAME_LENGTH,"%s",f.readStringUntil('\n').c_str());
      snprintf(station[i].url,STATION_URL_LENGTH,"%s",f.readStringUntil('\n').c_str());
    }
    f.close();
  }
  f = LittleFS.open( "/sender.txt", "r" );
  if (f) {
    for (int i=0; i<MAXSTATION; i++) {
      Serial.printf("%s=>%s\n",f.readStringUntil('\n').c_str(),station[i].name);
      Serial.printf("%s=>%s\n",f.readStringUntil('\n').c_str(),station[i].url);
    }
    f.close();
  }
  if (audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT)) {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.print("Set PinOut: BCLK:");
    Serial.print(I2S_BCLK);
    Serial.print(" LRC:");
    Serial.print(I2S_LRC);
    Serial.print(" DOUT:");
    Serial.println(I2S_DOUT);
#endif
  }
  audio.setBufsize(30000,600000);
  audio.setVolumeSteps(100);
  mydisplay.begin(&tftx);
  mydisplay.show_ip(WiFi.localIP().toString().c_str());
  rotary.begin(); //satrencoder handling
  rotary.setup(readRotaryISR); //register interrupt service routine
  rotary.setBoundaries(0, 100, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotary.setEncoderValue(cur_vol); //preset the value to current gain
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, ACTOR_START_VALUE, ACTOR_ON_VALUE, cur_vol, 1, "Lautstärke");
  webradio_on();
  mystate = true;

      Serial.println("station array");
    for (int i=0; i<MAXSTATION; i++) {
      Serial.println(station[i].name);
      Serial.println(station[i].url);
    }

}

void Webradio::html_create_json_part(String& json) {
  Switch_OnOff::html_create_json_part(json);
  json += ",\"slider1max\":100";
}

void Webradio::webradio_off() {
  write2log(LOG_SENSOR,1,"Radio off");
  audio.setVolume(0);
  audio.stopSong();
  radio_is_on = false;
  mydisplay.clear();
  mydisplay.show_time(true);
}

void Webradio::webradio_on() {
  cur_level = 0;
  radio_is_on = true;
  write2log(LOG_SENSOR,1,"Radio on");
  set_station();
  show_station();
  set_vol();
}

void Webradio::set_vol() {
  char volstr[20];
  int  bg;
  sprintf(volstr,"New Volume %u",cur_vol);
  write2log(LOG_SENSOR,1,volstr);
  if (cur_vol == 0) { 
    write2log(LOG_SENSOR,1,"Vol CMD: Radio off");
    set(obj_keyword,String("off")); 
  } else { 
    if ( ! radio_is_on) {
      write2log(LOG_SENSOR,1,"Vol CMD: Radio on");
      set(obj_keyword,String("on")); 
    }
    audio.setVolume(cur_vol);
    mydisplay.show_vol(cur_vol);
    preferences.putUChar("cur_vol", cur_vol);
  }
}

void Webradio::show_station() {
  if (cur_level == 0) {
    mydisplay.clear();
    mydisplay.show_ip(WiFi.localIP().toString().c_str());
    mydisplay.show_vol(cur_vol);
    mydisplay.show_time(false);
    mydisplay.show_station(station[cur_station].name);
  }
}

void Webradio::set_station() {
  if (cur_level == 1) {
    mydisplay.select_station(
        cur_station > 1 ? station[cur_station-2].name : "",
        cur_station > 0 ? station[cur_station-1].name : "",
        station[cur_station].name,
        cur_station < MAXSTATION ? station[cur_station+1].name : "",
        cur_station < MAXSTATION - 1 ? station[cur_station+2].name : ""
    );
  }
  if ( strlen(station[cur_station].url) > 10 ) {
    audio.connecttohost(station[cur_station].url);
    write2log(LOG_SENSOR,2,"Switch to ",station[cur_station].url);
  }
}

void Webradio::save_station() {
  File f;
  f = LittleFS.open( "/sender.txt", "r" );
  f = LittleFS.open( "/sender.txt", "w" );
  if (f) {
    for (int i=0; i<MAXSTATION; i++) {
      write2log(LOG_SENSOR,2,station[i].name,station[i].url);
      f.printf("%s\n",station[i].name);
      f.printf("%s\n",station[i].url);
    }
    delay(5);
    f.close();
  }
}

bool Webradio::set(const String& keyword, const String& value) {
  bool retval = false;
  String myvalue = value;
  replace(myvalue.begin(),myvalue.end(),'\n',' ');
// Gibt zusätzliche Hilfen für dieses Modul aus
  if ( keyword == String("?") || keyword == String("help") ) {
    cons_str = "{\"stat\":\"sender[0..9]_name=<neuer Name>\"}";
    ws.textAll(cons_str);
    cons_str = "{\"stat\":\"Gibt einen neuen Namen für die Station ein\"}";
    ws.textAll(cons_str);
    cons_str = "{\"stat\":\"sender[0..9]_url=<neue URL>\"}";
    ws.textAll(cons_str);
    cons_str = "{\"stat\":\"Gibt eine neue URL für die Station ein\"}";
    ws.textAll(cons_str);
    cons_str = "{\"stat\":\"sender[0..9] >>> Schaltet den Sender um\"}";
    ws.textAll(cons_str);
  }
// Gibt zusätzliche Settings für dieses Modul aus
  if (keyword == String("settings") ) {
    cons_str = "{\"stat\":\"Sender:\"}";
    ws.textAll(cons_str);
    File f = LittleFS.open( "/sender.txt", "r" );
    if (f) {
      Serial.print("sender.txt size: ");
      Serial.println(f.size());
      while (f.available()) {
        cons_str = "{\"stat\":\"";
        cons_str += f.readStringUntil('\n');
        cons_str += "\"}";
        write2log(LOG_SENSOR,1,cons_str.c_str());
        ws.textAll(cons_str);
      }
      f.close();
    } else {
      cons_str = "{\"stat\":\"Error opening sender.txt\"}";
      ws.textAll(cons_str);
    }
/*
  for (int i=0; i<10;i++) {
    Serial.printf("X%sX\n",station[i].name);
    Serial.printf("X%sX\n",station[i].url);
  }
*/
  }
  if (! Switch_OnOff::set(keyword, value)) {
    for (int i=0; i<MAXSTATION; i++) {
      if (!retval && keyword == String("sender"+String(i)+"_url") ) {
        write2log(LOG_SENSOR,1,String("Found: sender"+String(i)+"_url").c_str());
        snprintf(station[i].url,STATION_URL_LENGTH,"%s",myvalue.c_str());
        save_station();
        retval = true;
      }
      if (!retval && keyword == String("sender"+String(i)+"_name") ) {
        write2log(LOG_SENSOR,1,String("Found: sender"+String(i)+"_url").c_str());
        snprintf(station[i].name,STATION_NAME_LENGTH,"%s",myvalue.c_str());
        save_station();
        retval = true;
      }
      if (!retval && keyword == String("sender"+String(i))) {
        write2log(LOG_SENSOR,1,String("Found: sender"+String(i)).c_str());
        cur_station = i;
        if ( strlen(station[cur_station].url) > 10 ) {
          audio.connecttohost(station[cur_station].url);
          write2log(LOG_SENSOR,2,"Switch to ",station[cur_station].url);
        }
        retval = true;
      }
    }
  } else {
    retval = true;
  }
  return retval;
}

void Webradio::loop() {
  if (radio_is_on) {
    audio.loop();
    if (!audio.isRunning()) {
      webradio_off();
      webradio_on();
    }
  }
  if (slider_val_old != obj_slider_val ) {
    write2log(LOG_SENSOR,1,"Set Volume via Web");
    cur_vol = obj_slider_val;
    rotary.setEncoderValue(cur_vol); 
    set_vol();
    slider_val_old = obj_slider_val;
  }
  /// Loop actions for rotary encoder
  /// 1. change of position  
  if (rotary.encoderChanged()) {
    cur_position = rotary.readEncoder();
    char logstr[20];
    snprintf(logstr,19,"rotary pos: %u", cur_position);
    write2log(LOG_SENSOR,1,logstr);
    switch (cur_level) {
    case 0:
      cur_vol = cur_position;
      write2log(LOG_SENSOR,1,"Set Volume via Rotary");
      char vol_str[10];
      snprintf(vol_str,9,"S:%u",cur_position);
      set(obj_keyword,vol_str);
      break;
    case 1:
      cur_station = cur_position;
      set_station();
      break;
    }
  }
  /// 2. Button is pressed
  if (rotary.isEncoderButtonDown()) {
    if (!wasButtonDown) {
      //start measuring
      millis_Button_pressed = millis();
    }
    //else we wait since button is still down
    wasButtonDown = true;
  } else {
    //button is up
    if (wasButtonDown) {
      wasButtonDown = false;
      if (millis() - millis_Button_pressed >= LONG_PRESSED_AFTER_MS) {
//        long click => Radio off;
        set(obj_keyword,String("toggle")); 
      } else if (millis() - millis_Button_pressed >= SHORT_PRESSED_AFTER_MS) {
//      short click => Change Level
//      Store values for old level first
        level[cur_level].cur = cur_position;
//      Enter new level
        cur_level++;
        if (cur_level > MAXLEVEL-1) cur_level = 0;
//      Set Boundries for new Level
        rotary.setBoundaries(level[cur_level].min, level[cur_level].max, false);
//      Restore Position
        rotary.setEncoderValue(level[cur_level].cur);
        cur_position = level[cur_level].cur;
        last_min = -1;   
//      Do the action for this new level
        switch (cur_level) {
        case 0:
          /* Aktuelles Ratioprogramm anzeigen */
          show_station();
          break;        
        case 1:
          /* Senderwahl anzeigen */
          set_station();
          break;
        case 2:
          mydisplay.clear();

          break;
        }
      }
    }
  }
  /// End rotary

  if (obj_value != mystate) {
    mystate = obj_value;
    if (obj_value) { 
      write2log(LOG_SENSOR,1,"Web CMD: Radio on");
      webradio_on();
    } else {
      write2log(LOG_SENSOR,1,"Web CMD: Radio off");
      webradio_off();
    }
  }
  // Uhr aktualisieren wenn Minutenwechsel und Anzeige auf Radioprogramm
  if (last_min != timeinfo.tm_min) {
    if (radio_is_on) {
      if (cur_level == 0) mydisplay.show_time(false);
    } else {
      mydisplay.show_time(true);
    }
    last_min = timeinfo.tm_min;
  }
}

void audio_info(const char *info){
    write2log(LOG_SENSOR,2,"Info:", info);
}

void audio_showstreamtitle(const char *info){
  write2log(LOG_SENSOR,2,"Titel:", info);
  if (cur_level == 0) mydisplay.show_title(info);
  cons_str = "{\"sens2\":\"";
  cons_str += info;
  cons_str += "\"}";
  ws.textAll(cons_str);
}

void audio_bitrate(const char *info) {
  char bpsInfo[5];
  bpsInfo[0] = info[0];
  bpsInfo[1] = info[1];
  bpsInfo[2] = info[2];
  bpsInfo[3] = 'K';
  bpsInfo[4] = 0;
  write2log(LOG_SENSOR,2,"Bitrate:", info);
  cons_str = "{\"sens3\":\"";
  cons_str += bpsInfo;
  cons_str += "\"}";
  ws.textAll(cons_str);
}

void audio_showstation(const char *info){
  write2log(LOG_SENSOR,2,"Station:", info);
  cons_str = "{\"sens1\":\"";
  cons_str += info;
  cons_str += "\"}";
  ws.textAll(cons_str);
}


/**************************************************************************************************
 * 
 * Display
 * 
***************************************************************************************************/

void RadioDisplay::clear() {
  tft->fillScreen(GC9A01A_BLACK);
}

void RadioDisplay::show_ip(const char* myip) {
  tft->setTextColor(GC9A01A_WHITE);  
  tft->setTextSize(1);
  tft->setCursor(75, 225);
  tft->println(myip);
}

void RadioDisplay::show_vol(uint8_t vol) {
  if (vol > 90) vol=90;
  fillArc(119,119,-90,180,120,120,ARC_WIDTH,GC9A01A_BLACK);
  fillArc(119,119,-90,vol*2,120,120,ARC_WIDTH,GC9A01A_YELLOW);
}

void RadioDisplay::show_time(bool big) {
  tft->setTextColor(GC9A01A_WHITE); 
  if (big) {
    clear();
    tft->setTextSize(7);
    tft->setCursor(50,100);
  } else {
    tft->fillRect(80, 30, 90, 23, GC9A01A_BLACK);
    tft->setTextSize(3);
    tft->setCursor(80,30);
  }
  if ( timeinfo.tm_min < 10) {
    tft->printf("%d:0%d",timeinfo.tm_hour, timeinfo.tm_min);
  } else {
    tft->printf("%d:%d",timeinfo.tm_hour, timeinfo.tm_min);
  }
}

void RadioDisplay::show_station(const char* mystation) {
  tft->fillRect(25, 65, 190, 55, GC9A01A_BLACK);
  show_text(mystation, 25, 65, GC9A01A_ORANGE);
}

void RadioDisplay::show_title(const char* mytitle) {
  tft->fillRect(0, 130, 240, 60, GC9A01A_BLACK);
  show_text(mytitle, 25, 130, GC9A01A_GREEN);
}

void RadioDisplay::select_station(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4) {
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

void RadioDisplay::show_text(const char* mytext, int posx, int posy, uint16_t color) {
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

void RadioDisplay::begin(Adafruit_GC9A01A* mytft) {
  tft = mytft;
  tft->begin();
  tft->fillScreen(GC9A01A_BLACK);
}

int RadioDisplay::splitStr(const char* inStr, int startPos, int maxLen, char* resultStr) {
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

void RadioDisplay::fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour)
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
