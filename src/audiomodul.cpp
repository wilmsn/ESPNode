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
  audio_media_num_dir = 0;
  File root = SD.open("/");
  File dir = root.openNextFile();
  while(dir){
    if(dir.isDirectory()) audio_media_num_dir++;
    dir = root.openNextFile();
  }
  dir.close();
  root.close();
//  audio_media_cur_dir = 1;
//  audio_media_cur_file = 1;
#endif
#ifdef USE_FTP
  ftp.addFilesystem("SD", &SD);
#endif
  audio_set_modus(Off);
  last_modus = Radio;
}

void AudioModul::html_create(String& tmpstr) {
  Switch_OnOff::html_create(tmpstr);
//  String tmpstr;
  tmpstr = String("\"audio_show\":1");
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
      tmpstr += String(",\"audio_media\":1");
      audio_media_web_init();
    break;
#endif
#ifdef USE_AUDIO_SPEAKER
    case Speaker:
      tmpstr += String(",\"audio_speak\":1");
      audio_speak_web_init();
    break;
#endif
    case Off:

    break;
    default:

    break;
  }
  tmpstr += String(",\"audio_vol\":") + String(audio_vol);
  tmpstr += String(",\"audio_bas\":") + String(audio_bas);
  tmpstr += String(",\"audio_tre\":") + String(audio_tre);
  ws.textAll(tmpstr.c_str());
  write2log(LOG_MODULE,1,tmpstr.c_str());
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
      tmpstr += "}";
      write2log(LOG_MODULE,1,tmpstr.c_str());
      ws.textAll(tmpstr.c_str());
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
      tmpstr = "{\"audio_bas\":";
      tmpstr += audio_bas;
      tmpstr += "}";
      write2log(LOG_MODULE,1,tmpstr.c_str());
      ws.textAll(tmpstr.c_str());
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
      tmpstr = "{\"audio_tre\":";
      tmpstr += audio_tre;
      tmpstr += "}";
      write2log(LOG_MODULE,1,tmpstr.c_str());
      ws.textAll(tmpstr.c_str());
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
      tmpstr = "{\"audio_radio\":\"1\"}";
      write2log(LOG_MODULE,1,tmpstr.c_str());
      ws.textAll(tmpstr.c_str());
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
      audio_set_modus(Media);
      retval = true;
    }
#endif
#ifdef USE_AUDIO_SPEAKER
    // Set for speaker
    if ( keyword == String(AUDIO_SPEAKER) || keyword == String("audio_speak") ) {
      audio_set_modus(Speaker);
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
      write2log(LOG_MODULE,1,"audio_set_modus: case Off");
      rotarymodul.setMaxLevel(0);
      rotarymodul.initLevel(0, 0, 0, 100);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(0);
      modus = Off;
      audio_all_apps_off();
      audiodisplay.show_time(true);
    break;
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
      last_modus = Radio;
      audio_all_apps_off();
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
      rotarymodul.initLevel(1, 0, audio_media_cur_dir, audio_media_num_dir-1);
      rotarymodul.initLevel(2, 0, audio_media_cur_file, 100);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(audio_vol);
      modus = Media;
      last_modus = Media;
      audio_all_apps_off();
      audio_media_on();
      audio_media_disp_init();
      audio_media_web_init();
    break;
#endif
#ifdef USE_AUDIO_SPEAKER
    case Speaker:
      write2log(LOG_MODULE,1,"audio_set_modus: case Speaker");
      modus = Speaker;
      last_modus = Speaker;
      all_apps_off();
      audio_speak_on();
    break;
#endif
    default:
      write2log(LOG_MODULE,1,"audio_set_modus: case default");
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
              audio_media_disp_init();
              timeout_set = false;
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
  char str2[30];
  int j=0;
  bool tz_found = false;
  write2log(LOG_MODULE,2,"MP3 data:", info);
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
  if (info[0] == 'A' && info[1] == 'r' && info[2] == 't') {
    if (mediaPlayMode) audiodisplay.show_info1(str2);
  }
  if (info[0] == 'T' && info[1] == 'i' && info[2] == 't') {
    if (mediaPlayMode) audiodisplay.show_info2(str2);
  }
}

void audio_showstreamtitle(const char *info){
  String tmpstr;
  if (radioPlayMode) audiodisplay.show_info2(info);
  tmpstr = String("{\"audiomsg2\":\"") + String(info) + "\"}";
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
  tmpstr = String("{\"audiomsg1\":\"") + String(info) + String("\"}");
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
  ws.textAll("{\"audio_radio\":0}");
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
}

void AudioModul::audio_media_off() {
  ws.textAll("{\"audio_media\":0}");
}

void AudioModul::audio_media_disp_init() {
  audiodisplay.clear();
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  audiodisplay.show_vol(audio_vol);
  audiodisplay.show_time(false);
}

void AudioModul::audio_media_web_init() {
  String tmpstr;
  uint16_t dirNo = 0;
  uint16_t fileNo;
  File file;
  File root;
  File dir;
  write2log(LOG_MODULE,1,"Mediaplayer Web Init");
  ws.textAll("{\"audio_media\":1}");
  root = SD.open("/");
  root.rewindDirectory();
  dir = root.openNextFile();
  while (dir) {
    if (dir.isDirectory()) {
      tmpstr = "{\"audio_media_add_album\":\"A#" + String(dirNo) + "#0#" + String(dir.name()) + String("\"}");
      write2log(LOG_MODULE,1,tmpstr.c_str());
      ws.textAll(tmpstr.c_str());
      fileNo = 0;
      file = dir.openNextFile();
      while (file) {
        if (String(file.name()).endsWith(".mp3")) {
          tmpstr = "{\"audio_media_add_album\":\"T#" + String(dirNo) + "#" + String(fileNo) + "#" + String(file.name()) + String("\"}");
          write2log(LOG_MODULE,1,tmpstr.c_str());
          ws.textAll(tmpstr.c_str());
        }
        file = dir.openNextFile();
        fileNo++;
      }
    }
    dir = root.openNextFile();
    dirNo++;
  }
  file.close();
  dir.close();
  root.close();
}

void AudioModul::audio_media_sel_file(File& dir, File& file, uint8_t count) {
  for (int i=0; i<=count; i++) {
        file = dir.openNextFile();
        if (file) {
          Serial.print("audio_media_sel_file: ");
          Serial.println(file.name());
        }
  }
}

void AudioModul::audio_media_play(uint8_t _dirNo, uint8_t _fileNo) {
  String fileName;
  File root;
  File dir;
  File file;
  root = SD.open("/");
  if (root) {
    audio_media_sel_file(root, dir, _dirNo);
    if (dir) {
      audio_media_sel_file(dir, file, _fileNo);
      if (file) {
        fileName = String(dir.name()) + String("/") + String(file.name());
      }
      file.close();
    }
    dir.close();
  }
  root.close();
  audio.connecttoFS(SD,fileName.c_str());
}

void AudioModul::audio_media_select_dir() {
  String albumpic;
  String obj[5];
  for(int i=0;i<5;i++) obj[i]="";
  File root;
  File dir;
  File bmpFile;
  size_t bmpFileSize;
  uint16_t *bmp;
  root = SD.open("/");
  if (root.isDirectory()) {
    if (audio_media_cur_dir > 2) {
      audio_media_sel_file(root, dir,audio_media_cur_dir-3);
    }
  }
  if (audio_media_cur_dir > 1) { 
    dir=root.openNextFile();
    if (dir) obj[0]=dir.name();
  }
  if (audio_media_cur_dir > 0) { 
    dir=root.openNextFile();
    if (dir) obj[1]=dir.name();
  }
  Serial.println(obj[0]);
  Serial.println(obj[1]);
  dir=root.openNextFile();
  if (dir) {
    obj[2]=dir.name();
    String bmpFileName = String("/")+obj[2]+String("/cover.bmp");
  Serial.println(bmpFileName);
    bmpFile = SD.open(bmpFileName.c_str(),"r");
    if (bmpFile) {
      bmpFileSize = bmpFile.size();
  Serial.println("bmpFileName ok");
  Serial.print("Size: ");
  Serial.println(bmpFileSize);
  bmp = (uint16_t*)malloc(bmpFileSize);
      uint16_t c1;
      uint16_t c2;
      size_t i = (bmpFileSize>>1)-1;
      size_t j = 0;
      size_t start_data;
      Serial.print(bmpFileName);
      Serial.println(" found");
      Serial.print("i = ");
      Serial.println(i);
      Serial.print("sizeof(*bmp): ");
      Serial.println(sizeof(*bmp));
      while (bmpFile.available()) {
//        for (int i=0;i<65;i++) bmpFile.read();
      Serial.println(i);
      Serial.println(j);
//        if (j == 0x0a ) {
//          start_data = bmpFile.read() | bmpFile.read()<<8 | bmpFile.read()<<16 | bmpFile.read()<<24;
//          j+=4;
//        }
        if ( j >= 66) {
          c1=bmpFile.read();
          c2=bmpFile.read();
          if (i >= 0) bmp[i] = (c2<<8) | c1;
        } else {
          bmpFile.read();
          bmpFile.read();
        }
        i--;
        j++;
      }
    }
    bmpFile.close();
  }
  if (audio_media_cur_dir <= audio_media_num_dir -1) {
    dir=root.openNextFile();
    if (dir) obj[3]=dir.name();
  }
  if (audio_media_cur_dir <= audio_media_num_dir -2) {
    dir=root.openNextFile();
    if (dir) obj[4]=dir.name();
  }
  dir.close();
  root.close();
//  audiodisplay.select(obj[1].c_str(),obj[2].c_str(),obj[3].c_str());
  audiodisplay.select(obj[2].c_str(),bmp);
  free(bmp);
//  free(bmp_r);
//  audiodisplay.show_jpg(albumpic);
}

void AudioModul::audio_media_select_file() {
  String obj[5];
  for(int i=0;i<5;i++) obj[i]="";
  File root;
  File dir;
  File file;
  root = SD.open("/");
  if (root.isDirectory()) {
    audio_media_sel_file(root, dir, audio_media_cur_dir);
  }
  audio_media_num_file = 0;
  while (dir.openNextFile()) audio_media_num_file++;
  dir.rewindDirectory();
  if (dir.isDirectory()) {
    if (audio_media_cur_file > 2) {
      audio_media_sel_file(dir, file, audio_media_cur_file-3);
    }
  }
  if (audio_media_cur_file > 1) { 
    file=dir.openNextFile();
    if (file) obj[0]=file.name();
  }
  if (audio_media_cur_file > 0) { 
    file=dir.openNextFile();
    if (file) obj[1]=file.name();
  }
  file=dir.openNextFile();
  if (file) obj[2]=file.name();
  if (audio_media_cur_file <= audio_media_num_file -2) {
    file=dir.openNextFile();
    if (file) obj[3]=file.name();
  }
  if (audio_media_cur_file <= audio_media_num_file -3) {
    file=dir.openNextFile();
    if (file) obj[4]=file.name();
  }
  rotarymodul.initLevel(2,0,audio_media_cur_file,audio_media_num_file-1);
  file.close();
  dir.close();
  root.close();
  audiodisplay.select(obj[1].c_str(),obj[2].c_str(),obj[3].c_str());
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