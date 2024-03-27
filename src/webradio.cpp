#include "config.h"

#ifdef _WEBRADIO_H_
//#include "webradio.h"
#include "Audio.h"
#include "AiEsp32RotaryEncoder.h"
#include <Preferences.h>

/// Da ich die Direktive für den ESP32S3 nicht kenne, hier eine eigene:
#define DO_ESP32S3
/// Alternative auch eine eigene Direktive für den ESP32:
//#define DO_ESP32

/// @brief Eine Instanz für das Audio 
Audio audio;
/// @brief Die Variable timeinfo wird im Rumpfprogramm verwaltet, hier nur für die Anzeige der Zeit genutzt.
extern tm timeinfo;

// Definitions for ESP32 Board
#ifdef DO_ESP32
#warning "ESP32"
// I2S Settings
#define LRCLK 26
#define BCLK 27
#define DOUT 25
// Rotary Encoder Settings
#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 33
#define ROTARY_ENCODER_BUTTON_PIN 4
#define ROTARY_ENCODER_VCC_PIN -1
//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4
//TFT Settings
#define GC9A01A_TFT_DC 5
#define GC9A01A_TFT_CS 21
#define ARC_SIGMENT_DEGREES 3
#define ARC_WIDTH 5
#endif
#ifdef DO_ESP32S3
#warning "ESP32-S3"
#define I2S_DOUT     6
#define I2S_BCLK     5
#define I2S_LRC      4
#define ROTARY_ENCODER_A_PIN      37
#define ROTARY_ENCODER_B_PIN      36
#define ROTARY_ENCODER_BUTTON_PIN 35
#define ROTARY_ENCODER_VCC_PIN    -1
#define ROTARY_ENCODER_STEPS      4
#define LONG_PRESSED_AFTER_MS     1000
#define SHORT_PRESSED_AFTER_MS    20
// Display
// SCL (Display) => SCK  12
// SDA (Display) => MOSI 11
#define GC9A01A_TFT_CS        10
#define GC9A01A_TFT_DC        9
#endif


/// @brief Level definitions:
/// @brief Level beginnen bei MINLEVEL und enden bei MAXLEVEL
/// @brief 
#define MINLEVEL    1
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
// Station definitions
#define MAXSTATION            9
#define STATION_NAME_LENGTH   30
#define STATION_URL_LENGTH    128
#define ACTOR_START_VALUE     true
#define ACTOR_ON_VALUE        true

typedef struct {
    uint8_t min;
    uint8_t max;
    uint8_t cur;
} level_t;

typedef struct {
    uint8_t num;
    char    name[STATION_NAME_LENGTH];
    char    url[STATION_URL_LENGTH];
} station_t;

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
Preferences prefs;


void IRAM_ATTR readRotaryISR() {
  rotary.readEncoder_ISR();
}

void Webradio::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword) {
  // First, preallocate all the memory needed for the buffering and codecs, never to be freed
  prefs.begin("prefs",false);
  if ( !prefs.isKey("cur_vol") ) {
    prefs.putUChar("cur_vol", 5);
    prefs.putUChar("cur_station", 0);
  }
  cur_vol = prefs.getUChar("cur_vol");
  cur_level = MINLEVEL;
  cur_station = prefs.getUChar("cur_station");
  uint8_t mylevel = MINLEVEL;
  level[mylevel].min = 0;
  level[mylevel].max = 100;
  level[mylevel].cur = cur_vol;
  mylevel++;
  level[mylevel].min = 0;
  level[mylevel].max = MAXSTATION;
  level[mylevel].cur = cur_station;
  mylevel++;
  level[mylevel].min = 0;
  level[mylevel].max = 1;
  level[mylevel].cur = 0;
  if ( ! prefs.isKey("stat_0_url") ) {
    prefs.putString("stat_0_url","http://stream.lokalradio.nrw/4459m27");
    prefs.putString("stat_0_name","Radio Kiepenkerl");
  }
  if ( ! prefs.isKey("stat_1_url") ) {
    prefs.putString("stat_1_url","http://streams.radio21.de/nrw/mp3-192/web");
    prefs.putString("stat_1_name","Radio 21 NRW");
  }
  if ( ! prefs.isKey("stat_2_url") ) {
    prefs.putString("stat_2_url","http://icecast.ndr.de/ndr/njoy/live/mp3/128/stream.mp3");
    prefs.putString("stat_2_name","N-Joy");
  }
//  if ( ! prefs.isKey("stat_3_url") ) {
    prefs.putString("stat_3_url","");
    prefs.putString("stat_3_name","leer");
//  }
//  if ( ! prefs.isKey("stat_4_url") ) {
    prefs.putString("stat_4_url","");
    prefs.putString("stat_4_name","leer");
//  }
//  if ( ! prefs.isKey("stat_5_url") ) {
    prefs.putString("stat_5_url","");
    prefs.putString("stat_5_name","leer");
//  }
//  if ( ! prefs.isKey("stat_6_url") ) {
    prefs.putString("stat_6_url","");
    prefs.putString("stat_6_name","leer");
//  }
//  if ( ! prefs.isKey("stat_7_url") ) {
    prefs.putString("stat_7_url","");
    prefs.putString("stat_7_name","leer");
//  }
//  if ( ! prefs.isKey("stat_8_url") ) {
    prefs.putString("stat_8_url","");
    prefs.putString("stat_8_name","leer");
//  }
//  if ( ! prefs.isKey("stat_9_url") ) {
    prefs.putString("stat_9_url","");
    prefs.putString("stat_9_name","leer");
//  }
  prefs.getString("stat_0_url",station[0].url,STATION_URL_LENGTH);
  prefs.getString("stat_0_name",station[0].name,STATION_NAME_LENGTH);
  prefs.getString("stat_1_url",station[1].url,STATION_URL_LENGTH);
  prefs.getString("stat_1_name",station[1].name,STATION_NAME_LENGTH);
  prefs.getString("stat_2_url",station[2].url,STATION_URL_LENGTH);
  prefs.getString("stat_2_name",station[2].name,STATION_NAME_LENGTH);
  prefs.getString("stat_3_url",station[3].url,STATION_URL_LENGTH);
  prefs.getString("stat_3_name",station[3].name,STATION_NAME_LENGTH);
  prefs.getString("stat_4_url",station[4].url,STATION_URL_LENGTH);
  prefs.getString("stat_4_name",station[4].name,STATION_NAME_LENGTH);
  prefs.getString("stat_5_url",station[5].url,STATION_URL_LENGTH);
  prefs.getString("stat_5_name",station[5].name,STATION_NAME_LENGTH);
  prefs.getString("stat_6_url",station[6].url,STATION_URL_LENGTH);
  prefs.getString("stat_6_name",station[6].name,STATION_NAME_LENGTH);
  prefs.getString("stat_7_url",station[7].url,STATION_URL_LENGTH);
  prefs.getString("stat_7_name",station[7].name,STATION_NAME_LENGTH);
  prefs.getString("stat_8_url",station[8].url,STATION_URL_LENGTH);
  prefs.getString("stat_8_name",station[8].name,STATION_NAME_LENGTH);
  prefs.getString("stat_9_url",station[9].url,STATION_URL_LENGTH);
  prefs.getString("stat_9_name",station[9].name,STATION_NAME_LENGTH);
  prefs.end();
// ToDo erweitern um alle Stationen
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
  audio.setBufsize(30000,300000);
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
}

void Webradio::html_create_json_part(String& json) {
  Switch_OnOff::html_create_json_part(json);
  json += ",\"slider1max\":100";
}

void Webradio::webradio_off() {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("webadio_off");
#endif
  write2log(LOG_SENSOR,1,"Radio off");
  audio.setVolume(0);
  audio.stopSong();
  radio_is_on = false;
  mydisplay.clear();
}

void Webradio::webradio_on() {
  cur_level = MINLEVEL;
  radio_is_on = true;
  write2log(LOG_SENSOR,1,"Radio on");
  set_station();
  show_station();
  set_vol();
}

void Webradio::set_vol() {
  char volstr[20];
  int  bg;
//  cur_vol=10;
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
    mydisplay.show_vol(cur_vol);
    prefs.begin("prefs",false);
    prefs.putUChar("cur_vol", cur_vol);
    prefs.end();
  }
}

void Webradio::show_station() {
  if (cur_level == MINLEVEL) {
    mydisplay.clear();
    mydisplay.show_ip(WiFi.localIP().toString().c_str());
    mydisplay.show_vol(cur_vol);
    mydisplay.show_time();
    mydisplay.show_station(station[cur_station].name);
  }
}

void Webradio::set_station() {
  if (cur_level == MINLEVEL+1) {
    mydisplay.select_station(
        cur_station > 1 ? station[cur_station-2].name : "",
        cur_station > 0 ? station[cur_station-1].name : "",
        station[cur_station].name,
        cur_station < MAXSTATION ? station[cur_station+1].name : "s3",
        cur_station < MAXSTATION - 1 ? station[cur_station+2].name : "s4"
    );
  }
  if ( strlen(station[cur_station].url) > 10 ) audio.connecttohost(station[cur_station].url);
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
    case MINLEVEL:
      cur_vol = cur_position;
      write2log(LOG_SENSOR,1,"Set Volume via Rotary");
      char vol_str[10];
      snprintf(vol_str,9,"S:%u",cur_position);
      set(obj_keyword,vol_str);
      break;
    case MINLEVEL+1:
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
        if (cur_level > MAXLEVEL) cur_level = MINLEVEL;
//      Set Boundries for new Level
        rotary.setBoundaries(level[cur_level].min, level[cur_level].max, false);
//      Restore Position
        rotary.setEncoderValue(level[cur_level].cur);
        cur_position = level[cur_level].cur;
        last_min = -1;   
//      Do the action for this new level
        switch (cur_level) {
        case MINLEVEL:
          /* Aktuelles Ratioprogramm anzeigen */
          show_station();
          break;        
        case MINLEVEL+1:
          /* Senderwahl anzeigen */
          set_station();
          break;
        case MINLEVEL+2:
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
    if (cur_level == MINLEVEL) mydisplay.show_time();
    last_min = timeinfo.tm_min;
  }
}

void audio_info(const char *info){
    write2log(LOG_SENSOR,2,"Info:", info);
}

void audio_showstreamtitle(const char *info){
  write2log(LOG_SENSOR,2,"Titel:", info);
  if (cur_level == MINLEVEL) mydisplay.show_title(info);
}

void audio_bitrate(const char *info) {
    char bpsInfo[5];
    bpsInfo[0] = info[0];
    bpsInfo[1] = info[1];
    bpsInfo[2] = info[2];
    bpsInfo[3] = 'K';
    bpsInfo[4] = 0;
    write2log(LOG_SENSOR,2,"Bitrate:", info);
}

void audio_showstation(const char *info){
  write2log(LOG_SENSOR,2,"Station:", info);
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

void RadioDisplay::show_time() {
  tft->fillRect(80, 30, 90, 23, GC9A01A_BLACK);
  tft->setTextColor(GC9A01A_WHITE); 
  tft->setTextSize(3);
  tft->setCursor(80,30);
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
