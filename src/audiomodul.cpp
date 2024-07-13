#include "config.h"
#ifdef USE_AUDIOMODUL

#include "audiomodul.h"
#include "common.h"

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
// SD Card mit HW-SPI (ok bei 3V3 Adaptern)
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

/// @brief Variable zur Steuerung der Anzeige Steaminhalte im Display
bool radioPlayMode = false;
bool mediaPlayMode = false;


void AudioModul::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword)  {
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, false, true);
#ifdef USE_AUDIO_RADIO
  audio_radio_load_stations();
#endif
  preferences.begin("settings",false);
  if ( !preferences.isKey("audio_vol") ) {
    preferences.putUChar("audio_vol", 0);
    preferences.putUChar("audio_tre", 0);
    preferences.putUChar("audio_bas", 0);
#ifdef USE_AUDIO_RADIO
    preferences.putUChar("ar_cur_station", 0);
#endif
  }
#ifdef USE_AUDIO_RADIO
  audio_radio_cur_station = preferences.getUChar("ar_cur_station");
#endif
  audio_vol = 0; //preferences.getUChar("audio_vol");
  audio_tre = preferences.getUChar("audio_tre");
  audio_bas = preferences.getUChar("audio_bas");
  preferences.end();
  audiodisplay.begin(&tftx);
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  rotarymodul.begin();
//  rotarymodul.initLevel(0,0,audio_vol,100);
//  rotarymodul.initLevel(1,0,audio_radio_cur_station,10);
//  rotarymodul.initLevel(2,0,0,2);
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
#ifdef USE_AUDIO_MEDIA
  SD.begin(SD_CS);
  audio_media_num_dir = 0;
  File root = SD.open("/");
  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()) audio_media_num_dir++;
    file = root.openNextFile();
  }
  file.close();
  root.close();
  audio_media_cur_dir = 1;
  audio_media_cur_file = 1;
#endif
  audio_set_modus(Off);
  last_modus = Radio;
}

void AudioModul::html_create_json_part(String& json) {
  Switch_OnOff::html_create_json_part(json);
  json += ",\"audio_show\":1";
#ifdef USE_AUDIO_RADIO  
  json += ",\"audio_radio_show\":1";
  json += ",\"audio_radio\":";
  if (obj_value) {
    json += (modus == Radio)?1:0;
  } else {
    json += "1";
  }
#endif
#ifdef USE_AUDIO_MEDIA
  json += ",\"audio_media_show\":1";
  json += ",\"audio_media\":";
  json += (modus == Media)?1:0;
#endif
#ifdef USE_AUDIO_SPEAKER
  json += ",\"audio_speak_show\":1";
  json += ",\"audio_speak\":";
  json += (modus == Speaker)?1:0;
#endif
  json += ",\"audio_vol\":";
  json += audio_vol;
  json += ",\"audio_bas\":";
  json += audio_bas;
  json += ",\"audio_tre\":";
  json += audio_tre;
#ifdef USE_AUDIO_RADIO
  audio_radio_send_stn2web();
#endif
}

bool AudioModul::set(const String& keyword, const String& value) {
  bool retval = false;
  String myvalue = value;
  write2log(LOG_MODULE,2,keyword.c_str(),value.c_str());
  if ( ! Switch_OnOff::set(keyword, value) ) {
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
      if (audio_vol == 0) {
        audio_set_modus(Off);
      }
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
#ifdef USE_AUDIO_RADIO
    // Set for radio
    // Radio einschalten
    if ( keyword == String(AUDIO_RADIO) || keyword == String("audio_radio") ) {
      audio_set_modus(Radio);
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
#endif
#ifdef USE_AUDIO_MEDIA
    // Set for media
    if ( keyword == String(AUDIO_MEDIA) || keyword == String("audio_media") ) {
      audio_set_modus(Media);
      html_json = "{\"audio_media\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      retval = true;
    }
#endif
#ifdef USE_AUDIO_SPEAKER
    // Set for speaker
    if ( keyword == String(AUDIO_SPEAKER) || keyword == String("audio_speak") ) {
      audio_set_modus(Speaker);
      html_json = "{\"audio_speak\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      audio_speak_show();
      // TODO: Speaker einschalten, alles andere aus
      retval = true;
    }
#endif
  } else {
    // Ein- und Ausschalten erfolgt im Elternobjekt
    if ((! obj_value) && (modus != Off)) audio_set_modus(Off);
    if (( obj_value) && (modus == Off)) audio_set_modus(last_modus);
    retval = true;
  }
  return retval;
}

void AudioModul::audio_all_apps_off() {
#ifdef USE_AUDIO_RADIO
      audio_radio_off();
#endif
#ifdef USE_AUDIO_MEDIA
      audio_media_off();
#endif
#ifdef USE_AUDIO_SPEAKER
      audio_speak_off();
#endif
}

void AudioModul::audio_set_modus(modus_t _modus) {
  switch (_modus) {
    case Off:
      Serial.println("audio_set_modus: case Off");
      rotarymodul.setMaxLevel(0);
      rotarymodul.initLevel(0, 0, 0, 100);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(0);
      modus = Off;
      audio_all_apps_off();
      audiodisplay.show_time(true);
    break;
    case Select:
      Serial.println("audio_set_modus: case Select");
      rotarymodul.setMaxLevel(1);
      rotarymodul.initLevel(1, 0, 0, 100);
      rotarymodul.initLevel(0, 0, (uint8_t)modus, modus_count-1);
      rotarymodul.setLevel(0);
      rotarymodul.setValue((uint8_t)last_modus);
      modus = Select;
      audio_show_modus(last_modus);      
    break;
#ifdef USE_AUDIO_RADIO
    case Radio:
      Serial.println("audio_set_modus: case Radio");
      rotarymodul.setMaxLevel(1);
      rotarymodul.initLevel(0, 0, audio_vol, 100);
      rotarymodul.initLevel(1, 0, audio_radio_cur_station, MAXSTATIONS-1);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(audio_vol);
      modus = Radio;
      last_modus = Radio;
      audio_all_apps_off();
      audio_radio_on();
    break;
#endif
#ifdef USE_AUDIO_MEDIA
    case Media:
      Serial.println("audio_set_modus: case Media");
      rotarymodul.setMaxLevel(2);
      rotarymodul.initLevel(0, 0, audio_vol, 100);
      rotarymodul.initLevel(1, 0, audio_media_cur_dir, audio_media_num_dir-1);
      rotarymodul.initLevel(2, 0, audio_media_cur_file, 100);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(audio_vol);
      modus = Media;
      last_modus = Media;
      audio_all_apps_off();
      audio_media_on();
    break;
#endif
#ifdef USE_AUDIO_SPEAKER
    case Speaker:
      Serial.println("audio_set_modus: case Speaker");
      modus = Speaker;
      last_modus = Speaker;
      all_apps_off();
      audio_speak_on();
    break;
#endif
    default:
      Serial.println("audio_set_modus: case default");
      modus = Off;
      audio_all_apps_off();
      set_switch(0);
      audiodisplay.show_time(true);
    break;
  }
}

void AudioModul::audio_show_modus(modus_t _modus) {
  if (_modus == Off) audiodisplay.show_modus("Off");
  if (_modus == Settings) audiodisplay.show_modus("Settings");
#ifdef USE_AUDIO_RADIO
  if (_modus == Radio) audiodisplay.show_modus("Radio");
#endif
#ifdef USE_AUDIO_MEDIA
  if (_modus == Media) audiodisplay.show_modus("Media");
#endif
#ifdef USE_AUDIO_SPEAKER
  if (_modus == Speaker) audiodisplay.show_modus("Speaker");
#endif
}

void AudioModul::loop(time_t now) {
  // Jede Minute benötigt die Uhrzeit ein Update
  if (timeinfo.tm_min != old_min) {
    time_update = true;
    old_min = timeinfo.tm_min;
  } else {
    time_update = false;
  }
#ifdef USE_AUDIO_RADIO
  radioPlayMode = (modus == Radio) && (rotarymodul.curLevel() == 0);
#endif
#ifdef USE_AUDIO_MEDIA
  mediaPlayMode = (modus == Media) && (rotarymodul.curLevel() == 0);
#endif

  if (modus != Off) {
#if defined(USE_AUDIO_LIB)
    audio.loop();
    if (!audio.isRunning()) {
#ifdef USE_AUDIO_RADIO
      if (modus == Radio) {
        audio_radio_off();
        audio_radio_on();
      }
#endif
#ifdef USE_AUDIO_MEDIA
      if (modus == Media) {
        if (audio_media_cur_file < audio_media_num_file) {
          audio_media_cur_file++;
        } else {
          audio_media_cur_file = 0;
        }
        audio_media_play(audio_media_cur_dir, audio_media_cur_file);
      }
#endif
    }
#elif defined(USE_ESP8266AUDIO_LIB)
    if (!mp3->loop()) mp3->stop();
    // ToDo wieder neu starten
#endif
  }
  rotarymodul.loop(now);
// Hier wird der Klickstream definiert
  switch(modus) {
      case Off:
      if ( time_update ) audiodisplay.show_time(true);
      switch (rotarymodul.changed()) {
        case 1:
        // Der Rotary wurde gedreht => Anwendung ändern
        if (rotarymodul.curValue() > 5) {
          // Anlage einschalten
          audio_set_modus(last_modus);
        }
        break;
        case 2:
        // Der Rotary wurde kurz gedrückt => Anwendung bestätigen
        // Keine Aktion
        break;
        case 3:
        // Der Rotary wurde lang gedrückt => Anwendungsauswahl starten
        // Keine Aktion
        break;
      }
    break;
#ifdef USE_AUDIO_RADIO
    case Radio:
      if ( time_update && rotarymodul.curLevel() == 0) audiodisplay.show_time(false);
      if ( timeout_set && (now - timeout_start) > TIMEOUT) {
        rotarymodul.setLevel(0);
        rotarymodul.setIsChanged(2);
        timeout_set = false;
      }
      switch (rotarymodul.changed()) {
        case 1:
        // Der Rotary wurde gedreht
          switch(rotarymodul.curLevel()) {
            case 0:
            // Lautstärke ändern
              audio_vol = rotarymodul.curValue();
              set("audio_vol",String(audio_vol));
            break;
            case 1:
            // Senderwahl
              audio_radio_cur_station = rotarymodul.curValue();
              audio_radio_set_station();
              timeout_start = now;
            break;
          }
        break;
        case 2:
        // Der Rotary wurde kurz gedrückt
          switch(rotarymodul.curLevel()) {
            case 0:
            // Modus: Ausgewählten Sender anzeigen
              audio_radio_show();
            break;
            case 1:
            // Senderwahl anzeigen
              audio_radio_select();
              timeout_start = now;
              timeout_set = true;
            break;
          }
        break;
        case 3:
        // Der Rotary wurde lang gedrückt
        // Anwendungswahl todo
        audio_set_modus(Select);
        timeout_start = now;
        timeout_set = true;
        break;
      }
    break;
#endif
#ifdef USE_AUDIO_MEDIA
    case Media:
      if ( time_update && rotarymodul.curLevel() == 0) audiodisplay.show_time(false);
      if ( timeout_set && now - timeout_start > TIMEOUT) {
        rotarymodul.setLevel(0);
        timeout_set = false;
      }
      switch (rotarymodul.changed()) {
        case 1:
        // Der Rotary wurde gedreht
          switch(rotarymodul.curLevel()) {
            case 0:
            // Lautstärke ändern
              audio_vol = rotarymodul.curValue();
              set("audio_vol",String(audio_vol));
            break;
            case 1:
            // Albumauswahl
              audio_media_cur_dir = rotarymodul.curValue();
              audio_media_select_dir();
            break;
            case 2:
            // Songauswahl
              audio_media_cur_file = rotarymodul.curValue();
              audio_media_select_file();
            break;
          }
        break;
        case 2:
        // Der Rotary wurde kurz gedrückt
          switch(rotarymodul.curLevel()) {
            case 0:
            // Wiedergabe aktuellen Song anzeigen
              audio_media_play(audio_media_cur_dir,audio_media_cur_file);
              audio_media_show();
            break;
            case 1:
            // Albumauswahl initiieren und anzeigen
              audio_media_select_dir();
            break;
            case 2:
            // Songauswahl initiieren und anzeigen
              audio_media_select_file();
            break;
          }

        break;
        case 3:
        // Der Rotary wurde lang gedrückt => Anwendungsauswahl starten
          audio_set_modus(Select);
          timeout_start = now;
          timeout_set = true;
        break;
      }
    break;
#endif
#ifdef USE_AUDIO_SPEAKER
    case Speaker:
      if ( time_update && rotarymodul.curLevel() == 0) audiodisplay.show_time(false);
      switch (rotarymodul.changed()) {
        case 1:
        // Der Rotary wurde gedreht => Anwendung ändern

        break;
        case 2:
        // Der Rotary wurde kurz gedrückt => Anwendung bestätigen

        break;
        case 3:
        // Der Rotary wurde lang gedrückt => Anwendungsauswahl starten


      }
    break;
#endif
    case Select:
    // Applikation auswählen
      if ( timeout_set && now - timeout_start > TIMEOUT) {
        audio_set_modus(last_modus);
        timeout_set = false;
      }
      switch (rotarymodul.changed()) {
        case 1:
        // Der Rotary wurde gedreht => Anwendung ändern
          Serial.print("Loop Select case 1: ");
          Serial.println(rotarymodul.curValue());
//          rotarymodul.initLevel(1,0,rotarymodul.curValue(),modus_count-1);
          audio_show_modus((modus_t)rotarymodul.curValue());
        break;
        case 2:
        // Der Rotary wurde kurz gedrückt => Anwendung bestätigen
          Serial.print("Loop Select case 2: ");
          Serial.println(rotarymodul.curValue());
          audio_set_modus((modus_t)rotarymodul.curValue(0));
        break;
        case 3:
        // Der Rotary wurde lang gedrückt
        // Keine Aktion
        break;
      }
    break;
    case Settings:
    // Zusätzliche Einstellungen: Bass, Höhen 
      if ( timeout_set && now - timeout_start > TIMEOUT) {
        audio_set_modus(last_modus);
        timeout_set = false;
      }
      switch (rotarymodul.changed()) {
        case 1:
        // Der Rotary wurde gedreht => Anwendung ändern

        break;
        case 2:
        // Der Rotary wurde kurz gedrückt => Anwendung bestätigen

        break;
        case 3:
        // Der Rotary wurde lang gedrückt

        break;
      }
    break;
  }
// Ende Klickstream Definition  
}
/************************************************************************************
// Die folgenden Funktionen ergänzen die Lib: ESP32-audioI2S
// Die Funktionsnamen sind dort festgelegt
************************************************************************************/

void audio_info(const char *info){
    write2log(LOG_MODULE,2,"Info:", info);
}

void audio_id3data(const char *info){
    write2log(LOG_MODULE,2,"MP3 data:", info);
    if (mediaPlayMode) audiodisplay.show_info2(info);   
}

void audio_showstreamtitle(const char *info){
    write2log(LOG_MODULE,2,"Titel:", info);
    if (radioPlayMode) audiodisplay.show_info2(info);
    html_json = "{\"audiomsg2\":\"";
    html_json += info;
    html_json += "\"}";
    write2log(LOG_MODULE,1,html_json.c_str());
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
    if (radioPlayMode || mediaPlayMode) audiodisplay.show_bps(bpsInfo);
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
 *  Ab hier alles fürs Radio
 * 
 * 
**********************************************************************************************************/
#ifdef USE_AUDIO_RADIO

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

#endif
/*********************************************************************************************************
 * 
 *  Ab hier alles für den Mediaplayer
 * 
 * 
**********************************************************************************************************/
#ifdef USE_AUDIO_MEDIA

void AudioModul::audio_media_on() {
  audio_media_play(audio_media_cur_dir,audio_media_cur_file);
  if (rotarymodul.curLevel() == 0) audio_media_show();
}

void AudioModul::audio_media_off() {
}

void AudioModul::audio_media_play(uint8_t _dirNo, uint8_t _fileNo) {
  String fileName;
  uint8_t tmp_dir;
  uint8_t tmp_file;
  File root;
  File dir;
  File file;
  root = SD.open("/");
  if (root) {
    for (int i=0; i<=_dirNo; i++) dir = root.openNextFile();
    if (dir) {
      fileName = String(dir.name());
      for (int i=0; i<=_fileNo; i++) file = dir.openNextFile();
      if (file) {
        fileName += String("/") + String(file.name());
      }
      file.close();
    }
    dir.close();
  }
  root.close();
  audiodisplay.show_info1(fileName.c_str());
  audio.connecttoFS(SD,fileName.c_str());
}

void AudioModul::audio_media_select_dir() {
  Serial.print("audio_media_select_dir => cur_dir: ");
  Serial.println(audio_media_cur_dir);
  String obj[5];
  for(int i=0;i<5;i++) obj[i]="";
  File root;
  File dir;
  root = SD.open("/");
  Serial.print("Anzahl Dirs: ");
  Serial.println(audio_media_num_dir);
  Serial.println(">>>>>>>FF>>>>>>>>>>>");
  if (root.isDirectory()) {
    if (audio_media_cur_dir > 2) {
      for (int i=0; i<=audio_media_cur_dir-2; i++) {
        dir = root.openNextFile();
        if (dir.isDirectory()) {
          Serial.print("audio_media_file_from_dir: ");
          Serial.println(dir.name());
        }
      }
    }
  Serial.println(">>>>>>Stop>>>>>>>>>>");
  if (audio_media_cur_dir > 1) { 
    dir=root.openNextFile();
    if (dir) obj[0]=dir.name();
  }
  Serial.println(obj[0]);
  if (audio_media_cur_dir > 0) { 
    dir=root.openNextFile();
    if (dir) obj[1]=dir.name();
  }
  Serial.println(obj[1]);
  dir=root.openNextFile();
  if (dir) obj[2]=dir.name();
  Serial.println(obj[2]);
  if (audio_media_cur_dir <= audio_media_num_dir -1) {
    dir=root.openNextFile();
    if (dir) obj[3]=dir.name();
  }
  Serial.println(obj[3]);
  if (audio_media_cur_dir <= audio_media_num_dir -2) {
    dir=root.openNextFile();
    if (dir) obj[4]=dir.name();
  }
  Serial.println(obj[4]);
  Serial.println("----------------------");
  dir.close();
  root.close();
  audiodisplay.select(obj[0].c_str(),obj[1].c_str(),obj[2].c_str(),obj[3].c_str(),obj[4].c_str());
  }
}

void AudioModul::audio_media_select_file() {
  Serial.print("audio_media_select_file => cur_dir: ");
  Serial.println(audio_media_cur_dir);
  Serial.print("audio_media_select_file => cur_file: ");
  Serial.println(audio_media_cur_file);
  String obj[5];
  for(int i=0;i<5;i++) obj[i]="";
  File root;
  File dir;
  File file;
  root = SD.open("/");
  Serial.print("Anzahl Dirs: ");
  Serial.println(audio_media_num_dir);
  Serial.println(">>>>>>>FF>Dir>>>>>>>>>>");
  if (root.isDirectory()) {
    for (int i=0; i<=audio_media_cur_dir; i++) {
      dir = root.openNextFile();
      if (dir.isDirectory()) {
        Serial.print("audio_media_file_from_file (dir): ");
        Serial.println(dir.name());
      }
    }
  }
  Serial.println(">>>>>>Stop>Dir>>>>>>>>>");
  audio_media_num_file = 0;
  while (dir.openNextFile()) audio_media_num_file++;
  dir.rewindDirectory();
  Serial.print("Anzahl Files: ");
  Serial.println(audio_media_num_file);
  Serial.println(">>>>>>>FF>File>>>>>>>>>>");
  if (dir.isDirectory()) {
    if (audio_media_cur_file > 2) {
      for (int i=0; i<=audio_media_cur_file-3; i++) {
        file = dir.openNextFile();
        if (file) {
          Serial.print("audio_media_file_from_file (file): ");
          Serial.println(file.name());
        }
      }
    }
  }
  Serial.println(">>>>>>Stop>File>>>>>>>>>");
  if (audio_media_cur_file > 1) { 
    file=dir.openNextFile();
    if (file) obj[0]=file.name();
  }
  Serial.println(obj[0]);
  if (audio_media_cur_file > 0) { 
    file=dir.openNextFile();
    if (file) obj[1]=file.name();
  }
  Serial.println(obj[1]);
  file=dir.openNextFile();
  if (file) obj[2]=file.name();
  Serial.println(obj[2]);
  if (audio_media_cur_file <= audio_media_num_file -1) {
    file=dir.openNextFile();
    if (file) obj[3]=file.name();
  }
  Serial.println(obj[3]);
  if (audio_media_cur_file <= audio_media_num_file -2) {
    file=dir.openNextFile();
    if (file) obj[4]=file.name();
  }
  Serial.println(obj[4]);
  Serial.println("----------------------");
  file.close();
  dir.close();
  root.close();
  audiodisplay.select(obj[0].c_str(),obj[1].c_str(),obj[2].c_str(),obj[3].c_str(),obj[4].c_str());

}

void AudioModul::audio_media_show() {
  Serial.println("In audio_media_show");
  audiodisplay.clear();
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  audiodisplay.show_vol(audio_vol);
  audiodisplay.show_time(false);
  audiodisplay.show_info1("Mediaplayer");
}

#endif
/*********************************************************************************************************
 * 
 *  Ab hier alles für den Bluetoothspeaker
 * 
 * 
**********************************************************************************************************/
#ifdef USE_AUDIO_SPEAKER

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

#endif