#include "config.h"
#ifdef USE_AUDIOMODUL
#include "audiomodul.h"
#include "common.h"
#include "Audio.h"
#include "audiodisplay_bmps.h"

#ifndef TFT_ROT
#define TFT_ROT  1
#endif

#ifndef ROTARY_ENCODER_VCC_PIN
#define ROTARY_ENCODER_VCC_PIN     -1
#endif
#ifndef ROTARY_ENCODER_STEPS
#define ROTARY_ENCODER_STEPS       4
#endif
#ifndef ROTARY_ENCODER_R_PULLDOWN
#define ROTARY_ENCODER_R_PULLDOWN  false
#endif

#ifndef KLICK_TIMEOUT
#define KLICK_TIMEOUT       20
#endif

#ifdef USE_FTP
#include <FtpServer.h>
FtpServer        ftp;
#endif

#ifdef USE_DISPLAY_GC9A01A
#ifdef CONFIG_IDF_TARGET_ESP32
#warning "Compiling Display GC9A01A with Settings for ESP32"
#endif
#ifdef CONFIG_IDF_TARGET_ESP32S3
#warning "Compiling Display GC9A01A with Settings for ESP32-S3"
#endif
#include "audiodisplay_GC9A01A.h"
AudioDisplay  display(TFT_CS, TFT_DC, TFT_RES, TFT_ROT);
#endif

String audio_radio_stationname;
String audio_radio_streamtitle;
String audio_kbs;

#ifdef USE_ROTARY

#include "AiEsp32RotaryExtention.h"

AiEsp32RotaryExtention  rotary = AiEsp32RotaryExtention(ROT_S1, ROT_S2, ROT_SW, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS, ROTARY_ENCODER_R_PULLDOWN);

void IRAM_ATTR intrSRV() {
    rotary.readEncoder_ISR();
}
#endif

#ifdef DISPLAY
uint16_t* bmpBuffer;

void bootMessage(uint8_t txtcolor, const char* myMsg, bool newline) {
  display.bootMessage(txtcolor, myMsg, newline);
}

#ifdef USE_AUDIO_MEDIA
#include <TJpg_Decoder.h>
// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  for(uint8_t xb=0; xb<w; xb++){
    for(uint8_t yb=0; yb<h; yb++){
      bmpBuffer[x+xb+((y+yb)*160)]=bitmap[xb+(yb*h)];
    }
  }
  display.drawRGBBitmap(x+80, y+10, bitmap, w, h);
  return 1;
}
#endif
#endif // DISPLAY

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
// Applications einrichten
// 1) Radio
  //  Ebene 0 Lautstärke einstellen
  this_app = rotary.app_add(0,100,0);
  // Ebene 1 Sender auswählen
  this_lev = rotary.lev_add(this_app,0,MAXSTATIONS-1,0);
  // Ebene 2 Sender abspielen
  this_lev = rotary.lev_add(this_app,0,1,0);
// 2) Mediaplayer
  // Ebene 0 Lautstärke einstellen
  this_app = rotary.app_add(0,100,0);
  // Ebene 1 Album auswählen
  this_lev = rotary.lev_add(this_app,0,100,0);
  // Ebene 2 Musikstück auswählen
  this_lev = rotary.lev_add(this_app,0,100,0);
  // Ebene 3 Musikstück abspielen 
  this_lev = rotary.lev_add(this_app,0,1,0);
// 3) Speaker
  // Ebene 0 Lautstärke einstellen
  this_app = rotary.app_add(0,100,0);
  // Ebene 1 Keine Aktion - nur Dummy
  this_lev = rotary.lev_add(this_app,0,1,0);
// 4) Settings
  // Ebene 0 App auswählen
  this_app = rotary.app_add(0,LastApp-1,0);
  // Ebene 1 App starten
  this_lev = rotary.lev_add(this_app,0,1,0);
// 5) Update Music Library
// TODO: Prüfen ob wirklich benötigt
  this_app = rotary.app_add(0,1,0);
  this_lev = rotary.lev_add(this_app,0,1,0);
// Ende Rotary Initialisierung
  if (html_info.length() > 2 ) html_info += String(",");
  html_info += String("\"tab_head_rotary\":\"Rotary\"") +
               String(",\"tab_line1_rotary\":\"A-Pin:#GPIO: ") + String(ROT_S1)+ String("\"") +
               String(",\"tab_line2_rotary\":\"B-Pin:#GPIO: ") + String(ROT_S2)+ String("\"") +
               String(",\"tab_line3_rotary\":\"SW-Pin:#GPIO: ") + String(ROT_SW)+ String("\"") +
               String(",\"tab_line4_rotary\":\"Resistor:#") + String(ROTARY_ENCODER_R_PULLDOWN? "pulldown" : "pullup")+ String("\"");
#endif
#ifdef USE_AUDIO_RADIO
#ifdef DISPLAY
    bootMessage(0, "Radio", false);
#endif
  audio_radio_load_stations();
  write2log(LOG_MODULE,1,"Radio Stations loaded");
#ifdef DISPLAY
    bootMessage(1, "OK", true);
#endif
#endif
#ifdef USE_AUDIO_MEDIA
#ifdef DISPLAY
    bootMessage(0, "SD Card", false);
#endif
  if (SD.begin(SD_CS)) {
    sd_cardsize = SD.cardSize();
    sd_cardType = SD.cardType();
    sd_usedbytes = SD.usedBytes();
#if defined(DEBUG_SERIAL)
    Serial.print("SD Size: ");
    Serial.print(sd_cardsize/1073741824);
    Serial.println(" GB");
    Serial.print("SD Used: ");
    Serial.print(sd_usedbytes/1073741824);
    Serial.println(" GB");
#endif        
#ifdef DISPLAY
    char sdinfo[20];
    snprintf(sdinfo,19,"SD: %llu/%llu GB",sd_cardsize/1073741824, sd_usedbytes/1073741824);
    bootMessage(1, sdinfo, true);
#endif
  } else {
#ifdef DISPLAY
    bootMessage(2, "Error", true);
#endif
#if defined(DEBUG_SERIAL)
  Serial.println("Error mounting SD Card");
#endif        
  }
#endif  // USE_AUDIO_MEDIA

  if (audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT)) {
    write2log(LOG_MODULE,6,"Init I2S: BCLK:",String(I2S_BCLK).c_str()," LRC:",String(I2S_LRC).c_str()," OUT:",String(I2S_DOUT).c_str());
  } else {
    write2log(LOG_SYSTEM,6,"ERROR I2S: BCLK:",String(I2S_BCLK).c_str()," LRC:",String(I2S_LRC).c_str()," OUT:",String(I2S_DOUT).c_str());
  }
  //audio.setBufsize(30000,600000);
  audio.setVolumeSteps(100);
  audio.setVolume(audio_vol);

#ifdef USE_AUDIO_MEDIA
#ifdef USE_FTP
  ftp.begin("ftp","ftp");    //username, password for ftp.   (default 21, 50009 for PASV)
#ifdef DISPLAY
    bootMessage(1,"FTP Server started");
#endif
#endif
#endif // USE_AUDIO_MEDIA

//  audio.setAudioTaskCore(1);
  if (html_info.length() > 2) html_info += String(",");
  html_info += String("\"tab_head_audio\":\"I2S: MA98357\"") +
               String(",\"tab_line1_audio\":\"DOUT:#GPIO: ") + String(I2S_DOUT)+ String("\"") +
               String(",\"tab_line2_audio\":\"BCLK:#GPIO: ") + String(I2S_BCLK)+ String("\"") +
               String(",\"tab_line3_audio\":\"LRC:#GPIO: ") + String(I2S_LRC)+ String("\"");
#ifdef USE_AUDIO_MEDIA
  html_info += String(",\"tab_head_sdcard\":\"SD Card\"") +
               String(",\"tab_line1_sdcard\":\"MOSI:#GPIO: ") + String(SD_MOSI)+ String("\"") +
               String(",\"tab_line2_sdcard\":\"MISO:#GPIO: ") + String(SD_MISO)+ String("\"") +
               String(",\"tab_line3_sdcard\":\"SCK:#GPIO: ") + String(SD_SCK)+ String("\"") +
               String(",\"tab_line4_sdcard\":\"CS:#GPIO: ") + String(SD_CS)+ String("\"") +
               String(",\"tab_line5_sdcard\":\"Size/Used:# ") + String(sd_cardsize/1073741824)+ String(" GB / ") + String(sd_usedbytes/1073741824) + String(" GB \"");
#endif
#ifdef DISPLAY                   
  display.cp437(true);
  html_info += display.html_info;
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
    } else { html_update(); }
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
      html_update();
#ifdef USE_ROTARY
      // Rotarymodul einstellen - nur wenn Änderungen nicht von dort kommen!
      if (! change_from_rotary) {
        rotary.val_set(audio_vol);
#ifdef DEBUG_SERIAL
        Serial.printf("audio_set_app: case Radio => Rotary app: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
      }
#endif
#ifdef DISPLAY
      // Lautstärke auf Display anzeigen
      display.vol(audio_vol);
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
    // In den Settings für das Radio werden die Sender eingestellt.
    if ( (_cmnd == String("audio_get_settings")) &&  ( mode == Radio ) ) {
      audio_radio_send_stn2web();
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
      //audio_radio_send_stn2web();
      audio_radio_on();
#ifdef DISPLAY
      display.radio_station(audio_radio_station[audio_radio_cur_station].name);
#endif
      html_update();
    }
    // Radio: Sender Name speichern ueber Webinterface
    if ( _cmnd == String("audio_radio_save_stn_name") ) {
      snprintf(audio_radio_station[audio_radio_cur_station].name,STATION_NAME_LENGTH,"%s",_val.c_str());
      audio_radio_save_stations();
      retval = true;
        }
    // Radio: Sender URL speichern ueber Webinterface
    if ( _cmnd == String("audio_radio_save_stn_url") ) {
      snprintf(audio_radio_station[audio_radio_cur_station].url,STATION_URL_LENGTH,"%s",_val.c_str());
      audio_radio_save_stations();
      retval = true;
    }
#endif
#ifdef USE_AUDIO_MEDIA
    // Set for mediaplayer
    // Schaltet den Mediaplayer an, falls er nicht an ist und spielt das aktuelle Lied ab.
    if ( _cmnd == String("audio_media") ) {
      if ( mode != Media ) audio_set_mode(Media);
      audio_media_play(audio_media_cur_album, audio_media_cur_song);
      html_update();
      retval = true;
    }
    // In den Settings für den Mediaplayer werden die Alben dargestellt.
    if ( (_cmnd == String("audio_get_settings")) && (mode == Media) ) {
        audio_media_get_album_for_web();
        retval = true;
    }
    // Wird ein Album geöffnet werden hier die Songs des Albums geladen.
    if ( _cmnd == "audio_media_get_songs" ) {
      audio_media_get_songs_for_web(_val.toInt());
      retval = true;
    }
    if ( _cmnd == String("audio_media_play") ) {
      audio_media_changemode = false;
      audio_media_play(audio_media_cur_album, audio_media_cur_song);
      retval = true;
    }
    if ( _cmnd == String("audio_media_sel_album") ) {
      display.screen_media();
      rotary.app_set(2,1);
      rotary.max_set(100); //Durch die maximale Anzahl der Alben ersetzen
//      show_album(value.toInt());
//      audio_set_app(Media);
      retval = true;
    }
    if ( _cmnd == String("audio_media_update") ) {
      display.screen_media_update();
      audio_media_start_update();
      rotary.app_set(2,0);
//      audio_media_sd_init_file = -1;
//      audio_media_sd_init_do = true;
    }
    if ( _cmnd == "audio_media_play_album" ) {
      audio_media_cur_album = _val.toInt();
      audio_media_cur_song = 0;
      audio_media_play(audio_media_cur_album,audio_media_cur_song);
      retval = true;
    }
    if ( _cmnd == "audio_media_play_song" ) {
      audio_media_cur_album = 0;
      audio_media_cur_song = 0;
      bool dirMode = true;
      int i =0;
      do {
        if (_val.charAt(i) == '#') {
          dirMode = false;
        } else {
          if (dirMode) {
            audio_media_cur_album = 10*audio_media_cur_album+(_val.charAt(i)-'0');
          } else {
            audio_media_cur_song = 10*audio_media_cur_song+(_val.charAt(i)-'0');
          }
        }
        i++;
      } while(i<_val.length());
      audio_media_play(audio_media_cur_album,audio_media_cur_song);
      retval = true;
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
  html_json += String(",\"audio_vol\":") + String(audio_vol);
  #ifdef USE_AUDIO_RADIO
  html_json += String(",\"audio_radio_show\":1,\"audio_radio\":");
  if (mode == Radio) {
    html_json += String("1");
//    for (int i=0; i<MAXSTATIONS; i++) {
//      html_json += String(",\"audio_radio_add_stn_") + String(i) + String("\":\"") + 
//                   String(audio_radio_station[i].url) +
//                   String(";") + String(audio_radio_station[i].name) + String("\"");
//    }
    html_json += String(",\"audiomsg1\":\"") + audio_radio_stationname + String("\"") +
                 String(",\"audiomsg2\":\"") + audio_radio_streamtitle + String("\"") +
                 String(",\"audiomsg4\":\"") + audio_kbs + String("\"");
  } else { html_json += String("0"); }
  #else
  html_json += String(",\"audio_radio_show\":0");
  #endif
  #ifdef USE_AUDIO_MEDIA
  html_json += String(",\"audio_media_show\":1,\"audio_media\":");
  if (mode == Media) {
    html_json += String("1");
    html_json += String(",\"audiomsg1\":\"") + audio_media_album_name + String("\"") +
                 String(",\"audiomsg2\":\"") + audio_media_song_name + String("\"");
  } else { html_json += String("0"); }
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

void AudioModul::html_update() {
  // Ein- Ausschalter der Anlage
  html_json = String("\"") + html_place + String("\":") + String(switch_value?"1":"0");
  html_json += String(",\"audio_vol\":") + String(audio_vol);
  switch (mode) {
#ifdef USE_AUDIO_RADIO
    case Radio:
      if ( mode_changed ) html_json += String(",\"audio_radio\":1");
    break;
#endif
#ifdef USE_AUDIO_MEDIA
    case Media:
      if ( mode_changed ) html_json += String(",\"audio_media\":1");
    break;
#endif
    default:
      html_json += String(",\"audio_radio\":0,\"audio_media\":0,\"audio_speak\":0");
  }
  Base_Generic::html_update();
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
#ifdef DISPLAY
      display.screen_radio();
      display.vol(audio_vol);
      display.radio_station(audio_radio_station[audio_radio_cur_station].name);
#endif
      // Weboberfläche einstellen
      html_update();
      break; //Radio
#endif  //USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
    case Media:
      if (! switch_value ) do_switch(true);
      audio_media_on();
#ifdef DISPLAY
      display.screen_media();
#endif
#ifdef USE_ROTARY
      rotary.app_set(2,0);
      rotary.max_set(100);
#endif
      // Weboberfläche einstellen
      html_update();
    break; //Media
    case MusicUpdate:
      display.screen_media_update();
      Serial.println("audio_set_app: MusicUpdate");
//      audio_media_init_sd();
//      display.screen_media();
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

#ifdef DISPLAY
      display.screen_off();              
#endif
      html_update();
    break; // Off
    } //switch
  }
}

void AudioModul::audio_off() {
  switch(mode) {
#ifdef USE_AUDIO_RADIO    
    case Radio:
      audio_radio_off();
    break;
#endif //USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
    case Media:
      audio_media_off();
    break;
#endif //USE_AUDIO_MEDIA
  }
}

void AudioModul::start_timeout() {
  timeout_set = true;
  timeout_start = now;
}

void AudioModul::loop(time_t now) {
// loop Funktionen aller eingebundenen Objekte aufruen
#ifdef DISPLAY  
  display.loop(now);
#endif
#ifdef USE_FTP
  ftp.handleFTP();
#endif
// Das Audio loop wird nur aufgerufen wenn Audio auch aktiv ist
  if (mode != Off) {
    audio.loop();
    if (!audio.isRunning()) {
#ifdef USE_AUDIO_RADIO
      if (mode == Radio) {
        audio_radio_off();
        audio_radio_on();
      }
#endif
#ifdef USE_AUDIO_MEDIA
      if (mode == Media) {
        if ( song_started > 0) {
          if (now - song_started > 2) {
            Serial.println("###> Song finished, next song");
            audio_media_cur_song++;
            if (! getSongByNumber(SD, audio_media_cur_album, audio_media_cur_song)) {
              audio_media_cur_album++;
              audio_media_cur_song = 0;
              if (! getSongByNumber(SD, audio_media_cur_album, audio_media_cur_song)) {
                audio_media_cur_album = 0;
                audio_media_cur_song = 0;
              }
            } 
            audio_media_play(audio_media_cur_album, audio_media_cur_song);
          }
        } else song_started = now;
      }
#endif
    } // if (!audio.isRunning())
  }  
#ifdef USE_ROTARY
  rotary.loop(now);
  // Hier wird der Klickstream für die Bedienung mittels Drehregler definiert
  // Für alle Funktionen, die mit einen Klick auf den Drehregler ausgeführt werden,
  // wird die Variable timeout_set auf true gesetzt. Findet innerhalb der Zeit KLICK_TIMEOUT
  // kein weiterer Klick statt, wird die AKtion zurückgesetzt und die Anzeige auf den
  // zuletzt aktiven Stand zurückgesetzt.
  if ( timeout_set ) {
    if ((now - timeout_start) > KLICK_TIMEOUT) {
        //TODO: Anzeige zurücksetzen
        rotary.app_set(rotary.app(),0);
        timeout_set = false;
        switch (rotary.app()) {
          case Radio:
            display.screen_radio();
          break;
          case Media:
            display.screen_media();
          break;
        }
    }
  }
  // Der Drehregler wurde gedreht.
  // Die darauf folgende Aktion ist abhängig von der App und der Ebene.
  if (rotary.valChanged()) {
#ifdef DEBUG_SERIAL
    Serial.printf("Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
    switch(rotary.app()) {
      case Off: {
          if (rotary.lev() == 0) {
            if (rotary.val() > 1) {
              // Die letzte App wird aufgerufen
              if (last_mode == Radio || last_mode == Off) set(String("audio_radio"),String("1"));
              if (last_mode == Media) set(String("audio_media"),String("1"));
            }              
          }
        } 
      break;
#ifdef USE_AUDIO_RADIO
      case Radio: {
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
#ifdef DISPLAY
              display.radio_select_station(
                          rotary.val() > 0 ? audio_radio_station[rotary.val()-1].name : "",
                          audio_radio_station[rotary.val()].name,
                          rotary.val() < MAXSTATIONS ? audio_radio_station[rotary.val()+1].name : "" );
#endif
              rot_last_val = rotary.val();
            break;
          }
      }
      break;  // case Radio
#endif // USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
      case Media: {
        switch(rotary.lev()) {
          case 0:
        // Lautstärke einstellen
            change_from_rotary = true;
            set("audio_vol",String(rotary.val()));
            change_from_rotary = false;
          break;
          case 1: {
        // Album auswählen
            start_timeout();
            audio_media_sel_album = rotary.val();
            audio_media_sel_song = 0;
            getSongByNumber(SD, audio_media_sel_album, 0);
#ifdef DISPLAY
            display.media_select_album(audio_media_album_name, cd_bmp);
#endif
          }
          break;
          case 2: {
          // Musikstück auswählen
            start_timeout();
            audio_media_sel_song = rotary.val();
            getSongByNumber(SD, audio_media_sel_album, audio_media_sel_song);
#ifdef DISPLAY
            display.media_select_song(audio_media_album_name, audio_media_song_name, cd_bmp);

#endif
          }
          break;
          case 3:
          // Album wechseln
//            audio_media_cur_song = rot_last_val;
//            start_timeout();
//            set(String("audio_media"),String("1"));
//            rot_last_val = rotary.val();
          break;
        }
      }
      break;  // case Media
#endif // USE_AUDIO_MEDIA
      case Settings: {
#ifdef DEBUG_SERIAL
        Serial.printf("---Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
        start_timeout();
          switch(rotary.lev()) {
            case 0:
            // App oder Eigenschaft auswählen
 #ifdef DISPLAY
              switch(rotary.val()) {
                case Off:
 //                 display.show_set_icon("Off");
                  display.drawRGBBitmap(80,80,off_bmp,OFF_BMP_HEIGHT,OFF_BMP_WIDTH);
                break;
                case Radio:
                  display.drawRGBBitmap(80,80,radio_bmp,RADIO_BMP_HEIGHT,RADIO_BMP_WIDTH);
                break;
                case Media:
                  display.drawRGBBitmap(80,80,media_bmp,MEDIA_BMP_HEIGHT,MEDIA_BMP_WIDTH);
#ifdef DEBUG_SERIAL
                  Serial.println("Media select");
                  Serial.printf("-1a-Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
                  break;
                case Speaker:
                  display.drawRGBBitmap(80,80,speaker_bmp,SPEAKER_BMP_HEIGHT,SPEAKER_BMP_WIDTH);
                break;
                case Settings:
                  display.drawRGBBitmap(80,80,settings_bmp,SETTINGS_BMP_HEIGHT,SETTINGS_BMP_WIDTH);
                break;
                case MusicUpdate:
                  display.drawRGBBitmap(80,80,music_update_bmp,MUSIC_UPDATE_BMP_HEIGHT,MUSIC_UPDATE_BMP_WIDTH);
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
//                  display.show_set_icon("Speaker");
                break;
                case 4:
//                  display.show_set_icon("Settings");
                break;
                case 5:
//                  display.show_set_icon("MusicUpdate");
                break;
              }
  //            rot_last_val = rotary.val();
            break;
          }
        }
      break; // case Settings
      }
#ifdef DEBUG_SERIAL
    Serial.printf(">>>Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
  }  //Ende rotary.valChanged
//  Der Button wurde kurz gedrückt
//  Hier wird eine Aktion innerhalb der App ausgeführt. 
//  Die Art der Aktion hängt von der App und der Ebene ab.
  if (rotary.buttonShortPressed) {
    uint8_t new_lev;
    rotary.buttonShortPressed = false;
#ifdef DEBUG_SERIAL
    Serial.printf("Rotary short Pressed App: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.get_max_lev(rotary.app()), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
    start_timeout();
    switch(rotary.app()) {
// Aktionen nach kurzen   Druck beim Radio
#ifdef USE_AUDIO_RADIO
      case Radio: {
        new_lev = rotary.lev_up();
#ifdef DEBUG_SERIAL
        Serial.printf("Rotary nev Lev %u\n", new_lev);
#endif
        switch(new_lev) {
          // Radio Ebene 1: Neuen Sender auswählen
          case 1:
            rotary.val_set(audio_radio_cur_station);
#ifdef DISPLAY
            display.screen_radio_select();
            display.radio_select_station(
                          audio_radio_cur_station > 0 ? audio_radio_station[audio_radio_cur_station-1].name : "",
                            audio_radio_station[audio_radio_cur_station].name,
                          audio_radio_cur_station < MAXSTATIONS ? audio_radio_station[audio_radio_cur_station+1].name : "" );
#endif
          break;
          // Radio Ebene 2: Neuen Sender übernehmen und Level zurück auf 0 => Play
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
#ifdef DISPLAY
              audio_radio_stationname = String("");
              display.radio_streamtitle(audio_radio_stationname);
              display.screen_radio();
#endif
              rotary.app_set(rotary.app(),0);
            }
            break;
          }
        }
      break; //case Radio
#endif //USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
      case Media: {
        switch(rotary.lev_up()) {
          // Media Ebene 1: Album auswählen
          case 1: {
            audio_media_changemode = true;
            audio_media_sel_album = audio_media_cur_album;
            audio_media_sel_song = audio_media_cur_song;
            rotary.val_set(audio_media_sel_album);
#ifdef DEBUG_SERIAL
            Serial.printf("Media change Album Lev 1: App: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.get_max_lev(rotary.app()), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
#ifdef DISPLAY
            getSongByNumber(SD, audio_media_sel_album, 0);
            display.media_select_album(audio_media_album_name, cd_bmp);
#endif
          }
          break;
          // Media Ebene 2: Musikstück des Albums auswählen
          case 2: {
          //  audio_media_sel_album = rot_last_val;
            rotary.val_set(audio_media_sel_song);
            getSongByNumber(SD, audio_media_sel_album, audio_media_sel_song);
#ifdef DEBUG_SERIAL
            Serial.printf("Media change Song App: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.get_max_lev(rotary.app()), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
#ifdef DISPLAY
            display.media_select_song(audio_media_album_name, audio_media_song_name, cd_bmp);
#endif
          }
          break;
          // Media Ebene 3: Player mit dem ausgewählten Stück starten
          case 3: {
            audio_media_cur_album = audio_media_sel_album;
            audio_media_cur_song = audio_media_sel_song;
#ifdef DEBUG_SERIAL
            Serial.printf("Media change execute: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.get_max_lev(rotary.app()), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
            rotary.app_set(2,0);
#ifdef DEBUG_SERIAL
            Serial.printf("Test nach app_set: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.get_max_lev(rotary.app()), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
            set(String("audio_media_play"),String("1"));
          }
          break;
        }
      }
      break; //case Media
#endif //USE_AUDIO_MEDIA
      case 4: {  //Settings
        switch(rotary.lev_up()) {
          // Off: Ausschalten
          case 1: {
            switch(rot_last_val) {
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
                set(String("audio_media_update"),String("1"));
              break;
#endif
              case Settings:

              break;
              default:
#ifdef DEBUG_SERIAL
                Serial.printf("Shortpress case: %u\n",rot_last_val);
#endif
              break;
            } // switch(rot_last_val)
          }
          break; // case 1
        } //switch(rotary.lev_up())
      }
      break; // case Settings
    }
  }
// Button wurde lang gedrückt
// Einleitung zum Wechsel der App
// Hier wird nur das Symbol der aktuellen App angezeigt und der Rotary auf Settings gesetzt.
// Die App wird erst gewechselt, wenn der Button nach Auswahl (Drehen) erneut gedrückt wird.
  if (rotary.buttonLongPressed) {
      rotary.buttonLongPressed = false;
#ifdef DEBUG_SERIAL
      Serial.printf("Rotary Long Pressed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
#endif
      start_timeout();
      if ( ! rotary.app_set((uint8_t)Settings,0,0,(uint16_t)LastApp-1,(uint16_t)mode) ) 
             Serial.printf("Error: app_set(%u,0,0,%u,%u);",(uint8_t)Settings, (uint16_t)LastApp-1, (uint16_t)mode);
      Serial.printf("##Rotary Long Pressed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary.app(), rotary.lev(), rotary.val(), rotary.min(), rotary.max(), rot_last_val);
//      rotary.app_set(Settings,0);
#ifdef DISPLAY
      display.screen_settings();
      switch (mode) {
        case Off:
          display.drawRGBBitmap(80,80,off_bmp,OFF_BMP_HEIGHT,OFF_BMP_WIDTH);
        break;
        case Radio:
          display.drawRGBBitmap(80,80,radio_bmp,RADIO_BMP_HEIGHT,RADIO_BMP_WIDTH);
        break;
        case Media:
          display.drawRGBBitmap(80,80,media_bmp,MEDIA_BMP_HEIGHT,MEDIA_BMP_WIDTH);
        break;
        case Speaker:
          display.drawRGBBitmap(80,80,speaker_bmp,SPEAKER_BMP_HEIGHT,SPEAKER_BMP_WIDTH);
        break;
        case Settings:
          display.drawRGBBitmap(80,80,settings_bmp,SETTINGS_BMP_HEIGHT,SETTINGS_BMP_WIDTH);
        break;
        case MusicUpdate:
          display.drawRGBBitmap(80,80,music_update_bmp,MUSIC_UPDATE_BMP_HEIGHT,MUSIC_UPDATE_BMP_WIDTH);
        break;
      }
#endif
      rot_last_val = mode;
  }
#endif //USE_ROTARY
// Ende Klickstream Definition
// Update der Media Data
//#ifdef USE_AUDIO_MEDIA
/*  if (audio_media_do_update) {
    if (! audio_media_update_running) {
      audio_media_update_lowstr = (char*)malloc(SD_DIR_LENGTH);
      audio_media_update_highstr = (char*)malloc(SD_DIR_LENGTH);
      memset(audio_media_update_lowstr,0,SD_DIR_LENGTH);
      memset(audio_media_update_highstr,0,SD_DIR_LENGTH);
      deleteFile(SD,"/album.txt");
      audio_media_update_running = true;
    }
    sd_root = SD.open("/");
    if (sd_root) {
      audio_media_update_found = false;
      sd_root.rewindDirectory();
      sd_dir = sd_root.openNextFile(); */
/* Handlungsanweisung in der aufrufenden Prozedur
 * Start: s0 und s1 ist leer, s2 der Name des ersten Dirs
 *        Wenn Rückgabewert "true" dann s1 = s2 tauschen.
 * Ende des durchlaufs (/ Dir komplett ausgelesen): s0 = s1; s1 = leer
 * Nächste Durchlauf */
 /*     while (sd_dir) {
        if (sd_dir.isDirectory()) {
          if ( audio_media_sort(audio_media_update_lowstr, audio_media_update_highstr, sd_dir.name()) ) {
            audio_media_update_found = true;
            strcpy(audio_media_update_highstr, sd_dir.name());
          }
        }
        sd_dir = sd_root.openNextFile();
      }
      sd_dir.close();
      Serial.println(audio_media_update_lowstr);
      strcpy(audio_media_update_lowstr, audio_media_update_highstr);
      memset(audio_media_update_highstr,0,SD_DIR_LENGTH);
    }
    sd_root.close();
    appendFile(SD, "/album.txt", audio_media_update_lowstr);
    if ( ! audio_media_update_found ) {
      Serial.println("Media Update fertig!");
//      readFile(SD,"/album.txt");
      free(audio_media_update_lowstr);
      free(audio_media_update_highstr);
      audio_media_do_update = false;
      audio_media_update_running = false;
      
    }
  }*/
//#endif //USE_AUDIO_MEDIA
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
#ifdef DISPLAY                   
    display.set_bps(str2);
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
#ifdef DISPLAY
    String myalbum = String(str2);
    display.media_album(myalbum);
#endif
    tmpstr += String("\"audiomsg1\":\"Artist: ")+String(str2)+String("\"");
  }
  if (info[0] == 'T' && info[1] == 'i' && info[2] == 't') {
#ifdef DISPLAY          
    String mysong = String(str2);         
    display.media_song(mysong);
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
#ifdef DISPLAY                   
  tmpstr = String(info);
  display.radio_streamtitle(tmpstr);
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
#ifdef DISPLAY
  tmpstr = String(info).substring(0,3) + String(" KBps");
  display.radio_bps(tmpstr.c_str());
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
#ifdef DISPLAY
    display.screen_radio();
    display.radio_station(audio_radio_station[audio_radio_cur_station].name);
#endif
  }
}

void AudioModul::audio_radio_send_stn2web() {
  for (int i=0; i<MAXSTATIONS; i++) {
    String html_json = String("{\"audio_radio_add_stn") + String(i) + String("\":\"") +
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
  //audio_radio_send_stn2web();
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
  getSongByNumber(SD, audio_media_cur_album, audio_media_cur_song);
  audio_media_play(audio_media_cur_album,audio_media_cur_song);
  display.screen_media();
  display.setCursor(20,120);
  display.print(audio_media_album_name);
  display.setCursor(30,150);
  display.print(audio_media_song_name);
  ws.textAll("{\"audio_media\":1}");
//  write2log(LOG_MODULE,2,"Anzahl Songs: ",String(allSongs).c_str());
}

void AudioModul::audio_media_off() {
  ws.textAll("{\"audio_media\":0}");
}

void AudioModul::audio_media_get_album_for_web() {
  String tmpstr = String("{");
  uint16_t dirNo = 0;
  bool setKomma = false;
  File root = SD.open("/");
  if (!root) {
    return; // Fehler beim Öffnen des Root-Verzeichnisses
  }
  root.rewindDirectory();
  File dir = root.openNextFile();
  while (dir) {
    if (dir.isDirectory()) {
      if ( setKomma ) tmpstr += String(",");
      tmpstr += String("\"audio_media_add_album_d") + String(dirNo) + String("f0") + String("\":\"A#")+String(dirNo) + 
                String("#0#") + String(dir.name()) + String("\"");
      if (tmpstr.length() > 500) {
        tmpstr += String("}");
        ws.textAll(tmpstr);
        write2log(LOG_MODULE,1,tmpstr.c_str());
        tmpstr = String("{");
        setKomma = false;
      } else {
        setKomma = true;
      }
      dirNo++;
    }
    dir = root.openNextFile();
  }
  tmpstr += String("}");
  ws.textAll(tmpstr);
  write2log(LOG_MODULE,1,tmpstr.c_str());
  if ( dir ) dir.close();
  root.close();
}

void AudioModul::audio_media_get_songs_for_web(uint16_t reqDirNo) {
  String tmpstr = String("{");
  uint16_t dirNo = 0;
  uint16_t fileNo = 0;
  File root = SD.open("/");
  if (!root) return;
  root.rewindDirectory();
  File dir = root.openNextFile();
  while (dir) {
    if (dir.isDirectory()) {
      if ( dirNo == reqDirNo ) {
        // Jetzt die Dateien in dem Verzeichnis durchgehen
        dir.rewindDirectory();
        File file = dir.openNextFile();
        while (file) {
          if ( String(file.name()).endsWith(".mp3") ) {
            if (fileNo > 0) tmpstr += String(",");
            // Datei gefunden, jetzt den Eintrag für die Weboberfläche erstellen
            // Der Eintrag hat das Format: "audio_media_add_album_d0f0":"A#0#0#filename.mp3"
            // Dabei steht A für Album, 0 für die Albumnummer, 0 für die Dateinummer und filename.mp3 für den Dateinamen
            tmpstr += String("\"audio_media_add_album_d") + String(dirNo) + String("f") + String(fileNo) + String("\":\"T#") + 
                      String(dirNo) + String("#") + String(fileNo) + String("#") + String(file.name()) + String("\"");
            fileNo++;
          }
          file = dir.openNextFile();
        }
        if (file) file.close();
      }
      dirNo++;
    }
    dir = root.openNextFile();
  }
  root.close();
  tmpstr += "}";
  Serial.printf("audio_media_get_songs_for_web: %s\n", tmpstr.c_str());
  ws.textAll(tmpstr);
}

void AudioModul::audio_media_play(uint16_t _albumNo, uint16_t _songNo) {
  if (getSongByNumber(SD, _albumNo, _songNo)) {
    write2log(LOG_MODULE,2,"Play song: ",audio_media_song_name.c_str());
    String songPath = String("/") + audio_media_album_name + String("/") + audio_media_song_name;
    write2log(LOG_MODULE,2,"Song path: ",songPath.c_str());
#ifdef DISPLAY
    display.media_album(audio_media_album_name);
    display.media_song(audio_media_song_name);
    display.screen_media();
#endif
//    audio.stopSong();
    Serial.printf("###> Play song: %s\n", songPath.c_str());
    audio.connecttoFS(SD,songPath.c_str());
    audio.setVolume(audio_vol);
    song_started = 0;
  }
}

bool AudioModul::getSongByNumber(fs::FS &fs, uint16_t albumNo, uint16_t songNo) {
  bool retval = false;
  bool dir_found = false;
  bool file_found = false;
  uint16_t dirNo = 0;
  uint16_t songFileNo = 0;
  String tmpstr;
  Serial.printf("GetSongByNumber suche Album #%u Song #%u\n", albumNo, songNo);
  File root = fs.open("/");
  if (!root) {
    Serial.println("Failed to open directory");
    return false;
  }
  root.rewindDirectory();
  File dir = root.openNextFile();
  while (dir) {
    Serial.printf("GetSongByNumber filename #%u in root: %s\n", dirNo, dir.name());
    if (dir.isDirectory()) {
      if ( dirNo == albumNo ) {
        audio_media_album_name = String(dir.name());
        Serial.printf("Found directory: %s\n", dir.name());
        // Jetzt die Dateien in dem Verzeichnis durchgehen
        dir.rewindDirectory();
        songFileNo = 0;
        Serial.printf("Searching for song #%u in album #%u\n", songNo, albumNo);
        // Alle Dateien im Verzeichnis durchgehen
        File file = dir.openNextFile();
        while (file) {
          tmpstr = String(file.name());
          if ( tmpstr.endsWith(".mp3") ) {
            Serial.printf("GetSongByNumber filename #%u in album: %s\n", songFileNo, file.name());
            if ( songFileNo == songNo ) {
              Serial.printf("Found song: %s\n", file.name());
              audio_media_song_name = String(file.name());
              file_found = true;
            }
            songFileNo++;
          }
          file = dir.openNextFile();
        }
        dir_found = true;
        audio_media_album_name = String(dir.name());
        Serial.printf("Found directory: %s\n", dir.name());
        break;
      }
      dirNo++;
    }
    dir = root.openNextFile();
  }
  root.close();
  if (! (dir_found && file_found) ) {
    Serial.printf("Album #%u and Song #%u not found\n", albumNo, songNo);
    return false;
  }
  return true;
}

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
 * 0) Folgende Dateien werden nicht beruecksichtigt: ".*" (Dateien deren Name mit einem Punkt anfaengt)
 * 1) Ist strlen(s0) == 0 und strlen(s1) == 0 dann ist der Rückgabewert true; 
 * 2) Ist strlen(s0) == 0 und strlen(s1) > 0 und s1 > s2 dann ist der Rückgabewert true;
 * 3) Ist strlen(s0) == 0 und strlen(s1) > 0 und s1 <= s2 dann ist der Rückgabewert false;
 * 4) Ist strlen(s0) > 0 und strlen(s1) == 0 und s0 < s2 dann ist der Rückgabewert true;
 * 5) Ist strlen(s0) > 0 und strlen(s1) == 0 und s0 >= s2 dann ist der Rückgabewert false;
 * 6) Ist strlen(s0) > 0 und s0 < s2 und s1 > s2 dann ist der Rückgabewert true;
 * 7) Ist strlen(s0) > 0 und s0 < s2 und s1 <= s2 dann ist der Rückgabewert false;
 * 8) Ist strlen(s0) > 0 und s0 >= s2 dann ist der Rückgabewert false;
 * 
 * Handlungsanweisung in der aufrufenden Prozedur
 * Start: s0 und s1 ist leer, s2 der Name des ersten Dirs
 *        Wenn Rückgabewert "true" dann s2 und s1 tauschen.
 * Ende des durchlaufs (/ Dir komplett ausgelesen): s0 = s1; s1 = leer
 * Nächste Durchlauf
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

void AudioModul::appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.println(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void AudioModul::deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

#endif //USE_AUDIO_MEDIA

#endif  //USE_AUDIOMODUL
