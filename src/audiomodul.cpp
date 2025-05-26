#include "config.h"
#ifdef USE_AUDIOMODUL
#include "audiomodul.h"
#include "common.h"
#include "Audio.h"

#ifdef USE_AUDIODISPLAY_GC9A01A
#define USE_AUDIODISPLAY
#include "audiodisplay_GC9A01A.h"
AudioDisplay  audiodisplay(TFT_CS, TFT_DC, TFT_ROT);
#endif


#ifdef USE_ROTARY
#include "rotarylib4ESP32.h"
RotaryLibMulti  rotary;

void IRAM_ATTR intrSRV() {
  rotary.read();
}
#endif

/// @brief Variable zur Steuerung der Anzeige Steaminhalte im Display
String     audiomsg1;
String     audiomsg2;
String     audiomsg3;
String     audiomsg4;
String     audiomsg5;



/// @brief Instance for audio (I2S and decoder) device
Audio            audio;



void AudioModul::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword)  {
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, false, true, true);
  uint8_t this_app;
  html_info = "";
#ifdef USE_ROTARY
  pinMode(ROT_S1, INPUT_PULLUP);
  pinMode(ROT_S2, INPUT_PULLUP);
  pinMode(ROT_SW, INPUT_PULLUP);
  attachInterrupt(ROT_S1, intrSRV, CHANGE);
  attachInterrupt(ROT_S2, intrSRV, CHANGE);
  attachInterrupt(ROT_SW, intrSRV, CHANGE);
  rotary.begin(ROT_S1,ROT_S2,ROT_SW);
  //Damit das Gerät per Rotarydrehung eingeschaltet werden kann muss app 0, lev 0 (mit begin() installiert)
  //Werte von 0 bis 10 zulassen.
  rotary.app_set(0,0,0,10,0);
  if (html_info.length() > 2 ) html_info += String(",");
  html_info += String("\"tab_head_rotary\":\"Rotary\"") +
               String(",\"tab_line1_rotary\":\"A-Pin:#GPIO: ") + String(ROT_S1)+ String("\"") +
               String(",\"tab_line2_rotary\":\"B-Pin:#GPIO: ") + String(ROT_S2)+ String("\"") +
               String(",\"tab_line3_rotary\":\"SW-Pin:#GPIO: ") + String(ROT_SW)+ String("\"");
#endif
#ifdef USE_AUDIO_RADIO
#ifdef USE_ROTARY
  // app 1 und lev 0 
  this_app = rotary.app_add(0,100,0);
  // app 1 und lev 1
  rotary.lev_add(this_app,0,MAXSTATIONS-1,0);
  // app1 und lev2
  rotary.lev_add(this_app);
  Serial.println(String("Init Radio Rotary app:")+String(this_app));
#endif
  audio_radio_load_stations();
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
#ifdef USE_AUDIODISPLAY_GC9A01A
  html_info += String(",\"tab_head_display\":\"Display: GC9A01A\"");
#endif
#ifdef USE_AUDIODISPLAY_ST7789
  html_info += String(",\"tab_head_display\":\"Display: ST7789\"");
#endif
  html_info += String(",\"tab_line1_display\":\"SCK:#GPIO: ") + String(TFT_SCK)+ String("\"") +
               String(",\"tab_line2_display\":\"MOSI:#GPIO: ") + String(TFT_MOSI)+ String("\"") +
               String(",\"tab_line3_display\":\"CS:#GPIO: ") + String(TFT_CS)+ String("\"") +
               String(",\"tab_line4_display\":\"DC:#GPIO: ") + String(TFT_DC)+ String("\"");
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
}

bool AudioModul::set(const String& _cmnd, const String& _val) {
  bool retval = false;
  String myvalue = _val;
  write2log(LOG_MODULE,2,_cmnd.c_str(),_val.c_str());
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
        Serial.printf("audio_set_app: case Radio => Rotary app: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
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
    // Radio: Sender einstellen hier ueber command "station 2"
    if ( _cmnd == String("station") ) {
      uint8_t newstation = myvalue.toInt();
      if ( newstation < MAXSTATIONS) audio_radio_cur_station = newstation;
#ifdef USE_AUDIODISPLAY
      audiodisplay.show_info1(audio_radio_station[audio_radio_cur_station].name);
#endif
      retval = true;
    }
    // Radio: Sender einstellen ueber Webinterface
    if ( keyword == String("audio_radio_set_stn") ) {
      for (int i=0; i<MAXSTATIONS; i++) {
        if (String(audio_radio_station[i].name) == myvalue) audio_radio_cur_station = i;  
      }
      audio_radio_on();
      html_upd_data();
#ifdef USE_AUDIODISPLAY
      audiodisplay.show_info1(audio_radio_station[audio_radio_cur_station].name);
#endif
    }
    // Radio: Sender Name speichern ueber Webinterface
    if ( keyword == String("audio_radio_save_stn_name") ) {
      snprintf(audio_radio_station[audio_radio_cur_station].name,STATION_NAME_LENGTH,"%s",myvalue.c_str());
      audio_radio_save_stations();
//      audio_web_stat();
        }
    // Radio: Sender URL speichern ueber Webinterface
    if ( keyword == String("audio_radio_save_stn_url") ) {
      snprintf(audio_radio_station[audio_radio_cur_station].url,STATION_URL_LENGTH,"%s",myvalue.c_str());
      audio_radio_save_stations();
//      audio_web_stat();
    }
#endif
#ifdef USE_AUDIO_MEDIA
    // Set for mediaplayer
    // Radio einschalten => Wiiedergabe
    if ( keyword == String(AUDIO_MEDIA) || keyword == String("audio_media") ) {
      //audio_set_app(Media);
      audio_media_play(audio_media_cur_dir, audio_media_cur_file);
      retval = true;
    }
    if ( keyword == String("audio_media_sel_album") ) {
      audiodisplay.screen(Disp_Media);
      rotary.app_set(2,1);
      rotary.max_set(100); //Durch die maximale Anzahl der Alben ersetzen
      show_album(value.toInt());
//      audio_set_app(Media);
      retval = true;
    }
    if ( keyword == String("audio_media_sd_init") ) {
      rotary.app_set(2,0);
      audio_media_sd_init_file = -1;
      audio_media_sd_init_do = true;
    }
    audiodisplay.screen(Disp_Media);


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
    html_json += String(",\"audiomsg1\":\"") + audiomsg1 + String("\"") +
                 String(",\"audiomsg2\":\"") + audiomsg2 + String("\"") +
//                 String(",\"audiomsg3\":\"") + audiomsg3 + String("\"") +
                 String(",\"audiomsg4\":\"") + audiomsg4 + String("\"");

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
  Serial.println("audio_set_mode:");
  Serial.println(String("New Mode: ") + print_mode(new_mode)); 
  Serial.println(String("Mode: ") + print_mode(mode)); 
  Serial.println(String("Last Mode: ") + print_mode(last_mode)); 
  Serial.println(String("Default Mode: ") + print_mode(default_mode)); 
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
      Serial.println(audio_vol);
      Serial.printf("audio_set_app: case Radio => Rotary app: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
        if (! switch_value ) do_switch(true);
        audio_radio_on();
      // Radio: Wiedergabe im Display
#ifdef USE_AUDIODISPLAY
      audiodisplay.screen(Screen_Radio);
      audiodisplay.vol(audio_vol);
      audiodisplay.show_info1(audio_radio_station[audio_radio_cur_station].name);
#endif
      // Weboberfläche einstellen
        html_upd_data();
      break; //Radio
#endif  //USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
    case Media:
      audiodisplay.screen(Disp_Media);
      rotary.app_set(2,0);
      rotary.max_set(100);
    break;
    case MusicUpdate:
      audiodisplay.screen(Disp_MusicUpdate);
      Serial.println("audio_set_app: MusicUpdate");
      audio_media_init_sd();
      audiodisplay.screen(Disp_Media);
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
      audiomsg1 = "";
      audiomsg2 = "";
      audiomsg3 = "";
      audiomsg4 = "";
      audiomsg5 = "";

#ifdef USE_AUDIODISPLAY
      audiodisplay.screen(Screen_Off);              
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
  audiodisplay.loop(now);
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
  // Hier wird der Klickstream definiert
/*  if ( timeout_set ) {
    if ((now - timeout_start) > KLICK_TIMEOUT) {
        //TODO: Anzeige zurücksetzen
        rotary.lev_set(0);
        timeout_set = false;
        switch (rotary.app()) {
          case Radio:
            audio_radio_disp_play();
          break;
        }
      }
  }*/
  if (rotary.valChanged()) {
    Serial.printf("Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
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
              Serial.println(rotary.val());
            break;
            case 1:
          // Sender wechseln
              start_timeout();
#ifdef USE_AUDIODISPLAY
              audiodisplay.select(rotary.val()>(uint16_t)0 ? audio_radio_station[rotary.val()-1].name : " ",
                                  audio_radio_station[rotary.val()].name,
                                  rotary.val()<MAXSTATIONS-1 ? audio_radio_station[rotary.val()+1].name : " ");
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
        Serial.printf("---Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
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
                  Serial.println("Media select");
                  Serial.printf("-1a-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
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
              Serial.printf("-2-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
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
                  Serial.println("Media set");
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
    Serial.printf(">>>Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
  }  //Ende rotary.valChanged
  if (rotary.buttonShortPressed) {
    rotary.buttonShortPressed = false;
    Serial.printf("Rotary short Pressed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
    start_timeout();
    switch(rotary.app()) {
        break;
        case Radio:
          switch(rotary.lev_up()) {
            // Radio: Neuen Sender auswählen
            case 1:
              rotary.val_set(audio_radio_cur_station);
#ifdef USE_AUDIODISPLAY
              audiodisplay.screen(Screen_Radio);
              audiodisplay.select(rotary.val()>(uint16_t)0 ? audio_radio_station[rotary.val()-1].name : " ",
                                      audio_radio_station[rotary.val()].name,
                                      rotary.val()<MAXSTATIONS-1 ? audio_radio_station[rotary.val()+1].name : " ");
#endif
            break;
            // Radio: Neuen Sender übernehmen und Level zurück auf 0 => Play
            case 2:
              if (audio_radio_cur_station != rot_last_val)
                  set(String("audio_radio_set_stn"),String(rot_last_val));
//              audio_radio_disp_play();
              rotary.lev_set(0);
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
            rotary.lev_set(0);
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
                  set(String(AUDIO_MEDIA),String("1"));
                break;
                case MusicUpdate:
                  set(String("audio_media_sd_init"),String("1"));
                break;
#endif
                default:
                  Serial.printf("Shortpress case: %u\n",rot_last_val);
                break;
              }
            break;
          }
        break;
      }
  }
  if (rotary.buttonLongPressed) {
      rotary.buttonLongPressed = false;
      Serial.printf("Rotary Long Pressed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
      start_timeout();
      rotary.app_set(Settings,0,0,LastApp-1,mode);
//      rotary.app_set(Settings,0);
#ifdef USE_AUDIODISPLAY
      audiodisplay.screen(Screen_Settings);
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
    audiodisplay.show_info2(str2);
#endif
    audiomsg1 = String(str2);
    tmpstr += String("\"audiomsg1\":\"Artist: ")+audiomsg1+String("\"");
  }
  if (info[0] == 'T' && info[1] == 'i' && info[2] == 't') {
#ifdef USE_AUDIODISPLAY                   
    audiodisplay.show_info2(str2);
#endif
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
#ifdef USE_AUDIODISPLAY                   
  audiodisplay.show_info2(info);
#endif
  tmpstr = String("{\"audiomsg2\":\"") + audiomsg2 + "\"}";
  write2log(LOG_MODULE,1,tmpstr.c_str());
  ws.textAll(tmpstr.c_str());
}

void audio_bitrate(const char *info) {
  String tmpstr;
  audiomsg4 = String(info).substring(0,3) + String(" KBps");
  tmpstr = String("{\"audiomsg4\":\"") + audiomsg4 + String("\"}");
  write2log(LOG_MODULE,1,tmpstr.c_str());
  ws.textAll(tmpstr.c_str());
#ifdef USE_AUDIODISPLAY
  audiodisplay.show_bps(audiomsg4.c_str());
#endif
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
  if ( strlen(audio_radio_station[audio_radio_cur_station].url) > 10 ) {
    audio.connecttohost(audio_radio_station[audio_radio_cur_station].url);
    write2log(LOG_MODULE,2,"Switch to ",audio_radio_station[audio_radio_cur_station].url);
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
}

#endif  //USE_AUDIO_RADIO

#endif  //USE_AUDIOMODUL
