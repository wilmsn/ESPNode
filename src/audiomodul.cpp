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
/// @brief Instance for ohysical TFT Display
#ifdef DISPLAY_GC9A01A
Adafruit_GC9A01A tftx(GC9A01A_TFT_CS, GC9A01A_TFT_DC);
#endif
/// @brief Instance for displayhandling
AudioDisplay     audiodisplay;

void AudioModul::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword)  {
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, true, true);
  audio_radio_load_stations();
  preferences.begin("settings",false);
  if ( !preferences.isKey("audio_vol") ) {
    preferences.putUChar("audio_vol", 5);
    preferences.putUChar("audio_tre", 0);
    preferences.putUChar("audio_bas", 0);
    preferences.putUChar("ar_cur_station", 0);
  }
  audio_radio_cur_station = preferences.getUChar("ar_cur_station");
  audio_vol = 5; //preferences.getUChar("audio_vol");
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
  set("audio_radio","1");
  set("audio_vol",String(audio_vol).c_str());
  audio_on();
}

void AudioModul::html_create_json_part(String& json) {
  Switch_OnOff::html_create_json_part(json);
  json += ",\"audio_show\":1";
  json += ",\"audio_radio\":";
  json += (modus == Radio)?1:0;
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
  audio_radio_station_json(json);
}

bool AudioModul::set(const String& keyword, const String& value) {
  bool retval = false;
  String myvalue = value;
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
      // TODO: Übergabe an Audio
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
      // TODO: Übergabe an Audio
      retval = true;
    }
    // Set for radio
    if ( keyword == String("audio_radio") ) {
      modus = Radio;
      html_json = "{\"audio_radio\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      // TODO: Radio einschalten, alles ander aus
      retval = true;
    }
    if ( keyword == String("audio_radio_save_stn_name") ) {
      snprintf(station[audio_radio_cur_station].name,STATION_NAME_LENGTH,"%s",value.c_str());
      audio_radio_save_stations();
    }
    if ( keyword == String("audio_radio_save_stn_url") ) {
      snprintf(station[audio_radio_cur_station].url,STATION_URL_LENGTH,"%s",value.c_str());
      audio_radio_save_stations();
    }
    // Set for media
    if ( keyword == String("audio_media") ) {
      modus = Media;
      html_json = "{\"audio_media\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      // TODO: Media einschalten, alles ander aus
      retval = true;
    }
    // Set for speaker
    if ( keyword == String("audio_speak") ) {
      modus = Speaker;
      html_json = "{\"audio_speak\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      // TODO: Speaker einschalten, alles ander aus
      retval = true;
    }

  }
  return retval;
}

// Wofür ???
void AudioModul::set_modus(uint8_t _modus) {
  switch (_modus) {
    case 0:
      modus = Radio;
      set("audio_radio","1");
    break;
    case 1:
      modus = Media;
      set("audio_media","1");
    break;
    case 2:
      modus = Speaker;
      set("audio_speak","1");
    break;        
  }
}

uint8_t AudioModul::get_modus() {
  uint8_t retval = 0; 
  switch (modus) {
    case Radio:
      retval = 0;
    break;
    case Media:
      retval = 1;
    break;
    case Speaker:
      retval = 2;
    break;        
  }
  return retval;
}

void AudioModul::loop(time_t now) {
  if (modus != Off) {
#if defined(USE_AUDIO_LIB)
    audio.loop();
    if (!audio.isRunning()) {
      if (modus == Radio) {
        audio_off();
        audio_on();
      }
    }
#elif defined(USE_ESP8266AUDIO_LIB)
    if (!mp3->loop()) mp3->stop();
    // ToDo wieder neu starten
#endif
  }
  rotarymodul.loop();
  switch (rotarymodul.changed()) {
    case 1:
      // Der Rotary wurde gedreht
      if ( rotarymodul.curLevel() == 0 ) {
        audio_vol = rotarymodul.curValue();
        set("audio_vol",String(audio_vol));
        if ( ! obj_value && (audio_vol > 0) ) {
          set_switch(1);
          if ( modus == Radio ) audio_radio_show_station();
        }
        if ( obj_value && (audio_vol == 0) ) {
          set_switch(0);
          audiodisplay.show_time(true);
        }
      }
      // Level 1 wählt den Sender
      if ( rotarymodul.curLevel() == 1 ) {
        audio_radio_cur_station = rotarymodul.curValue();
        audio_radio_set_station();
      }
      // Level 2 wählt den Modus (Radio, Mediaplayer, Bluetoothspeaker)
      if ( rotarymodul.curLevel() == 2 ) {
        set_modus(rotarymodul.curValue());
        audiodisplay.show_modus(get_modus());
      }
    break;
    case 2:
      // Der Rotary wurde kurz gedrueckt
      if (obj_value) {
        Serial.printf("Rotary Level %u Position: %u Slider: %u\n", rotarymodul.curLevel(), rotarymodul.curValue(), audio_vol);
        switch(rotarymodul.curLevel()) {
          case 0:
            audio_radio_show_station();
          break;
          case 1:
            audio_radio_set_station();
          break;
          case 2:
            audiodisplay.show_modus(get_modus());
          break;
        }
      } else {
        rotarymodul.setLevel(0);
      }
    break;
    case 3: 
      // Der Rotary wurde lang gedrueckt
      set_switch(2);
      rotarymodul.setLevel(0);
      rotarymodul.setValue(0);
      set("audio_vol",String(0));
    break;
  }
  if ( changed() ) {
//    Serial.printf("Web Slider Position: %u \n", get_slider_val());
    if ( rotarymodul.curLevel() == 0 ) {
      rotarymodul.setValue( audio_vol );  
      Serial.printf("Rotary Level %u Position: %u Slider: %u\n", rotarymodul.curLevel(), rotarymodul.curValue(), audio_vol);
    }
  }
}

void AudioModul::audio_off() {
  write2log(LOG_MODULE,1,"Audio off");
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
  modus = Off;
}

void AudioModul::audio_on() {
//  modus = Radio;
  write2log(LOG_MODULE,1,"Audio on");
  if ( modus == Radio ) {
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
    audio_radio_show_station();
  }
// modus = Media
// todo  
// modus = Speaker
// todo  
}

/*********************************************************************************************************
 * 
 *  Ab hier alles fürs Radio
 * 
 * 
**********************************************************************************************************/

void AudioModul::audio_radio_station_json(String& json) {
  for (int i=0; i<MAXSTATIONS; i++) {
    json += ",\"audio_radio_add_stn\":\"";
    json += station[i].url;
    json += ";";
    json += station[i].name;
    json += "\"";
  }
}

void AudioModul::audio_radio_show_station() {
  audiodisplay.clear();
  audiodisplay.show_ip(WiFi.localIP().toString().c_str());
  audiodisplay.show_vol(audio_vol);
  audiodisplay.show_time(false);
  audiodisplay.show_station(station[audio_radio_cur_station].name);
}

void AudioModul::audio_radio_set_station() {
  audiodisplay.select_station(
    audio_radio_cur_station > 1 ? station[audio_radio_cur_station-2].name : "",
    audio_radio_cur_station > 0 ? station[audio_radio_cur_station-1].name : "",
    station[audio_radio_cur_station].name,
    audio_radio_cur_station < MAXSTATIONS ? station[audio_radio_cur_station+1].name : "",
    audio_radio_cur_station < MAXSTATIONS - 1 ? station[audio_radio_cur_station+2].name : ""
  );
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

//
// Die folgenden Funktionen ergänzen die Lib: ESP32-audioI2S
// Die Funktionsnamen sind dort festgelegt
//

void audio_info(const char *info){
    write2log(LOG_MODULE,2,"Info:", info);
}

void audio_showstreamtitle(const char *info){
    write2log(LOG_MODULE,2,"Titel:", info);
    if (rotarymodul.curLevel() == 0) audiodisplay.show_title(info);
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
    if (rotarymodul.curLevel() == 0) audiodisplay.show_station(info);
    html_json = "{\"audiomsg1\":\"";
    html_json += info;
    html_json += "\"}";
    write2log(LOG_WEB,1,html_json.c_str());
    ws.textAll(html_json);
}


#endif