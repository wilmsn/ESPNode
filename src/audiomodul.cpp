#include "config.h"
#ifdef USE_AUDIOMODUL

#include "audiomodul.h"
#include "common.h"

/// @brief Instances for the Preferences is defined in main.h
extern Preferences preferences;
/// @brief Instance for websockets is defined in webserver.h
extern AsyncWebSocket ws;

RotaryModul rotarymodul;

void AudioModul::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword)  {
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, true, true);
  set_slider_max_value(100);
  set_slider(20);
  rotarymodul.begin();
  rotarymodul.initLevel(0,0,20,100);
  rotarymodul.initLevel(1,0,0,10);
  rotarymodul.initLevel(2,0,0,2);
  audio_vol = 20;
  audio_bas = 0;
  audio_tre = 0;
  modus = Radio;
  set("radio","1");
}

void AudioModul::html_create_json_part(String& json) {
  Switch_OnOff::html_create_json_part(json);
  json += ",\"show_audio\":1";
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
      retval = true;
    }
    if ( keyword == String("audio_radio") ) {
      html_json = "{\"audio_radio\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      retval = true;
    }
    if ( keyword == String("audio_media") ) {
      html_json = "{\"audio_media\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      retval = true;
    }
    if ( keyword == String("audio_speak") ) {
      html_json = "{\"audio_speak\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      retval = true;
    }
    if ( keyword == String("audio_menu") ) {
      html_json = "{\"audio_speak\":1}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
      retval = true;
    }

  }
  return retval;
}

void AudioModul::set_modus(uint8_t _modus) {
  switch (_modus) {
    case 0:
      modus = Radio;
      set("radio","1");
    break;
    case 1:
      modus = Media;
      set("media","1");
    break;
    case 2:
      modus = Speaker;
      set("speak","1");
    break;        
  }
}


void AudioModul::loop() {
  rotarymodul.loop();
  switch (rotarymodul.changed()) {
    case 1:
      if ( rotarymodul.curLevel() == 0 ) {
        audio_vol = rotarymodul.curValue();
        set("audio_vol",String(audio_vol));
        if ( ! obj_value && (audio_vol > 0) ) set_switch(1);
        if ( obj_value && (audio_vol == 0) ) set_switch(0);
      }
      // Level 2 wählt den Modus (Radio, Mediaplayer, Bluetoothspeaker)
      if ( rotarymodul.curLevel() == 2 ) {
        set_modus(rotarymodul.curValue());
      }
    break;
    case 2:
      if (obj_value) {
        Serial.printf("Rotary Level %u Position: %u Slider: %u\n", rotarymodul.curLevel(), rotarymodul.curValue(), audio_vol);
      } else {
        rotarymodul.setLevel(0);
      }
    break;
    case 3: 
//      Serial.printf("Rotary long press \n");
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

#endif