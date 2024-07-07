#include "config.h"
#ifdef USE_AUDIOMODUL

#include "audiomodul.h"
#include "common.h"

#ifdef USE_MEDIAPLAYER
#include "FS.h"
#include "SD.h"
#endif

#ifdef USE_AUDIO_LIB
#include "Audio.h"
/// @brief Instance for audio (I2S and decoder) device
Audio            audio;
#endif

#ifdef USE_ESP8266AUDIO_LIB
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
AudioGeneratorMP3 *mp3 = NULL;
AudioFileSourceICYStream *file = NULL;
AudioFileSourceBuffer *buff = NULL;
AudioOutputI2S *out = NULL;
#endif

#if defined(CONFIG_IDF_TARGET_ESP32) 
#warning "Compiling Audiomodule with Settings for ESP32"
// Config für das Audiomodul
// Definitions for ESP32 Board
// I2S Settings
#define I2S_LRC                         26
#define I2S_BCLK                        27
#define I2S_DOUT                        25
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
#warning "Compiling Audiomodule with Settings for ESP32-S3"
//#warning "Settings for ESP32-S3"
#define I2S_DOUT                        6
#define I2S_BCLK                        5
#define I2S_LRC                         4
// Display
// SCL (Display) => SCK                 12
// SDA (Display) => MOSI                11
#define GC9A01A_TFT_CS                  8
#define GC9A01A_TFT_DC                  9
// SD Card mit HW-SPI (ok bei 3V3)
//#define SD_SCK                        12
//#define SD_MISO                       13 
//#define SD_MOSI                       11 
#define SD_CS                           10
#endif

#define ARC_SIGMENT_DEGREES             3
#define ARC_WIDTH                       5

/// @brief Instances for the Preferences is defined in main.h
extern Preferences preferences;
/// @brief Instance for websockets is defined in webserver.h
extern AsyncWebSocket ws;

/// @brief Instance for the rotary module
RotaryModul      rotarymodul;
/// @brief Instance for ohysical TFT Display
#ifdef DISPLAY_GC9A01A
Adafruit_GC9A01A tftx(GC9A01A_TFT_CS, GC9A01A_TFT_DC);
#endif
/// @brief Instance for displayhandling
AudioDisplay     audiodisplay;

void AudioModul::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword)  {
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, false, true);
  audio_radio_load_stations();
  preferences.begin("settings",false);
  if ( !preferences.isKey("audio_vol") ) {
    preferences.putUChar("audio_vol", 0);
    preferences.putUChar("audio_tre", 0);
    preferences.putUChar("audio_bas", 0);
    preferences.putUChar("ar_cur_station", 0);
  }
  audio_radio_cur_station = preferences.getUChar("ar_cur_station");
  audio_vol = 0; //preferences.getUChar("audio_vol");
  audio_tre = preferences.getUChar("audio_tre");
  audio_bas = preferences.getUChar("audio_bas");
  preferences.end();
  audiodisplay.begin(&tftx);
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  rotarymodul.begin();
  rotarymodul.initLevel(0,0,audio_vol,100);
  rotarymodul.initLevel(1,0,audio_radio_cur_station,10);
  rotarymodul.initLevel(2,0,0,2);
#if defined(USE_AUDIO_LIB)
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
#elif defined(USE_ESP8266AUDIO_LIB)
//  if (out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT))  {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.print("Set PinOut: BCLK:");
    Serial.print(I2S_BCLK);
    Serial.print(" LRC:");
    Serial.print(I2S_LRC);
    Serial.print(" DOUT:");
    Serial.println(I2S_DOUT);
#endif
//  }
#endif
#if defined(USE_AUDIO_LIB)
  //audio.setBufsize(30000,600000);
  audio.setVolumeSteps(100);
  audio.setVolume(audio_vol);
#endif

    if(!SD.begin(SD_CS)){
        Serial.println("Card Mount Failed");
       // return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);


    File root = SD.open("/");
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println (file.name());
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
    file.close();
    root.close();

  set_modus(Off);
}

void AudioModul::html_create_json_part(String& json) {
  Switch_OnOff::html_create_json_part(json);
  json += ",\"audio_show\":1";
  json += ",\"audio_radio\":";
  if (obj_value) {
    json += (modus == Radio)?1:0;
  } else {
    json += "1";
  }
  json += ",\"audio_media\":";
  json += (modus == Media)?1:0;
  json += ",\"audio_speak\":";
  json += (modus == Speaker)?1:0;
  json += ",\"audio_vol\":";
  json += audio_vol;
  json += ",\"audio_bas\":";
  json += audio_bas;
  json += ",\"audio_tre\":";
  json += audio_tre;
  audio_radio_send_stn2web();
}

bool AudioModul::set(const String& keyword, const String& value) {
  bool retval = false;
  String myvalue = value;
//  Serial.print("Audiomodul::set: ");
//  Serial.print(keyword);
//  Serial.println();
  if ( ! Switch_OnOff::set(keyword, value) ) {
//    Serial.println("Switch_OnOff::set war false!");
    std::replace(myvalue.begin(),myvalue.end(),'\n',' ');
    if ( keyword == String("audio_vol") ) {
      audio_vol = value.toInt();
      html_json = "{\"audio_vol\":";
      html_json += audio_vol;
      html_json += "}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
#if defined(USE_AUDIO_LIB)
      audio.setVolume(audio_vol);
#elif defined(USE_ESP8266AUDIO_LIB)
      out->SetGain(((float)audio_vol)/100.0);
#endif
      audiodisplay.show_vol(audio_vol);
      retval = true;
    }
    if ( keyword == String("audio_bas") ) {
      // Bass kann nur über die Weboberfläche eingestellt werden
      audio_bas = value.toInt();
      html_json = "{\"audio_bas\":";
      html_json += audio_bas;
      html_json += "}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
#if defined(USE_AUDIO_LIB)
      audio.setTone((int8_t)-40+audio_bas,0,(int8_t)-40+audio_tre);
#elif defined(USE_ESP8266AUDIO_LIB)
//      out->SetGain(((float)audio_vol)/100.0);
#endif
      retval = true;
    }
    if ( keyword == String("audio_tre") ) {
      // Höhen können nur über die Weboberfläche eingestellt werden
      audio_tre = value.toInt();
      html_json = "{\"audio_tre\":";
      html_json += audio_tre;
      html_json += "}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
#if defined(USE_AUDIO_LIB)
      audio.setTone((int8_t)-40+audio_bas,0,(int8_t)-40+audio_tre);
#elif defined(USE_ESP8266AUDIO_LIB)
//      out->SetGain(((float)audio_vol)/100.0);
#endif
      retval = true;
    }
    // Set for radio
    // Radio einschalten
    if ( keyword == String(AUDIO_RADIO) || keyword == String("audio_radio") ) {
      set_modus(Radio);
      rotarymodul.initLevel(1,0,audio_radio_cur_station,MAXSTATIONS);
      html_json = "{\"audio_radio\":\"1\"}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      retval = true;
    }
    // Radio: Sender einstellen hier ueber command "station 2"
    if ( keyword == String(AUDIO_RADIO_SET_STATION) ) {
      uint8_t newstation = value.toInt();
      if ( newstation < MAXSTATIONS) audio_radio_cur_station = newstation;
      retval = true;
    }
    // Radio: Sender einstellen ueber Webinterface
    if ( keyword == String("audio_radio_set_stn") ) {
      for (int i=0; i<MAXSTATIONS; i++) {
        if (String(station[i].name) == value) audio_radio_cur_station = i;  
      }
      audio_radio_on();
    }
    // Radio: Sender Name speichern ueber Webinterface
    if ( keyword == String("audio_radio_save_stn_name") ) {
      snprintf(station[audio_radio_cur_station].name,STATION_NAME_LENGTH,"%s",value.c_str());
      audio_radio_save_stations();
      audio_radio_send_stn2web();
    }
    // Radio: Sender URL speichern ueber Webinterface
    if ( keyword == String("audio_radio_save_stn_url") ) {
      snprintf(station[audio_radio_cur_station].url,STATION_URL_LENGTH,"%s",value.c_str());
      audio_radio_save_stations();
      audio_radio_send_stn2web();
    }
    // Set for media
    if ( keyword == String(AUDIO_MEDIA) || keyword == String("audio_media") ) {
      set_modus(Media);
      html_json = "{\"audio_media\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      rotarymodul.initLevel(1,0,audio_media_cur_dir,MAXALBUM);
      audio_media_show();
      // TODO: Media einschalten, alles andere aus
      retval = true;
    }
    // Set for speaker
    if ( keyword == String(AUDIO_SPEAKER) || keyword == String("audio_speak") ) {
      set_modus(Speaker);
      html_json = "{\"audio_speak\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      audio_speak_show();
      // TODO: Speaker einschalten, alles andere aus
      retval = true;
    }

  } else {
    retval = true;
  }
  return retval;
}

void AudioModul::set_modus(modus_t _modus) {
  switch (_modus) {
    case Radio:
      modus = Radio;
      audio_media_off();
      audio_speak_off();
      audio_radio_on();
    break;
    case Media:
      modus = Media;
      audio_radio_off();
      audio_speak_off();
      audio_media_on();
    break;
    case Speaker:
      modus = Speaker;
      audio_radio_off();
      audio_media_off();
      audio_speak_on();
    break;
    default:
      modus = Off;
      audio_radio_off();
      set_switch(0);
      audiodisplay.show_time(true);
    break;
  }
}

void AudioModul::loop(time_t now) {
  // Jede Minute benötigt die Uhrzeit ein Update
  if (timeinfo.tm_min != old_min) {
    switch(modus) {
      case Radio:
         if ( rotarymodul.curLevel() == 0 ) audiodisplay.show_time(false);
      break;
      case Media:
         if ( rotarymodul.curLevel() == 0 ) audiodisplay.show_time(false);
      break;
      case Speaker:
         if ( rotarymodul.curLevel() == 0 ) audiodisplay.show_time(false);
      break;
      default:
        audiodisplay.show_time(true);
      break;
    }
    old_min = timeinfo.tm_min;
  }
  if (modus != Off) {
#if defined(USE_AUDIO_LIB)
    audio.loop();
    if (!audio.isRunning()) {
      if (modus == Radio) {
        audio_radio_off();
        audio_radio_on();
      }
    }
#elif defined(USE_ESP8266AUDIO_LIB)
    if (!mp3->loop()) mp3->stop();
    // ToDo wieder neu starten
#endif
  }
  rotarymodul.loop(now);
  switch (rotarymodul.changed()) {
    case 1: {
      // Der Rotary wurde gedreht
      if ( rotarymodul.curLevel() == 0 ) {
        audio_vol = rotarymodul.curValue();
        set("audio_vol",String(audio_vol));
        if ( ! obj_value && (audio_vol > 0) ) {
          set_switch(1);
          if ( modus == Radio ) audio_radio_show();
        }
        if ( audio_vol == 0 ) {
          set_modus(Off);
        } else {
          if ( modus == Off) set_modus(Radio);
        }
      }
      // Level 1
      if ( rotarymodul.curLevel() == 1 ) {
        if (modus == Radio) {
          // Level 1 Modus Radio wählt den Sender
          audio_radio_cur_station = rotarymodul.curValue();
          audio_radio_set_station();
        }
        if (modus == Media) {
          audio_media_cur_dir = rotarymodul.curValue();
          audio_media_select();
        }
      }
      // Level 2 wählt den Modus (Radio, Mediaplayer, Bluetoothspeaker)
      if ( rotarymodul.curLevel() == 2 ) {
        switch(rotarymodul.curValue()) {
          case 1:
              set(AUDIO_MEDIA,"1");
          break;
          case 2:
              set(AUDIO_SPEAKER,"1");
          break;
          default:
              set(AUDIO_RADIO,"1");
          break;
        }
        Serial.print("Loop Rot-Level:2 modus=");
        switch(modus) {
          case Radio:
            Serial.println("Radio");
          break;
          case Media:
            Serial.println("Media");
          break;
          case Speaker:
            Serial.println("Speaker");
          break;
          default:
            Serial.println("Off");
          break;
        }
        audiodisplay.show_modus(modus);
      }
    }
    break;
    case 2: {
      // Der Rotary wurde kurz gedrueckt
      if (obj_value) {
        Serial.printf("Rotary Level %u Position: %u Slider: %u\n", rotarymodul.curLevel(), rotarymodul.curValue(), audio_vol);
        switch(rotarymodul.curLevel()) {
          case 0:
            switch (modus) {
              case Radio:
                audio_radio_show();
              break;
              case Media:
                audio_media_show();
              break;
              case Speaker:
                audio_speak_show();
              break;
            }  
          break;
          case 1:
            switch (modus) {
              case Radio:
                audio_radio_select();
              break;
              case Media:
                audio_media_select();
              break;
              case Speaker:
                audio_speak_show();
              break;
            }  
          break;
          case 2:
            audiodisplay.show_modus(modus);
          break;
        }
      } else {
        rotarymodul.setLevel(0);
      }
    }
    break;
    case 3: { 
      // Der Rotary wurde lang gedrueckt
      set_switch(2);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(0);
      set("audio_vol",String(0));
    }
    break;
  }
  if ( changed() ) {
//    Serial.printf("Web Slider Position: %u \n", get_slider_val());
    if (modus != Off && ! obj_value) {
      set_modus(Off);
    }
    if ( modus == Off && obj_value) {
      if ( modus == Off) set_modus(Radio);
    }
    if ( rotarymodul.curLevel() == 0 ) {
      if (modus != Off) rotarymodul.setValue( audio_vol );  
      Serial.printf("Rotary Level %u Position: %u Slider: %u\n", rotarymodul.curLevel(), rotarymodul.curValue(), audio_vol);
    }
  }
}

/*********************************************************************************************************
 * 
 *  Ab hier alles fürs Radio
 * 
 * 
**********************************************************************************************************/

void AudioModul::audio_radio_off() {
  write2log(LOG_MODULE,1,"Radio off");
#if defined(USE_AUDIO_LIB)
  audio.stopSong();
#elif defined(USE_ESP8266AUDIO_LIB)
  if (mp3) {
    mp3->stop();
    delete mp3;
    mp3 = NULL;
  }
  if (buff) {
    buff->close();
    delete buff;
    buff = NULL;
  }
  if (file) {
    file->close();
    delete file;
    file = NULL;
  }
#endif
}

void AudioModul::audio_radio_on() {
  write2log(LOG_MODULE,1,"Radio on");
  if ( strlen(station[audio_radio_cur_station].url) > 10 ) {
#if defined(USE_AUDIO_LIB)
    audio.connecttohost(station[audio_radio_cur_station].url);
#elif defined(USE_ESP8266AUDIO_LIB)
    file = new AudioFileSourceICYStream(station[audio_radio_cur_station].url);
//  file->RegisterMetadataCB(MDCallback, (void*)"ICY");
    buff = new AudioFileSourceBuffer(file, 2048);
//  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
    out = new AudioOutputI2S();
    out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    out->SetGain(((float)audio_vol)/100.0);
    mp3 = new AudioGeneratorMP3();
//  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
    mp3->begin(buff, out);
#endif
    write2log(LOG_MODULE,2,"Switch to ",station[audio_radio_cur_station].url);
    if (rotarymodul.curLevel() == 0) audio_radio_show();
  }
}
/*
void AudioModul::audio_radio_station_json(String& json) {
  for (int i=0; i<MAXSTATIONS; i++) {
    json += "{\"audio_radio_add_stn\":\"";
    json += station[i].url;
    json += ";";
    json += station[i].name;
    json += "\"}";
  }
}
*/
void AudioModul::audio_radio_show() {
  audiodisplay.clear();
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  audiodisplay.show_vol(audio_vol);
  audiodisplay.show_time(false);
  audiodisplay.show_info1(station[audio_radio_cur_station].name);
}

void AudioModul::audio_radio_select() {
  audiodisplay.select(
    audio_radio_cur_station > 1 ? station[audio_radio_cur_station-2].name : "",
    audio_radio_cur_station > 0 ? station[audio_radio_cur_station-1].name : "",
    station[audio_radio_cur_station].name,
    audio_radio_cur_station < MAXSTATIONS ? station[audio_radio_cur_station+1].name : "",
    audio_radio_cur_station < MAXSTATIONS - 1 ? station[audio_radio_cur_station+2].name : ""
  );
}

void AudioModul::audio_radio_set_station() {
  audio_radio_select();
  if ( strlen(station[audio_radio_cur_station].url) > 10 ) {
#if defined(USE_AUDIO_LIB)
    audio.connecttohost(station[audio_radio_cur_station].url);
#elif defined(USE_ESP8266AUDIO_LIB)
//  file = new AudioFileSourceICYStream(station[audio_radio_cur_station].url);
//  file->RegisterMetadataCB(MDCallback, (void*)"ICY");
//  buff = new AudioFileSourceBuffer(file, 2048);
//  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
//  out = new AudioOutputI2S();
//  out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
//  out->SetGain(((float)audio_vol)/100.0);
//  mp3 = new AudioGeneratorMP3();
//  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
//  mp3->begin(buff, out);
#endif
    write2log(LOG_MODULE,2,"Switch to ",station[audio_radio_cur_station].url);
  }
}

void AudioModul::audio_radio_load_stations() {
  File f = LittleFS.open( "/sender.txt", "r" );
  if (f) {
    for (int i=0; i<MAXSTATIONS; i++) {
      snprintf(station[i].name,STATION_NAME_LENGTH,"%s",f.readStringUntil('\n').c_str());
      snprintf(station[i].url,STATION_URL_LENGTH,"%s",f.readStringUntil('\n').c_str());
      write2log(LOG_MODULE,2,station[i].name,station[i].url);
    }
    f.close();
  }
}

void AudioModul::audio_radio_save_stations() {
  File f = LittleFS.open( "/sender.txt", "w" );
  if (f) {
    for (int i=0; i<MAXSTATIONS; i++) {
      write2log(LOG_MODULE,3,"Save Station: ",station[i].name,station[i].url);
      f.printf("%s\n",station[i].name);
      f.printf("%s\n",station[i].url);
    }
    delay(5);
    f.close();
  }
}

void AudioModul::audio_radio_send_stn2web() {
  for (int i=0; i<MAXSTATIONS; i++) {
    String html_json = "{\"audio_radio_add_stn\":\"";
    html_json += station[i].url;
    html_json += ";";
    html_json += station[i].name;
    html_json += "\"}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
  }
}

//
// Die folgenden Funktionen ergänzen die Lib: ESP32-audioI2S
// Die Funktionsnamen sind dort festgelegt
//

void audio_info(const char *info){
    write2log(LOG_MODULE,2,"Info:", info);
}

void audio_showstreamtitle(const char *info){
    write2log(LOG_MODULE,2,"Titel:", info);
    if (rotarymodul.curLevel() == 0) audiodisplay.show_info2(info);
    html_json = "{\"audiomsg2\":\"";
    html_json += info;
    html_json += "\"}";
    write2log(LOG_WEB,1,html_json.c_str());
    ws.textAll(html_json);
}

void audio_bitrate(const char *info) {
    char bpsInfo[8];
    bpsInfo[0] = info[0];
    bpsInfo[1] = info[1];
    bpsInfo[2] = info[2];
    bpsInfo[3] = 'K';
    bpsInfo[4] = 'B';
    bpsInfo[5] = 'p';
    bpsInfo[6] = 's';
    bpsInfo[7] = 0;
    write2log(LOG_MODULE,2,"Bitrate:", info);
    if (rotarymodul.curLevel() == 0) audiodisplay.show_bps(bpsInfo);
    html_json = "{\"audiomsg4\":\"";
    html_json += bpsInfo;
    html_json += "\"}";
    write2log(LOG_WEB,1,html_json.c_str());
    ws.textAll(html_json);
}

void audio_showstation(const char *info){
    write2log(LOG_MODULE,2,"Station:", info);
    html_json = "{\"audiomsg1\":\"";
    html_json += info;
    html_json += "\"}";
    write2log(LOG_WEB,1,html_json.c_str());
    ws.textAll(html_json);
}

/*********************************************************************************************************
 * 
 *  Ab hier alles für den Mediaplayer
 * 
 * 
**********************************************************************************************************/

void AudioModul::audio_media_on() {
  if (rotarymodul.curLevel() == 0) audio_media_show();
}

void AudioModul::audio_media_off() {
}

void AudioModul::audio_media_select() {
Serial.println("x1");
  String dir[5];
  uint8_t tmp_dir = 0;
  if (audio_media_cur_dir == 0) { dir[0]=""; dir[1]=""; tmp_dir=2; }
  if (audio_media_cur_dir == 1) { dir[0]=""; tmp_dir=1; }
Serial.println("x2");
  File root = SD.open("/");
  if(root.isDirectory() ){
    File file = root.openNextFile();
    while(file && tmp_dir <5) {
      if(file.isDirectory()){
Serial.println("x3");
        dir[tmp_dir] = file.name();
        file = root.openNextFile();
        tmp_dir++;
      }
    }
Serial.println("x4");
    while (tmp_dir < 5) {
      dir[tmp_dir]="";
      tmp_dir++;
    }
    file.close();
  }
  root.close();
Serial.println("x5");
  audiodisplay.select(dir[0].c_str(),dir[1].c_str(),dir[2].c_str(),dir[3].c_str(),dir[4].c_str());
}

void AudioModul::audio_media_show() {
  Serial.println("In audio_media_show");
  audiodisplay.clear();
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  audiodisplay.show_vol(audio_vol);
  audiodisplay.show_time(false);
  audiodisplay.show_info1("Mediaplayer");
}

/*********************************************************************************************************
 * 
 *  Ab hier alles für den Bluetoothspeaker
 * 
 * 
**********************************************************************************************************/

void AudioModul::audio_speak_on() {
  if (rotarymodul.curLevel() == 0) audio_speak_show();
}

void AudioModul::audio_speak_off() {
}

void AudioModul::audio_speak_show() {
  audiodisplay.clear();
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  audiodisplay.show_vol(audio_vol);
  audiodisplay.show_time(false);
  audiodisplay.show_info1("Bluetooth Player");
}


#endif