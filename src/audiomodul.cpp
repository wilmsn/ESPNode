#include "config.h"
#ifdef USE_AUDIOMODUL
#include "audiomodul.h"
#include "common.h"
#include "Audio.h"

#ifndef KLICK_TIMEOUT
#define KLICK_TIMEOUT       20
#endif

#ifdef USE_AUDIODISPLAY_GC9A01A
#define USE_AUDIODISPLAY
#include "audiodisplay_GC9A01A.h"
AudioDisplay  audiodisplay(TFT_CS, TFT_DC, TFT_ROT);
#endif
#include <TJpg_Decoder.h>


String audio_radio_stationname;
String audio_radio_streamtitle;
String audio_kbs;

#ifdef USE_ROTARY

#include "AiEsp32RotaryExtention.h"

AiEsp32RotaryExtention  rotary = AiEsp32RotaryExtention(ROT_S1, ROT_S2, ROT_SW);
//AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(ROT_S1, ROT_S2, ROT_SW, -1, 4);

void IRAM_ATTR intrSRV() {
    rotary.readEncoder_ISR();
}
#endif

#ifdef USE_AUDIODISPLAY
uint16_t* bmpBuffer;

// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  for(uint8_t xb=0; xb<w; xb++){
    for(uint8_t yb=0; yb<h; yb++){
      bmpBuffer[x+xb+((y+yb)*160)]=bitmap[xb+(yb*h)];
    }
  }
  audiodisplay.drawRGBBitmap(x+80, y+10, bitmap, w, h);
  return 1;
}
#endif




/// @brief Instance for audio (I2S and decoder) device
Audio            audio;

void AudioModul::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword)  {
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, false, true, true);
  uint8_t this_app;
  uint8_t this_lev;
  html_info = "";
  html_has_info = true;
#ifdef DEBUG_SERIAL
  Serial.println("Auiomodul begin");
#endif
#ifdef USE_ROTARY
  rotary.areEncoderPinsPulldownforEsp32=false;
  rotary.begin();
  rotary.setup(intrSRV);
  rotary.setBoundaries(0, 100, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotary.setAcceleration(250);
  this_app = rotary.app_add(0,100,0);
  this_lev = rotary.lev_add(this_app,0,MAXSTATIONS-1,0);
  this_lev = rotary.lev_add(this_app,0,1,0);
  if (html_info.length() > 2 ) html_info += String(",");
  html_info += String("\"tab_head_rotary\":\"Rotary\"") +
               String(",\"tab_line1_rotary\":\"A-Pin:#GPIO: ") + String(ROT_S1)+ String("\"") +
               String(",\"tab_line2_rotary\":\"B-Pin:#GPIO: ") + String(ROT_S2)+ String("\"") +
               String(",\"tab_line3_rotary\":\"SW-Pin:#GPIO: ") + String(ROT_SW)+ String("\"");
#endif
#ifdef USE_AUDIO_RADIO
  audio_radio_load_stations();
  write2log(LOG_MODULE,1,"Radio Stations loaded");
#endif
  if (audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT)) {
    write2log(LOG_MODULE,6,"Init I2S: BCLK:",String(I2S_BCLK).c_str()," LRC:",String(I2S_LRC).c_str()," OUT:",String(I2S_DOUT).c_str());
  } else {
    write2log(LOG_SYSTEM,6,"ERROR I2S: BCLK:",String(I2S_BCLK).c_str()," LRC:",String(I2S_LRC).c_str()," OUT:",String(I2S_DOUT).c_str());
  }
  //audio.setBufsize(30000,600000);
  audio.setVolumeSteps(100);
  audio.setVolume(audio_vol);

//  audio.setAudioTaskCore(1);
  if (html_info.length() > 2) html_info += String(",");
  html_info += String("\"tab_head_audio\":\"I2S: MA98357\"") +
               String(",\"tab_line1_audio\":\"DOUT:#GPIO: ") + String(I2S_DOUT)+ String("\"") +
               String(",\"tab_line2_audio\":\"BCLK:#GPIO: ") + String(I2S_BCLK)+ String("\"") +
               String(",\"tab_line3_audio\":\"LRC:#GPIO: ") + String(I2S_LRC)+ String("\"");
#ifdef USE_AUDIODISPLAY                   
  html_info += audiodisplay.html_info;
#endif
  audio_set_mode(Off);  
#ifdef USE_AUDIO_SPEAKER
  default_mode = Speaker;
#endif
#ifdef USE_AUDIO_MEDIA
  default_mode = Media;
#endif
#ifdef USE_AUDIO_RADIO
  default_mode = Radio;
#endif

#ifdef USE_AUDIO_MEDIA
  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(JPG_SCALE);
  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(false);
  // The decoder must be given the exact name of the mcu buffer function above
  TJpgDec.setCallback(tft_output);
#endif
  write2log(LOG_MODULE,1,"End audiomodul.begin()");
#ifdef DEBUG_SERIAL
  Serial.println(html_info);
  Serial.println("Auiomodul begin ende");
#endif
}

bool AudioModul::set(const String& _cmnd, const String& _val) {
  bool retval = false;
  String myvalue = _val;
  if ( Switch_OnOff::set(_cmnd, _val) ) {
    // Ein- und Ausschalten erfolgt im Elternobjekt
    // Schalter ist aus und mode ist nicht Off
    if ((! switch_value) && (mode != Off)) { audio_set_mode(Off); 
    // Schalter ist ein und mode ist Off
    } else if (( switch_value) && (mode == Off)) { audio_vol = 3; audio_set_mode(default_mode); 
    // Schalter normal
    } else { html_upd_data(); }
    retval = true;
  } else {  
    std::replace(myvalue.begin(),myvalue.end(),'\n',' ');
    // Lautstärke ändern
    if ( _cmnd == String("audio_vol") ) {
      // Sonderbehandlung: Lautstärke kleiner 2 schaltet die Anlage aus.
      if (audio_vol >= 2 && myvalue.toInt() < 2) if (mode != Off) audio_set_mode(Off);
      // Sonderbehandlung: Lautstärke größer 2 schaltet die Anlage ein.
      if (audio_vol <= 2 && myvalue.toInt() > 2) { if (mode == Off) audio_set_mode(default_mode); }
      audio_vol = myvalue.toInt();
      // Audiomodul einstellen
      audio.setVolume(audio_vol);
      // Weboberfläche einstellen
      html_upd_data();
#ifdef USE_ROTARY
      // Rotarymodul einstellen - nur wenn Änderungen nicht von dort kommen!
      if (! change_from_rotary) {
        rotary.val_set(audio_vol);
#ifdef DEBUG_SERIAL
        Serial.printf("audio_set_app: case Radio => Rotary app: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
      }
#endif
#ifdef USE_AUDIODISPLAY
      // Lautstärke auf Display anzeigen
      audiodisplay.vol(audio_vol);
#endif
      retval = true;
    }
#ifdef USE_AUDIO_RADIO
    // Set for radio
    // Radio einschalten => Wiiedergabe
    if ( _cmnd == String("audio_radio") ) {
      if ( audio_vol < 3 ) audio_vol = 3;
      if ( mode != Radio ) audio_set_mode(Radio);
      retval = true;
    }
    // Radio: Sender einstellen
    if ( _cmnd == String("audio_radio_set_stn") ) {
      for (uint8_t i=0; i<MAXSTATIONS; i++) {
        if (strcmp(audio_radio_station[i].url, myvalue.c_str()) == 0) {
          audio_radio_cur_station = i;    
          retval = true;
        }
      }
      html_upd_data();
      audio_radio_send_stn2web();
      audio_radio_on();
#ifdef USE_AUDIODISPLAY
      audiodisplay.radio_station(audio_radio_station[audio_radio_cur_station].name);
#endif
      html_upd_data();
    }
    // Radio: Sender Name speichern ueber Webinterface
    if ( _cmnd == String("audio_radio_save_stn_name") ) {
      snprintf(audio_radio_station[audio_radio_cur_station].name,STATION_NAME_LENGTH,"%s",_val.c_str());
      audio_radio_save_stations();
//      audio_radio_send_stn2web();
//      audio_web_stat();
        }
    // Radio: Sender URL speichern ueber Webinterface
    if ( _cmnd == String("audio_radio_save_stn_url") ) {
      snprintf(audio_radio_station[audio_radio_cur_station].url,STATION_URL_LENGTH,"%s",_val.c_str());
      audio_radio_save_stations();
//      audio_radio_send_stn2web();
//      audio_web_stat();
    }
#endif
#ifdef USE_AUDIO_MEDIA
    // Set for mediaplayer
    // Radio einschalten => Wiiedergabe
    if ( _cmnd == String("audio_media") ) {
      if ( mode != Media ) audio_set_mode(Media);
//      audio_media_play(audio_media_cur_dir, audio_media_cur_file);
      retval = true;
    }
    if ( _cmnd == String("audio_media_sel_album") ) {
      audiodisplay.screen_media();
      rotary.app_set(2,1);
      rotary.max_set(100); //Durch die maximale Anzahl der Alben ersetzen
//      show_album(value.toInt());
//      audio_set_app(Media);
      retval = true;
    }
    if ( _cmnd == String("audio_media_sd_init") ) {
      audiodisplay.screen_media_update();
      audio_media_start_update();
      rotary.app_set(2,0);
//      audio_media_sd_init_file = -1;
//      audio_media_sd_init_do = true;
    }
#endif
  }
  mode_changed = false;
  return retval;
}

void AudioModul::html_init() {
  Switch_OnOff::html_init();
  if (html_json.length() > 2) html_json += String(",");
  html_json += String("\"audio_show\":1,\"audio_trebas_enable\":0,\"audio_media_active\":1,\"audio_settings\":1");
  #ifdef USE_AUDIO_RADIO
  html_json += String(",\"audio_radio_show\":1,\"audio_radio\":");
  if (mode == Radio) {
    html_json += String("1");
    for (int i=0; i<MAXSTATIONS; i++) {
      html_json += String(",\"audio_radio_add_stn_") + String(i) + String("\":\"") + 
                   String(audio_radio_station[i].url) +
                   String(";") + String(audio_radio_station[i].name) + String("\"");
    }
    html_json += String(",\"audiomsg1\":\"") + audio_radio_stationname + String("\"") +
                 String(",\"audiomsg2\":\"") + audio_radio_streamtitle + String("\"") +
//                 String(",\"audiomsg3\":\"") + audiomsg3 + String("\"") +
                 String(",\"audiomsg4\":\"") + audio_kbs + String("\"");

  } else { html_json += String("0"); }
  #else
  html_json += String(",\"audio_radio_show\":0");
  #endif
  #ifdef USE_AUDIO_MEDIA
  html_json += String(",\"audio_media_show\":1");
  #else
  html_json += String(",\"audio_media_show\":0");
  #endif
  #ifdef USE_AUDIO_SPEAKER
  html_json += String(",\"audio_speak_show\":1");
  #else
  html_json += String(",\"audio_speak_show\":0");
  #endif
  html_json_filled = true;
}

void AudioModul::html_upd_data() {
  // Ein- Ausschalter der Anlage
  html_json = String("\"") + html_place + String("\":") + String(switch_value?"1":"0");
  html_json += String(",\"audio_vol\":") + String(audio_vol);
  switch (mode) {
#ifdef USE_AUDIO_RADIO
    case Radio:
      if ( mode_changed ) html_json += String(",\"audio_radio\":1");
    break;
#endif
    default:
      html_json += String(",\"audio_radio\":0");
  }
  html_update();
}

String AudioModul::print_mode(mode_t mymode) {
  String tmpstr;
  switch(mode) {
    case Off:
      tmpstr = "Off";
      break;
    case Radio:
      tmpstr = "Radio";
      break;
    case Media:
      tmpstr = "Media";
      break;
    case Speaker:
      tmpstr = "Speaker";
      break;
    default:
      tmpstr = "???";
      break;
  }
  return tmpstr;
}

void AudioModul::audio_set_mode(mymode_t new_mode) {
#ifdef DEBUG_SERIAL
  Serial.println("audio_set_mode:");
  Serial.println(String("New Mode: ") + print_mode(new_mode)); 
  Serial.println(String("Mode: ") + print_mode(mode)); 
  Serial.println(String("Last Mode: ") + print_mode(last_mode)); 
  Serial.println(String("Default Mode: ") + print_mode(default_mode)); 
#endif
  if ( new_mode != mode ) {
    mode_changed = true;
    if (new_mode == Radio || new_mode == Media || new_mode == Speaker) default_mode = new_mode;
    last_mode = mode;
    mode = new_mode;
    switch (mode) {
#ifdef USE_AUDIO_RADIO
      case Radio:
        write2log(LOG_MODULE,1,"audio_set_app: case Radio");
#ifdef USE_ROTARY
      rotary.app_set((uint8_t)Radio,0);
      rotary.val_set(audio_vol);
#ifdef DEBUG_SERIAL
      Serial.println(audio_vol);
      Serial.printf("audio_set_app: case Radio => Rotary app: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
#endif
        if (! switch_value ) do_switch(true);
        audio_radio_on();
      // Radio: Wiedergabe im Display
#ifdef USE_AUDIODISPLAY
      audiodisplay.screen_radio();
      audiodisplay.vol(audio_vol);
      audiodisplay.radio_station(audio_radio_station[audio_radio_cur_station].name);
#endif
      // Weboberfläche einstellen
        html_upd_data();
      break; //Radio
#endif  //USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
    case Media:
      audiodisplay.screen_media();
      rotary.app_set(2,0);
      rotary.max_set(100);
    break;
    case MusicUpdate:
      audiodisplay.screen_media_update();
      Serial.println("audio_set_app: MusicUpdate");
//      audio_media_init_sd();
//      audiodisplay.screen_media();
      rotary.app_set(2,0);
      rotary.max_set(100);
    break; // Media
#endif //USE_AUDIO_MEDIA
    case Off: 
    default:
      write2log(LOG_MODULE,1,"audio_set_app: case Off");
#ifdef USE_ROTARY
      rotary.app_set(0,0);
#endif
      audio_vol=0;
      do_switch(false);
/*      audiomsg1 = "";
      audiomsg2 = "";
      audiomsg3 = "";
      audiomsg4 = "";
      audiomsg5 = ""; */

#ifdef USE_AUDIODISPLAY
      audiodisplay.screen_off();              
#endif
      html_upd_data();
    break; // Off
    } //switch
  }
}

void AudioModul::audio_off() {
  switch(mode) {
    case Radio:
      audio_radio_off();
    break;
  }
}

void AudioModul::start_timeout() {
  timeout_set = true;
  timeout_start = now;
}

void AudioModul::loop(time_t now) {
#ifdef USE_AUDIODISPLAY  
  audiodisplay.loop(now);
#endif
  if (mode != Off) {
    audio.loop();
    if (!audio.isRunning()) {
#ifdef USE_AUDIO_RADIO
      if (mode == Radio) {
        audio_radio_off();
        audio_radio_on();
      }
#endif
    }
  }
#ifdef USE_ROTARY
  rotary.loop(now);
  // Hier wird der Klickstream definiert
  if ( timeout_set ) {
    if ((now - timeout_start) > KLICK_TIMEOUT) {
        //TODO: Anzeige zurücksetzen
        rotary.app_set(rotary.app(),0);
        timeout_set = false;
        switch (rotary.app()) {
          case Radio:
            audiodisplay.screen_radio();
          break;
        }
      }
  }
  if (rotary.valChanged()) {
#ifdef DEBUG_SERIAL
    Serial.printf("Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
    switch(rotary.app()) {
      case Off:
          if (rotary.lev() == 0) {
            if (rotary.val() > 1) {
              set(String("audio_radio"),String("1"));
            }
          } 
      break;
      case Radio:
          switch(rotary.lev()) {
            case 0:
          // Lautstärke einstellen
              change_from_rotary = true;
              set("audio_vol",String(rotary.val()));
              change_from_rotary = false;
#ifdef DEBUG_SERIAL
              Serial.println(rotary.val());
#endif
            break;
            case 1:
          // Sender Auswahl
              start_timeout();
#ifdef USE_AUDIODISPLAY
              audiodisplay.radio_select_station(
                          rotary.val() > 0 ? audio_radio_station[rotary.val()-1].name : "",
                          audio_radio_station[rotary.val()].name,
                          rotary.val() < MAXSTATIONS ? audio_radio_station[rotary.val()+1].name : "" );
#endif
              rot_last_val = rotary.val();
            break;
          }
      break;
      case Media:
        switch(rotary.lev()) {
          case 0:
        // Lautstärke einstellen
            change_from_rotary = true;
            set("audio_vol",String(rotary.val()));
            change_from_rotary = false;
          break;
          case 1:
        // Album wechseln
            start_timeout();
            set(String("audio_media_sel_album"),String(rotary.val()));
            rot_last_val = rotary.val();
        }
      break;
      case Settings:
#ifdef DEBUG_SERIAL
        Serial.printf("---Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
        start_timeout();
          switch(rotary.lev()) {
            case 0:
            // App oder Eigenschaft auswählen
  /*          Serial.printf("-1-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
            Serial.printf("-x-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
            Serial.printf("-x-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
            Serial.printf("-x-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
            Serial.printf("-x-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
            Serial.printf("-x-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
            Serial.printf("-x-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
            Serial.printf("-x-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
  */
 #ifdef USE_AUDIODISPLAY
              switch(rotary.val()) {
                case 0:
 //                 audiodisplay.show_set_icon("Off");
                break;
                case 1:
 //                 audiodisplay.show_set_icon("Radio");
                break;
                case 2:
 //                 audiodisplay.show_set_icon("Media");
#ifdef DEBUG_SERIAL
                  Serial.println("Media select");
                  Serial.printf("-1a-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
                  break;
                case 3:
  //                audiodisplay.show_set_icon("Speaker");
                break;
                case 4:
  //                audiodisplay.show_set_icon("Settings");
                break;
                case 5:
  //                audiodisplay.show_set_icon("MusicUpdate");
                break;
              }
#endif
              rot_last_val = rotary.val();
#ifdef DEBUG_SERIAL
              Serial.printf("-2-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
              break;
            case 1: 
            // Auswahl bestätigen
              switch(rot_last_val) {
                case 0:
                  set(keyword,String("0"));
                break;
                case 1:
                  set(String("audio_radio"),String("1"));
                break;
                case 2:
#ifdef DEBUG_SERIAL
                  Serial.println("Media set");
#endif
                  //set(String(AUDIO_MEDIA),String("1"));
                break;
                case 3:
//                  audiodisplay.show_set_icon("Speaker");
                break;
                case 4:
//                  audiodisplay.show_set_icon("Settings");
                break;
                case 5:
//                  audiodisplay.show_set_icon("MusicUpdate");
                break;
              }
  //            rot_last_val = rotary.val();
            break;
          }
      break;
    }
#ifdef DEBUG_SERIAL
    Serial.printf(">>>Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
  }  //Ende rotary.valChanged
  if (rotary.buttonShortPressed) {
    uint8_t new_lev;
    rotary.buttonShortPressed = false;
#ifdef DEBUG_SERIAL
    Serial.printf("Rotary short Pressed App: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.get_max_lev(rotary.app()), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
    start_timeout();
    switch(rotary.app()) {
      break;
      case Radio:
        new_lev = rotary.lev_up();
#ifdef DEBUG_SERIAL
        Serial.printf("Rotary nev Lev %u\n", new_lev);
#endif
        switch(new_lev) {
          // Radio: Neuen Sender auswählen
          case 1:
//            Serial.println("Radio Senderwahl");
            rotary.val_set(audio_radio_cur_station);
#ifdef USE_AUDIODISPLAY
            audiodisplay.screen_radio_select();
            audiodisplay.radio_select_station(
                          audio_radio_cur_station > 0 ? audio_radio_station[audio_radio_cur_station-1].name : "",
                          audio_radio_station[audio_radio_cur_station].name,
                          audio_radio_cur_station < MAXSTATIONS ? audio_radio_station[audio_radio_cur_station+1].name : "" );
#endif
          break;
          // Radio: Neuen Sender übernehmen und Level zurück auf 0 => Play
          case 2:
#ifdef DEBUG_SERIAL
            Serial.print("Radio neuer Sender: ");
            Serial.println(rot_last_val);
            Serial.print("Radio alter Sender: ");
            Serial.println(audio_radio_cur_station);
#endif
            if (audio_radio_cur_station != rot_last_val) {
              set(String("audio_radio_set_stn"),String(rot_last_val));
              audio_radio_cur_station = rot_last_val;
              audio_radio_play();
#ifdef USE_AUDIODISPLAY
              audio_radio_stationname = String("");
              audiodisplay.radio_streamtitle(audio_radio_stationname);
              audiodisplay.screen_radio();
#endif
#ifdef USE_ROTARY
              rotary.app_set(rotary.app(),0);
#endif
            }
            break;
          }
      break;
      case Media:
        switch(rotary.lev_up()) {
          // Media: Album auswählen
          case 1:
            set(String("audio_media_sel_album"),String("1"));
            //rot_last_val = rotary.val();
          break;
          // Media: Musikstück auswählen
          case 2:
            rotary.app_set(rotary.app(),0);
          break;
        }
      break;
      case Settings:
        switch(rotary.lev_up()) {
          // Off: Ausschalten
          case 1:
            switch(rot_last_val){
              case Off:
                set(keyword, "0");
              break;
#ifdef USE_AUDIO_RADIO
              case Radio:
                set(String("audio_radio"),String("1"));
              break;
#endif
#ifdef USE_AUDIO_MEDIA
              case Media:
                set(String("audio_media"),String("1"));
              break;
              case MusicUpdate:
                set(String("audio_media_sd_init"),String("1"));
              break;
#endif
              default:
#ifdef DEBUG_SERIAL
                Serial.printf("Shortpress case: %u\n",rot_last_val);
#endif
              break;
            }
          break;
        }
      break;
    }
  }
  if (rotary.buttonLongPressed) {
      rotary.buttonLongPressed = false;
#ifdef DEBUG_SERIAL
      Serial.printf("Rotary Long Pressed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
      start_timeout();
      rotary.app_set(Settings,0,0,LastApp-1,mode);
//      rotary.app_set(Settings,0);
#ifdef USE_AUDIODISPLAY
      audiodisplay.screen_settings();
      switch (mode) {
        case 0:
 //         audiodisplay.show_set_icon("Off");
        break;
        case 1:
 //         audiodisplay.show_set_icon("Radio");
        break;
        case 2:
 //         audiodisplay.show_set_icon("Media");
        break;
      }
#endif
      rot_last_val = mode;
  }
#endif //USE_ROTARY
// Ende Klickstream Definition
// Update der Media Data
#ifdef USE_AUDIO_MEDIA
  if (audio_media_do_update) {
    if (! audio_media_update_running) {
      audio_media_update_lowstr = (char*)malloc(SD_DIR_LENGTH);
      audio_media_update_highstr = (char*)malloc(SD_DIR_LENGTH);
      memset(audio_media_update_lowstr,0,SD_DIR_LENGTH);
      memset(audio_media_update_highstr,0,SD_DIR_LENGTH);
      sd_root = SD.open("/");
      sd_out = SD.open("album.txt",FILE_WRITE);
    }
    if (sd_root) {
      sd_root.rewindDirectory();
      sd_dir = sd_root.openNextFile();
      while (sd_dir) {
        if (sd_root.isDirectory()) {
          if ( audio_media_sort(audio_media_update_lowstr, audio_media_update_highstr, sd_dir.name()) ) {
            audio_media_update_found = true;
            strcpy(audio_media_update_highstr, sd_dir.name());
          }
        } else {
          sd_dir = sd_root.openNextFile();
        }
      }
      sd_out.println(audio_media_update_lowstr);
      strcpy(audio_media_update_lowstr, audio_media_update_highstr);
      memset(audio_media_update_highstr,0,SD_DIR_LENGTH);
    }
  }
  if ( ! audio_media_update_found ) {
    sd_root.close();
    sd_dir.close();
    free(audio_media_update_lowstr);
    free(audio_media_update_highstr);
    audio_media_do_update = false;
  }
#endif //USE_AUDIO_MEDIA
}

/************************************************************************************
// Die folgenden Funktionen ergänzen die Lib: ESP32-audioI2S
// Die Funktionsnamen sind dort festgelegt
************************************************************************************/

void audio_info(const char *info){
/*  String tmpstr;
  int found = String(info).indexOf("BitRate");
  Serial.printf("Info: %s Bitrate @ %u\n",info,found);
  if ( found >= 0) {
    tmpstr = String("{\"audiomsg4\":\"B: ") + String(info).substring(found) + String(" KBps\"}");
    write2log(LOG_MODULE,1,tmpstr.c_str());
    ws.textAll(tmpstr.c_str());
  }
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
    }
    if (strlen(str2) == 5) {
      str2[2]=' ';
      str2[3]='K';
      str2[4]='B';
      str2[5]='s';
      str2[6]=0;
    }
#ifdef USE_AUDIODISPLAY                   
    audiodisplay.set_bps(str2);
#endif
  }*/
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
#ifdef USE_AUDIODISPLAY                   
//    audiodisplay.radio_streamtitle(str2);
#endif
    tmpstr += String("\"audiomsg1\":\"Artist: ")+String(str2)+String("\"");
  }
  if (info[0] == 'T' && info[1] == 'i' && info[2] == 't') {
#ifdef USE_AUDIODISPLAY                   
//    audiodisplay.radio_streamtitle(str2);
#endif
    if (tmpstr.length() > 5) tmpstr += ",";
    tmpstr += String("\"audiomsg2\":\"Title: ")+String(str2)+String("\"");
  }
  if (info[0] == 'A' && info[1] == 'l' && info[2] == 'b') {
    if (tmpstr.length() > 5) tmpstr += ",";
    tmpstr += String("\"audiomsg3\":\"Album: ")+String(str2)+String("\"");
  }
  tmpstr += "}";
  if (tmpstr.length() > 5) {
    ws.textAll(tmpstr);
    write2log(LOG_MODULE,1,tmpstr.c_str());
  }
}

void audio_showstreamtitle(const char *info){
  String tmpstr;
#ifdef USE_AUDIODISPLAY                   
  tmpstr = String(info);
  audiodisplay.radio_streamtitle(tmpstr);
#endif
  audio_radio_streamtitle = String(info);
  tmpstr = String("{\"audiomsg2\":\"") + String(info) + "\"}";
  write2log(LOG_MODULE,1,tmpstr.c_str());
  ws.textAll(tmpstr.c_str());
}

void audio_bitrate(const char *info) {
  String tmpstr;
  tmpstr = String("{\"audiomsg4\":\"") + String(info).substring(0,3) + String(" KBps") + String("\"}");
  write2log(LOG_MODULE,1,tmpstr.c_str());
  ws.textAll(tmpstr.c_str());
#ifdef USE_AUDIODISPLAY
  tmpstr = String(info).substring(0,3) + String(" KBps");
  audiodisplay.radio_bps(tmpstr.c_str());
#endif
}

void audio_showstation(const char *info){
  String tmpstr;
  audio_radio_stationname = String(info);
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
  audio.stopSong();
  ws.textAll("{\"audio_radio\":0}");
}

void AudioModul::audio_radio_on() {
  ws.textAll("{\"audio_radio\":1}");
  write2log(LOG_MODULE,1,"Radio on");
  audio_radio_play();
}

void AudioModul::audio_radio_play() {
  if ( strlen(audio_radio_station[audio_radio_cur_station].url) > 10 ) {
    audio.connecttohost(audio_radio_station[audio_radio_cur_station].url);
    write2log(LOG_MODULE,2,"Switch to ",audio_radio_station[audio_radio_cur_station].url);
#ifdef USE_AUDIODISPLAY
    audiodisplay.screen_radio();
    audiodisplay.radio_station(audio_radio_station[audio_radio_cur_station].name);
#endif
  }
}

void AudioModul::audio_radio_send_stn2web() {
  for (int i=0; i<MAXSTATIONS; i++) {
    String html_json = String("{\"audio_radio_del_stn\":1") +
                       String("{\"audio_radio_add_stn") + String(i) + String("\":\"") +
                       String(audio_radio_station[i].url) + String(";") +
                       String(audio_radio_station[i].name) + String("\"}");
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
  }
}

void AudioModul::audio_radio_load_stations() {
  File f = LittleFS.open( "/sender.txt", "r" );
  if (f) {
    for (int i=0; i<MAXSTATIONS; i++) {
      snprintf(audio_radio_station[i].name,STATION_NAME_LENGTH,"%s",f.readStringUntil('\n').c_str());
      snprintf(audio_radio_station[i].url,STATION_URL_LENGTH,"%s",f.readStringUntil('\n').c_str());
      write2log(LOG_MODULE,2,audio_radio_station[i].name,audio_radio_station[i].url);
    }
    f.close();
  }
}

void AudioModul::audio_radio_save_stations() {
  File f = LittleFS.open( "/sender.txt", "w" );
  if (f) {
    for (int i=0; i<MAXSTATIONS; i++) {
      write2log(LOG_MODULE,3,"Save Station: ",audio_radio_station[i].name,audio_radio_station[i].url);
      f.printf("%s\n",audio_radio_station[i].name);
      f.printf("%s\n",audio_radio_station[i].url);
    }
    delay(5);
    f.close();
  }
  audio_radio_send_stn2web();
}

#endif  //USE_AUDIO_RADIO

/*********************************************************************************************************
 * 
 *  Ab hier alles für den Mediaplayer
 * 
 * 
**********************************************************************************************************/
#ifdef USE_AUDIO_MEDIA

void AudioModul::audio_media_on() {
//  audio_media_play(audio_media_cur_dir,audio_media_cur_file);
//  audiodisplay.screen(Disp_Media);
  ws.textAll("{\"audio_media\":1}");
//  write2log(LOG_MODULE,2,"Anzahl Songs: ",String(allSongs).c_str());
}

void AudioModul::audio_media_off() {
  ws.textAll("{\"audio_media\":0}");
}
/*
void AudioModul::audio_media_play(uint16_t _albumNo, uint16_t _songNo) {
  String sdName = "";
  char* dirName;
  char* fileName;
  bool dir_found = false;
  bool file_found = false;
  sdName = String("/")+String(dirName)+String("/")+String(fileName);
  if (sdName.length() > 5) audio.connecttoFS(SD,sdName);
  audiodisplay.show_info1(" ");
  audiodisplay.show_info2(" ");
  song_started = 0; 
  char dirname[125];
  audio_media_sd_get_dir(_albumNo, dirname);
  Serial.printf(">>>audio_media_play: %s",dirname);
}
*/

//TODO: Prüfen ob eigenständige Funktion benötigt wird. Ggf. Inhalte in die set Funktion verschieben.
void AudioModul::audio_media_start_update() {
  audio_media_do_update = true;
  audio_off();
}

/* Sortiert der übergebenen Strings
 * Es wird der String gesucht der aufsteigend sortiert der nächst größere zu S0 ist.
 * s0: der Referenzstring.
 * s1: der aktuell nächste String nach dem Referenzstring
 * s2: der zu testende String
 *
 * Faelle:
 * 0) Folgende Dateien werden nicht beruecksichtigt: ".*" (Dateien deren Name miteinem Punkt anfaengt)
 * 1) Ist strlen(s0) == 0 und strlen(s1) == 0 dann ist der Rückgabewert true; 
 * 2) Ist strlen(s0) == 0 und strlen(s1) > 0 und s1 > s2 dann ist der Rückgabewert true;
 * 3) Ist strlen(s0) == 0 und strlen(s1) > 0 und s1 <= s2 dann ist der Rückgabewert false;
 * 4) Ist strlen(s0) > 0 und strlen(s1) == 0 und s0 < s2 dann ist der Rückgabewert true;
 * 5) Ist strlen(s0) > 0 und strlen(s1) == 0 und s0 >= s2 dann ist der Rückgabewert false;
 * 6) Ist strlen(s0) > 0 und s0 < s2 und s1 > s2 dann ist der Rückgabewert true;
 * 7) Ist strlen(s0) > 0 und s0 < s2 und s1 <= s2 dann ist der Rückgabewert false;
 * 8) Ist strlen(s0) > 0 und s0 >= s2 dann ist der Rückgabewert false;
 */
bool AudioModul::audio_media_sort(const char* s0, const char* s1, const char* s2) {
  if ( (s2[0] == '.' ) ) {
    return false;  // Fall 0
  }
  if ( strlen(s0) == 0) {
    if ( strlen(s1) == 0) {
      return true;  // Fall 1
    } else {
      if ( strcmp(s1, s2) > 0 ) {
        return true;  // Fall 2
      } else {
        return false;  // Fall 3
      }
    }
  } else {
    if ( strlen(s1) == 0 ) {
      if ( strcmp(s0,s2) < 0 ) {
        return true;  // Fall 4
      } else {
        return false;  // Fall 5
      }        
    } else {
      if ( strcmp(s0, s2) < 0) {
        if ( strcmp(s1, s2) > 0 ) {
          return true;  // Fall 6
        } else {
          return false;  // Fall 7
        }
      } else {
        return false;  // Fall 8
      }
    }
  }
}

#endif //USE_AUDIO_MEDIA

#endif  //USE_AUDIOMODUL
