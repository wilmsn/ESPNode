#include "config.h"
#ifdef USE_AUDIOMODUL

#include "audiomodul.h"
#include "common.h"

#include "Audio.h"
/// @brief Instance for audio (I2S and decoder) device
Audio            audio;

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
#define GC9A01A_TFT_SCK                 12
#define GC9A01A_TFT_MOSI                11
#define GC9A01A_TFT_CS                  8
#define GC9A01A_TFT_DC                  9
// Rotary Encoder
#define ROTARY_ENCODER_A_PIN            1 //38 //37
#define ROTARY_ENCODER_B_PIN            2 //39 //36
#define ROTARY_ENCODER_SW_PIN           3 //40 //35
#define ROTARY_ENCODER_RESISTOR         INPUT_PULLUP
#endif

#define ARC_SIGMENT_DEGREES             3
#define ARC_WIDTH                       5

/// @brief Instances for the Preferences is defined in main.h
extern Preferences preferences;
/// @brief Instance for websockets is defined in webserver.h
extern AsyncWebSocket ws;

/// @brief Instance for the rotary module
RotaryModul      rotarymodul;
/// @brief Instance for displayhandling
AudioDisplay     audiodisplay;
/// @brief Instance for ohysical TFT Display
#ifdef DISPLAY_GC9A01A
Adafruit_GC9A01A tftx(GC9A01A_TFT_CS, GC9A01A_TFT_DC);
#endif

/// @brief Variable zur Steuerung der Anzeige Steaminhalte im Display
bool radioPlayMode = false;
bool mediaPlayMode = false;
String     audiomsg1;
String     audiomsg2;
String     audiomsg3;
String     audiomsg4;
String     audiomsg5;

uint32_t   allSongs;
uint16_t   allAlbum;


void AudioModul::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword)  {
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, false, true, true);
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
  audiodisplay.show_info1("Init");
  rotarymodul.begin(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_SW_PIN, ROTARY_ENCODER_RESISTOR);
//  rotarymodul.initLevel(0,0,audio_vol,100);
//  rotarymodul.initLevel(1,0,audio_radio_cur_station,10);
//  rotarymodul.initLevel(2,0,0,2);
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
  //audio.setBufsize(30000,600000);
  audio.setVolumeSteps(100);
  audio.setVolume(audio_vol);
  audio_set_modus(Off);
  last_modus = Radio;
//  audio.setAudioTaskCore(1);
#ifdef USE_AUDIO_MEDIA
  readSD();
//  read_SD_dir();
  char tmpstr[40];
  snprintf(tmpstr,40,"Anzahl Alben: %u Files: %u",allAlbum, allSongs);
  write2log(LOG_CRITICAL,1,tmpstr);
#endif

  obj_html_info =  String("\"tab_head_audio\":\"I2S: MA98357\"")+
                   String(",\"tab_line1_audio\":\"DOUT:#GPIO: ")+String(I2S_DOUT)+ String("\"")+
                   String(",\"tab_line2_audio\":\"BCLK:#GPIO: ")+String(I2S_BCLK)+ String("\"")+
                   String(",\"tab_line3_audio\":\"LRC:#GPIO: ")+String(I2S_LRC)+ String("\"")+
                   String(",\"tab_head_display\":\"Display: GC9A01A\"")+
                   String(",\"tab_line1_display\":\"SCK:#GPIO: ")+String(GC9A01A_TFT_SCK)+ String("\"")+
                   String(",\"tab_line2_display\":\"MOSI:#GPIO: ")+String(GC9A01A_TFT_MOSI)+ String("\"")+
                   String(",\"tab_line3_display\":\"CS:#GPIO: ")+String(GC9A01A_TFT_CS)+ String("\"")+
                   String(",\"tab_line4_display\":\"DC:#GPIO: ")+String(GC9A01A_TFT_DC)+ String("\"")+
                   String(",\"tab_head_rotary\":\"Rotary\"")+
                   String(",\"tab_line1_rotary\":\"A-Pin:#GPIO: ")+String(ROTARY_ENCODER_A_PIN)+ String("\"")+
                   String(",\"tab_line2_rotary\":\"B-Pin:#GPIO: ")+String(ROTARY_ENCODER_B_PIN)+ String("\"")+
                   String(",\"tab_line3_rotary\":\"SW-Pin:#GPIO: ")+String(ROTARY_ENCODER_SW_PIN)+ String("\"")+
                   String(",\"tab_line4_rotary\":\"Resistor:# ");
  switch (ROTARY_ENCODER_RESISTOR) {
    case INPUT:          obj_html_info += String("keiner")+String("\"");   break;
    case INPUT_PULLUP:   obj_html_info += String("pullup")+String("\"");   break;
    case INPUT_PULLDOWN: obj_html_info += String("pulldown")+String("\""); break;
  }
}

void AudioModul::html_create(String& tmpstr) {
  Switch_OnOff::html_create(tmpstr);
  tmpstr += String(",\"audio_show\":1");
#ifdef USE_AUDIO_RADIO
  tmpstr += String(",\"audio_radio_show\":1");
#endif
#ifdef USE_AUDIO_MEDIA
  tmpstr += String(",\"audio_media_show\":1");
#endif
#ifdef USE_AUDIO_SPEAKER
  tmpstr = String(",\"audio_speak_show\":1");
#endif
  switch(modus) {
#ifdef USE_AUDIO_RADIO
    case Radio:  
      // Radio ist vorhanden und wird im Web angezeigt
      tmpstr += String(",\"audio_radio\":1");
      audio_radio_web_init();
    break;
#endif
#ifdef USE_AUDIO_MEDIA
    case Media:
      tmpstr += String(",\"audio_media_active\":1");
    break;
#endif
#ifdef USE_AUDIO_SPEAKER
    case Speaker:
      tmpstr += String(",\"audio_speak\":1");
      audio_speak_web_init();
    break;
#endif
    case Off:
      tmpstr += String(",\"audio_radio\":0");
      tmpstr += String(",\"audio_media\":0");
    break;
    default:

    break;
  }
  tmpstr += String(",\"audio_vol\":") + String(audio_vol);
  tmpstr += String(",\"audio_bas\":") + String(audio_bas);
  tmpstr += String(",\"audio_tre\":") + String(audio_tre);
  if (audiomsg1.length() >2) tmpstr += String(",\"audiomsg1\":\"")+audiomsg1+String("\"");
  if (audiomsg2.length() >2) tmpstr += String(",\"audiomsg2\":\"")+audiomsg2+String("\"");
  if (audiomsg3.length() >2) tmpstr += String(",\"audiomsg3\":\"")+audiomsg3+String("\"");
  if (audiomsg4.length() >2) tmpstr += String(",\"audiomsg4\":\"")+audiomsg4+String("\"");
  if (audiomsg5.length() >2) tmpstr += String(",\"audiomsg5\":\"")+audiomsg5+String("\"");
  ws.textAll(tmpstr.c_str());
  write2log(LOG_MODULE,1,tmpstr.c_str());
#ifdef USE_AUDIO_MEDIA
//  if (modus == Media) allAlbum2Web();
#endif
}

bool AudioModul::set(const String& keyword, const String& value) {
  bool retval = false;
  String tmpstr;
  String myvalue = value;
  write2log(LOG_MODULE,2,keyword.c_str(),value.c_str());
  if ( ! Switch_OnOff::set(keyword, value) ) {
    std::replace(myvalue.begin(),myvalue.end(),'\n',' ');
    if ( keyword == String("audio_vol") ) {
      audio_vol = value.toInt();
      tmpstr = "{\"audio_vol\":";
      tmpstr += audio_vol;
      audio.setVolume(audio_vol);
      rotarymodul.setValue(audio_vol);
      audiodisplay.show_vol(audio_vol);
      if (audio_vol == 0) {
        audio_set_modus(Off);
      }
      tmpstr += "}";
      write2log(LOG_MODULE,1,tmpstr.c_str());
      ws.textAll(tmpstr.c_str());
      retval = true;
    }
    if ( keyword == String("audio_bas") ) {
      // Bass kann nur über die Weboberfläche eingestellt werden
      audio_bas = value.toInt();
      tmpstr = "{\"audio_bas\":";
      tmpstr += audio_bas;
      tmpstr += "}";
      write2log(LOG_MODULE,1,tmpstr.c_str());
      ws.textAll(tmpstr.c_str());
      audio.setTone((int8_t)-40+audio_bas,0,(int8_t)-40+audio_tre);
      retval = true;
    }
    if ( keyword == String("audio_tre") ) {
      // Höhen können nur über die Weboberfläche eingestellt werden
      audio_tre = value.toInt();
      tmpstr = "{\"audio_tre\":";
      tmpstr += audio_tre;
      tmpstr += "}";
      write2log(LOG_MODULE,1,tmpstr.c_str());
      ws.textAll(tmpstr.c_str());
      audio.setTone((int8_t)-40+audio_bas,0,(int8_t)-40+audio_tre);
      retval = true;
    }
#ifdef USE_AUDIO_RADIO
    // Set for radio
    // Radio einschalten
    if ( keyword == String(AUDIO_RADIO) || keyword == String("audio_radio") ) {
      if (modus != Off) audio_set_modus(Radio);
//      tmpstr = "{\"audio_radio\":\"1\"}";
//      write2log(LOG_MODULE,1,tmpstr.c_str());
//      ws.textAll(tmpstr.c_str());
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
      audio_radio_web_init();
    }
    // Radio: Sender Name speichern ueber Webinterface
    if ( keyword == String("audio_radio_save_stn_name") ) {
      snprintf(station[audio_radio_cur_station].name,STATION_NAME_LENGTH,"%s",value.c_str());
      audio_radio_save_stations();
      audio_radio_web_init();
    }
    // Radio: Sender URL speichern ueber Webinterface
    if ( keyword == String("audio_radio_save_stn_url") ) {
      snprintf(station[audio_radio_cur_station].url,STATION_URL_LENGTH,"%s",value.c_str());
      audio_radio_save_stations();
      audio_radio_web_init();
    }
#endif
#ifdef USE_AUDIO_MEDIA
    // Set for media
    if ( keyword == String(AUDIO_MEDIA) || keyword == String("audio_media") ) {
      if (modus != Off) audio_set_modus(Media);
      retval = true;
    }
    if ( keyword == "allAlbum2Web" ) {
      allAlbum2Web();
      retval = true;
    }
    if ( keyword == "audio_media_get_album" ) {
      audio_media_get_album(value.toInt());
      retval = true;
    }
    if ( keyword == "audio_media_play_album" ) {
      audio_media_cur_dir = value.toInt();
      audio_media_cur_file = 0;
      audio_media_play(audio_media_cur_dir,audio_media_cur_file);
      retval = true;
    }
    if ( keyword == "audio_media_play_song" ) {
      char tmpstr[10];
      audio_media_cur_dir = 0;
      audio_media_cur_file = 0;
      bool dirMode = true;
      snprintf(tmpstr,10,"%s",value.c_str());
      int i =0;
      do {
        if (tmpstr[i] == '#') {
          dirMode = false;
        } else {
          if (dirMode) {
            audio_media_cur_dir=10*audio_media_cur_dir+(tmpstr[i]-'0');
          } else {
            audio_media_cur_file=10*audio_media_cur_file+(tmpstr[i]-'0');
          }
        }
        i++;
      } while(i<strlen(tmpstr));
      audio_media_play(audio_media_cur_dir,audio_media_cur_file);
      retval = true;
    }
#endif
#ifdef USE_AUDIO_SPEAKER
    // Set for speaker
    if ( keyword == String(AUDIO_SPEAKER) || keyword == String("audio_speak") ) {
      if ( modus != Off) audio_set_modus(Speaker);
      tmpstr = "{\"audio_speak\":1}";
      write2log(LOG_MODULE,1,tmpstr.c_str());
      ws.textAll(tmpstr.c_str());
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

void AudioModul::audio_set_modus(modus_t _modus) {
  last_modus = modus;
  switch (_modus) {
    case Select:
      write2log(LOG_MODULE,1,"audio_set_modus: case Select");
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
      write2log(LOG_MODULE,1,"audio_set_modus: case Radio");
      rotarymodul.setMaxLevel(1);
      rotarymodul.initLevel(0, 0, audio_vol, 100);
      rotarymodul.initLevel(1, 0, audio_radio_cur_station, MAXSTATIONS-1);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(audio_vol);
      modus = Radio;
      do_switch(true);
      switch (last_modus) {
#ifdef USE_AUDIO_MEDIA
        case Media:
          audio_media_off();
        break;
#endif
#ifdef USE_AUDIO_SPEAKER
        case Speaker:
          audio_speak_off();
        break;
#endif
      }
      audio_radio_on();
      audio_radio_disp_init();
      audio_radio_web_init();
    break;
#endif
#ifdef USE_AUDIO_MEDIA
    case Media:
      write2log(LOG_MODULE,1,"audio_set_modus: case Media");
      rotarymodul.setMaxLevel(2);
      rotarymodul.initLevel(0, 0, audio_vol, 100);
      rotarymodul.initLevel(1, 0, audio_media_cur_dir, allAlbum-1);
      rotarymodul.initLevel(2, 0, audio_media_cur_file, 100);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(audio_vol);
      modus = Media;
      do_switch(true);
      switch (last_modus) {
#ifdef USE_AUDIO_RADIO
        case Radio:
          audio_radio_off();
        break;
#endif
#ifdef USE_AUDIO_SPEAKER
        case Speaker:
          audio_speak_off();
        break;
#endif
      }
      audio_media_on();
      audio_media_disp_init();
    break;
#endif
#ifdef USE_AUDIO_SPEAKER
    case Speaker:
      write2log(LOG_MODULE,1,"audio_set_modus: case Speaker");
      modus = Speaker;
      do_switch(true);
      switch (last_modus) {
#ifdef USE_AUDIO_RADIO
        case Radio:
          audio_radio_off();
        break;
#endif
#ifdef USE_AUDIO_MEDIA
        case Media:
          audio_media_off();
        break;
#endif
      }
      audio_speak_on();
    break;
#endif
    case Off: 
    default:
      write2log(LOG_MODULE,1,"audio_set_modus: case Off");
      ws.textAll("{\"audio_vol\":0}");
      rotarymodul.setMaxLevel(0);
      rotarymodul.initLevel(0, 0, 0, 100);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(0);
      audio_vol=0;
      do_switch(false);
      modus = Off;
      audiomsg1 = "";
      audiomsg2 = "";
      audiomsg3 = "";
      audiomsg4 = "";
      audiomsg5 = "";

#ifdef USE_AUDIO_RADIO
      audio_radio_off();
#endif
#ifdef USE_AUDIO_MEDIA
      audio_media_off();
#endif
#ifdef USE_AUDIO_SPEAKER
      audio_speak_off();
#endif
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
//  if (timeinfo.tm_min != old_min) {
  if (now - old_now > 59) { 
    time_update = true;
    old_now = now;
//    old_min = timeinfo.tm_min;
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
        if ( song_started > 0) {
          if (now - song_started > 2) {
            audio_media_cur_file++;
            if (! validSong(audio_media_cur_dir, audio_media_cur_file)) {
              audio_media_cur_dir++;
              audio_media_cur_file=0;
              if (! validSong(audio_media_cur_dir, audio_media_cur_file)) {
                audio_media_cur_dir=0;
                audio_media_cur_file=0;
              }
            } 
            audio_media_play(audio_media_cur_dir, audio_media_cur_file);
          }
        } else song_started = now;
      }
#endif
    }
  }
  rotarymodul.loop(now);
// Hier wird der Klickstream definiert
  switch(modus) {
      case Off:
      if ( time_update ) audiodisplay.show_time(true);
      switch (rotarymodul.changed()) {
        case 1:
        default:
        // Der Rotary wurde gedreht => Anwendung ändern
        if (rotarymodul.curValue() > 5) {
          // Anlage einschalten
          audio_set_modus(last_modus);
        }
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
              audio_radio_disp_init();
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
        audio_media_cur_dir   = audio_media_tmp_dir;
        audio_media_cur_album = audio_media_tmp_album;
        audio_media_cur_file  = audio_media_tmp_file;
        audio_media_cur_song  = audio_media_tmp_song;
        rotarymodul.setLevel(0);
        timeout_set = false;
        audio_media_disp_init();
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
              rotarymodul.setMaxValue(allAlbum-1);
              audio_media_cur_album = rotarymodul.curValue();
              audio_media_select_album();
              audio_media_cur_song = 0;
              timeout_start = now;
            break;
            case 2:
            // Songauswahl
              rotarymodul.setMaxValue(countSong4Album(audio_media_cur_dir)-1);
              audio_media_cur_file = rotarymodul.curValue();
              audio_media_select_song();
              timeout_start = now;
            break;
          }
        break;
        case 2:
        // Der Rotary wurde kurz gedrückt
          switch(rotarymodul.curLevel()) {
            case 0:
            // Wiedergabe aktuellen Song anzeigen
              audio_media_play(audio_media_cur_dir,audio_media_cur_file);
              audio_media_disp_init();
              timeout_set = false;
            break;
            case 1:
            // Albumauswahl initiieren und anzeigen
              audio_media_tmp_dir   = audio_media_cur_dir;
              audio_media_tmp_album = audio_media_cur_album;
              audio_media_tmp_song  = audio_media_cur_song;
              audio_media_tmp_file  = audio_media_cur_file;
              audio_media_select_album();
              timeout_start = now;
              timeout_set = true;
            break;
            case 2:
            // Songauswahl initiieren und anzeigen
              rotarymodul.setValue(audio_media_cur_file);
              audio_media_select_song();
              timeout_start = now;
              timeout_set = true;
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
          audio_show_modus((modus_t)rotarymodul.curValue());
        break;
        case 2:
        // Der Rotary wurde kurz gedrückt => Anwendung bestätigen
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
  char str2[30];
  int j=0;
  bool tz_found = false;
  write2log(LOG_MODULE,2,"Info:", info);
  for (int i=0; i<strlen(info) && i<40; i++) {
    if (! tz_found) {
      if (info[i] == ':') {
        tz_found = true;
        i++;
        i++;
      }
    }
    if(tz_found && j<30) {
      str2[j] = info[i];
      j++;
    }
  }
  str2[j]=0;
  if (info[0] == 'B' && info[1] == 'i' && info[3] == 'R') {
    if (strlen(str2) == 6) {
      str2[3]=' ';
      str2[4]='K';
      str2[5]='B';
      str2[6]='s';
      str2[7]=0;
      if (mediaPlayMode) audiodisplay.show_bps(str2);
    }
    if (strlen(str2) == 5) {
      str2[2]=' ';
      str2[3]='K';
      str2[4]='B';
      str2[5]='s';
      str2[6]=0;
      if (mediaPlayMode) audiodisplay.show_bps(str2);
    }
  }
}

void audio_id3data(const char *info){
  size_t infoSize = strlen(info);
  char str2[infoSize];
  int j=0;
  bool tz_found = false;
  write2log(LOG_MODULE,2,"MP3 data:", info);
  for (int i=0; i<strlen(info); i++) {
    if (! tz_found) {
      if (info[i] == ':') {
        tz_found = true;
        i++;
        i++;
      }
    }
    if(tz_found) {
      str2[j] = info[i];
      j++;
    }
  }
  str2[j]=0;
  String tmpstr = "{";
  if (info[0] == 'A' && info[1] == 'r' && info[2] == 't') {
    if (mediaPlayMode) audiodisplay.show_info1(str2);
    audiomsg1 = String(str2);
    tmpstr += String("\"audiomsg1\":\"Artist: ")+audiomsg1+String("\"");
  }
  if (info[0] == 'T' && info[1] == 'i' && info[2] == 't') {
    if (mediaPlayMode) audiodisplay.show_info2(str2);
    if (tmpstr.length() > 5) tmpstr += ",";
    audiomsg2 = String(str2);
    tmpstr += String("\"audiomsg2\":\"Title: ")+audiomsg2+String("\"");
  }
  if (info[0] == 'A' && info[1] == 'l' && info[2] == 'b') {
    if (tmpstr.length() > 5) tmpstr += ",";
    audiomsg3 = String(str2);
    tmpstr += String("\"audiomsg3\":\"Album: ")+audiomsg3+String("\"");
  }
  tmpstr += "}";
  if (tmpstr.length() > 5) {
    ws.textAll(tmpstr);
    write2log(LOG_MODULE,1,tmpstr.c_str());
  }
}

void audio_showstreamtitle(const char *info){
  String tmpstr;
  audiomsg2 = info;
  if (radioPlayMode) audiodisplay.show_info2(info);
  tmpstr = String("{\"audiomsg2\":\"") + audiomsg2 + "\"}";
  write2log(LOG_MODULE,1,tmpstr.c_str());
  ws.textAll(tmpstr.c_str());
}

void audio_bitrate(const char *info) {
  String tmpstr;
  char bpsInfo[9];
  bpsInfo[0] = info[0];
  bpsInfo[1] = info[1];
  bpsInfo[2] = info[2];
  bpsInfo[3] = ' ';
  bpsInfo[4] = 'K';
  bpsInfo[5] = 'B';
  bpsInfo[6] = 'p';
  bpsInfo[7] = 's';
  bpsInfo[8] = 0;
  if (radioPlayMode || mediaPlayMode) audiodisplay.show_bps(bpsInfo);
  tmpstr = String("{\"audiomsg4\":\"") + String(bpsInfo) + String("\"}");
  write2log(LOG_MODULE,1,tmpstr.c_str());
  ws.textAll(tmpstr.c_str());
}

void audio_showstation(const char *info){
  String tmpstr;
  audiomsg1 = info;
  tmpstr = String("{\"audiomsg1\":\"") + audiomsg1 + String("\"}");
  write2log(LOG_MODULE,1,tmpstr.c_str());
  ws.textAll(tmpstr.c_str());
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
  audio.stopSong();
  ws.textAll("{\"audio_radio\":0}");
}

void AudioModul::audio_radio_on() {
  write2log(LOG_MODULE,1,"Radio on");
  if ( strlen(station[audio_radio_cur_station].url) > 10 ) {
    audio.connecttohost(station[audio_radio_cur_station].url);
    write2log(LOG_MODULE,2,"Switch to ",station[audio_radio_cur_station].url);
  }
}

void AudioModul::audio_radio_disp_init() {
  audiodisplay.clear();
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  audiodisplay.show_vol(audio_vol);
  audiodisplay.show_time(false);
  audiodisplay.show_info1(station[audio_radio_cur_station].name);
}

void AudioModul::audio_radio_web_init() {
  String tmpstr;
  ws.textAll("{\"audio_radio\":1}");
  for (int i=0; i<MAXSTATIONS; i++) {
    tmpstr = "{\"audio_radio_add_stn\":\"";
    tmpstr += station[i].url;
    tmpstr += ";";
    tmpstr += station[i].name;
    tmpstr += "\"}";
    write2log(LOG_MODULE,1,tmpstr.c_str());
    ws.textAll(tmpstr.c_str());
  }
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
    audio.connecttohost(station[audio_radio_cur_station].url);
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
  if (rotarymodul.curLevel() == 0) audio_media_disp_init();
  ws.textAll("{\"audio_media\":1}");
  write2log(LOG_MODULE,2,"Anzahl Songs: ",String(allSongs).c_str());
}

void AudioModul::audio_media_off() {
  ws.textAll("{\"audio_media\":0}");
}

void AudioModul::audio_media_disp_init() {
  audiodisplay.clear();
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  audiodisplay.show_vol(audio_vol);
  audiodisplay.show_time(false);
  audiodisplay.show_info1(audiomsg1.c_str());
  audiodisplay.show_info2(audiomsg2.c_str());
}

void AudioModul::newEntry_dir(AudioModul::music_dir_t* p_new) {
  music_dir_t *p_search;
  music_dir_t *p_tmp;
  bool inserted = false;
  int result;
//  Serial.println("New Entry Dir");
  p_new->p_next = NULL;
  if (p_music_dir_initial) {
// 1. Einsortieren vor p_initial
    if (strcmp(p_music_dir_initial->dirName,p_new->dirName)>0) {
//      Serial.println(String(p_music_dir_initial->dirName)+String(" p_init ist groesser als ")+String(p_new->dirName));
      p_new->p_next = p_music_dir_initial;
      p_music_dir_initial = p_new;
      inserted = true;
    } else {
// 2. Einsortieren in der Mitte
      p_search = p_music_dir_initial;
      while (p_search->p_next && ! inserted) {
        p_tmp = p_search->p_next;
        if (strcmp(p_tmp->dirName, p_new->dirName)>0) {
//          Serial.println(String(p_tmp->dirName)+String(" p_tmp ist groesser als ")+String(p_new->dirName));
          p_search->p_next = p_new;
          p_new->p_next = p_tmp;
          inserted = true;
        } else {
          p_search = p_search->p_next;
        }
      }
// 3. Einsortieren am Ende
      if ( ! inserted) {
//        Serial.println(String(p_search->dirName)+String(" letzte ist kleiner als ")+String(p_new->dirName));
        p_search->p_next = p_new;
      }
    }
  } else {
    p_music_dir_initial = p_new;
  }
/*  Serial.println("-----Status-----");
  p_search = p_music_dir_initial;
  while (p_search) {
    Serial.println(p_search->dirName);
    p_search = p_search->p_next;
  }
  Serial.println("----------------");*/
}

void AudioModul::newEntry_file(AudioModul::music_file_t* p_new) {
    music_file_t *p_search;
    p_new->p_next = NULL;
    if (p_music_file_initial) {
        p_search = p_music_file_initial;
        while (p_search->p_next) {
            p_search = p_search->p_next;
        }
        p_search->p_next = p_new;
    } else {
        p_music_file_initial = p_new;
    }
}

void AudioModul::addMusic(uint16_t dirNo, uint16_t fileNo, const char* dirName, const char* fileName) {
// 1. Testen ob dir bereits vorhanden
  music_dir_t* p_dir_search = p_music_dir_initial;
  bool dir_found = false;
  while (p_dir_search) {
    if (p_dir_search->dirNo == dirNo) dir_found = true;
    p_dir_search = p_dir_search->p_next;
  }
  if (!dir_found) {
    music_dir_t* p_dir_new = (music_dir_t*) ps_malloc(sizeof(AudioModul::music_dir_t));
    p_dir_new->dirNo = dirNo;
    p_dir_new->dirName = (char*) ps_malloc(strlen(dirName));
    sprintf(p_dir_new->dirName,"%s",dirName);
    newEntry_dir(p_dir_new);
  }
// 2. Datei eintragen
  music_file_t* p_file_new = (music_file_t*) ps_malloc(sizeof(AudioModul::music_file_t));
  p_file_new->fileName = (char*) ps_malloc(strlen(fileName));
  p_file_new->dirNo = dirNo;
  p_file_new->fileNo = fileNo;
  sprintf(p_file_new->fileName,"%s", fileName);
  newEntry_file(p_file_new);
  yield();
}

void AudioModul::countSongs() {
  music_file_t *p_search;
  allSongs = 0;
  if (p_music_file_initial) {
    p_search = p_music_file_initial;
    while (p_search->p_next) {
      allSongs++;;
      p_search = p_search->p_next;
    }
  }
}

uint16_t AudioModul::countSong4Album(uint16_t dir) {
  music_file_t *p_search;
  uint16_t retval = 0;
  if (p_music_file_initial) {
    p_search = p_music_file_initial;
    while (p_search->p_next) {
      if (p_search->dirNo == dir) retval++;
      p_search = p_search->p_next;
    }
  }
  return retval;
}

void AudioModul::countAlbum() {
  music_dir_t *p_search;
  uint16_t last_dirNo;
  allAlbum = 0;
  if (p_music_dir_initial) {
    p_search = p_music_dir_initial;
    last_dirNo = p_search->dirNo;
    allAlbum = 1;
    while (p_search->p_next) {
      if (p_search->dirNo != last_dirNo) {
        allAlbum++;
        last_dirNo = p_search->dirNo;
      }
      p_search = p_search->p_next;
    }
  }
}

bool AudioModul::validSong(uint16_t dirNo, uint16_t fileNo) {
  bool retval = false;
  music_file_t *p_file_search;
  if (p_music_file_initial) {
    p_file_search = p_music_file_initial;
    while (p_file_search) {
      if ( p_file_search->dirNo == dirNo && p_file_search->fileNo == fileNo ) retval = true;
      p_file_search = p_file_search->p_next;
    }
  }
  return retval;
}

void AudioModul::nextSong() {
  audio_media_cur_file++;
  if ( ! validSong(audio_media_cur_dir,audio_media_cur_file)) {
    audio_media_cur_dir++;
    audio_media_cur_file = 0;
  }
  if ( ! validSong(audio_media_cur_dir,audio_media_cur_file)) {
    audio_media_cur_dir = 0;
    audio_media_cur_file = 0;
  }
}

void AudioModul::allAlbum2Web() {
  String tmpstr1;
  String tmpstr2;
  uint8_t dircnt = 0;
  music_dir_t *p_dir_search;
  if (p_music_dir_initial) {
    p_dir_search = p_music_dir_initial;
    while (p_dir_search) {
      tmpstr1 = String("\"audio_media_add_album_d")+String(p_dir_search->dirNo)+String("f0")
              + String("\":\"A#")+String(p_dir_search->dirNo)+String("#0#")+String(p_dir_search->dirName)+String("\"");
      switch (dircnt) {
        case 0:
          tmpstr2 = String("{") + tmpstr1;
        break;
        default:
          tmpstr2 += String(",") + tmpstr1;
          if (dircnt == 9) {
            tmpstr2 += "}";
            ws.textAll(tmpstr2);
            write2log(LOG_MODULE,1,tmpstr2.c_str());
          }
        break;
      }
      dircnt++;
      if (dircnt>9) dircnt=0;
      p_dir_search = p_dir_search->p_next;
    }
  }
  tmpstr2 += "}";
  ws.textAll(tmpstr2);
  write2log(LOG_MODULE,1,tmpstr2.c_str());
}

void AudioModul::audio_media_get_album(uint16_t reqDirNo) {
  String tmpstr = "{\"x\":0";
  music_file_t *p_search;
  if (p_music_file_initial) {
    p_search = p_music_file_initial;
    while (p_search) {
      if (p_search->dirNo == reqDirNo) {
        tmpstr += ",\"audio_media_add_album_d"+String(p_search->dirNo)+String("f")+String(p_search->fileNo)+String("\":\"T#")
            +String(p_search->dirNo)+String("#")+String(p_search->fileNo)+String("#")+String(p_search->fileName)+String("\"");
      }
      p_search = p_search->p_next;
    }
  }
  tmpstr += "}";
  ws.textAll(tmpstr);
}

void AudioModul::readSD() {
  File root;
  File dir;
  File file;
  uint16_t dirNo = 0;
  uint16_t fileNo = 0;
/*  music_t* p_tmp;
  while (p_initial) {
    p_tmp = p_initial->p_next;
    free(p_initial);
    p_initial = p_tmp;
  }*/
  yield();
  audiodisplay.clear();
  audiodisplay.show_info1("Reading SD");
  root = SD.open("/");
  root.rewindDirectory();
  dir = root.openNextFile();
  while (dir) {
    if (dir.isDirectory()) {
      file = dir.openNextFile();
      while (file) {
        if (String(file.name()).endsWith(".mp3")) {
          addMusic(dirNo, fileNo, dir.name(), file.name());
          fileNo++;
        }
        file = dir.openNextFile();
      }
      audiodisplay.show_info2(String(dirNo).c_str());
#if defined(DEBUG_SERIAL_MODULE)
      Serial.println(String("Lade Dir: "+String(dirNo)));
#endif
      dirNo++;
      fileNo = 0;
    }
    dir = root.openNextFile();
  }
  file.close();
  dir.close();
  root.close();
// Songnummern setzen
  music_dir_t* p_search = p_music_dir_initial;
  uint16_t albumNo = 0;
  while (p_search) {
    p_search->albumNo = albumNo;
    albumNo++;
    p_search = p_search->p_next;
  }
  countSongs();
  countAlbum();
#if defined(DEBUG_SERIAL_MODULE)
  Serial.println("===Chain Dir===");
  p_search = p_music_dir_initial;
  while (p_search) {
    Serial.println(String(p_search->albumNo)+String(" : ")+String(p_search->dirNo)+String(" : ")+String(p_search->dirName));
    p_search = p_search->p_next;
  }
  Serial.println("===============");
#endif
}

void AudioModul::audio_media_play(uint16_t _albumNo, uint16_t _songNo) {
  String sdName = "";
  char* dirName;
  char* fileName;
  bool dir_found = false;
  bool file_found = false;
  music_dir_t *p_dir_search;
  music_file_t *p_file_search;
  if (p_music_dir_initial) {
    p_dir_search = p_music_dir_initial;
    while (p_dir_search) {
      if (p_dir_search->albumNo == _albumNo) {
        dirName = p_dir_search->dirName;
        dir_found = true;
        p_dir_search = NULL;
      } else {
        p_dir_search = p_dir_search->p_next;
      }
    }
  }
  if (p_music_file_initial) {
    p_file_search = p_music_file_initial;
    while (p_file_search) {
      if (p_file_search->fileNo == _songNo && p_file_search->dirNo == _albumNo) {
        fileName = p_file_search->fileName;
        file_found = true;
        p_file_search = NULL;
      } else {
        p_file_search = p_file_search->p_next;
      }
    }
  }
  sdName = String("/")+String(dirName)+String("/")+String(fileName);
  if (sdName.length() > 5) audio.connecttoFS(SD,sdName.c_str());
  audiodisplay.show_info1(" ");
  audiodisplay.show_info2(" ");
  song_started = 0;
}

void AudioModul::audio_media_select_album() {
  String album[3];
  music_dir_t *p_search;
  music_dir_t *p_last = NULL;
  bool finished = false;
  album[0] = " ";
  album[1] = " ";
  album[2] = " ";
  if (p_music_dir_initial) {
    p_search = p_music_dir_initial;
    while (p_search && ! finished) {
      if (p_search->albumNo == audio_media_cur_album) {
        if (p_last) album[0] = p_last->dirName;
        album[1] = p_search->dirName;
        if (p_search->p_next) album[2] = (p_search->p_next)->dirName;
      }
      p_last = p_search;
      p_search = p_search->p_next;
    }
  }
  Serial.println(String("Display-select:")+album[0]+String(" ")+album[1]+String(" ")+album[2]);
  audiodisplay.select(album[0].c_str(),album[1].c_str(),album[2].c_str());
}

void AudioModul::audio_media_select_song() {
  String song[3];
  music_file_t *p_search;
  uint16_t act_file;
  int idx = 0;
  song[0] = " ";
  song[1] = " ";
  song[2] = " ";
  if (audio_media_cur_file > 0) {
    act_file= audio_media_cur_file -1;
  } else {
    act_file = 0;
    idx = 1;
  }
  if (p_music_file_initial) {
    p_search = p_music_file_initial;
    while (p_search) {
      if (p_search->dirNo == audio_media_cur_dir && p_search->fileNo == act_file) {
        if (idx < 3) {
          song[idx] = p_search->fileName;
          act_file++;
          idx++;
        }
      }
      p_search = p_search->p_next;
    }
  }
#if defined(DEBUG_SERIAL_MODULE)
  Serial.println(String("DirNo: ")+String(audio_media_cur_dir)+String(" FileNo: ")+String(audio_media_cur_file)+String(" ")+song[0]+String(" ")+song[1]+String(" ")+song[2]);
#endif
  audiodisplay.select(song[0].c_str(),song[1].c_str(),song[2].c_str());
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
  html_json = "{\"audio_speak\":1}";
  ws.textAll(html_json);
}

void AudioModul::audio_speak_off() {
  html_json = "{\"audio_speak\":0}";
  ws.textAll(html_json);
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