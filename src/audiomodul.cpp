#ifdef ESP32
#include "audiomodul.h"
#include "common.h"
#include "audiodisplay_bmps.h"

#ifndef KLICK_TIMEOUT
#define KLICK_TIMEOUT               20
#endif

AudioModul* audiomodul_ptr = NULL;

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
//AudioDisplay  display(TFT_CS, TFT_DC, TFT_RES, TFT_ROT);
#endif

#ifdef USE_ROTARY

#include "AiEsp32RotaryExtention.h"

//AiEsp32RotaryExtention  rotary = AiEsp32RotaryExtention(ROT_S1, ROT_S2, ROT_SW, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS, ROTARY_ENCODER_R_PULLDOWN);

void IRAM_ATTR intrSRV() {
    audiomodul_ptr->rotary->readEncoder_ISR();
}
#endif // USE_ROTARY

#ifdef USE_DISPLAY

void bootMessage(uint8_t txtcolor, const char* myMsg, bool newline) {
  audiomodul_ptr->display->bootMessage(txtcolor, myMsg, newline);
}
#endif // USE_DISPLAY

/************************************************************************************
// Die folgende Callbackfunktion ergänzen die Lib: ESP32-audioI2S
************************************************************************************/
void my_audio_info(Audio::msg_t m) {
  switch(m.e){
//        case Audio::evt_info:           Serial.printf("info: ....... %s\n", m.msg); break;
    case Audio::evt_eof: 
//      Serial.printf("end of file:  %s\n", m.msg); 
      audiomodul_ptr->song_eof = true;
    break;
    case Audio::evt_bitrate:
      audiomodul_ptr->bps = String(m.msg).toInt()/1000 + String(" KBps");
      audiomodul_ptr->display_update_set = true;
      audiomodul_ptr->html_update_set = true;
    break; // icy-bitrate or bitrate from metadata
    case Audio::evt_icyurl:
      write2log(LOG_WEB,2,"icy URL:",m.msg);
    break;
    case Audio::evt_id3data: {
      write2log(LOG_WEB,2,"ID3 data:",m.msg);
      String infostr = String(m.msg);
      if (infostr.indexOf("Artist") != -1) {
        audiomodul_ptr->media_artist_name = infostr.substring(infostr.indexOf("Artist: ")+8);
        audiomodul_ptr->display_update_set = true;
        audiomodul_ptr->html_update_set = true;
      }
      if (infostr.indexOf("Title") != -1) {
        audiomodul_ptr->media_song_name = infostr.substring(infostr.indexOf("Title: ")+7);
        audiomodul_ptr->display_update_set = true;
        audiomodul_ptr->html_update_set = true;
      }
      if (infostr.indexOf("Album") != -1) {
        audiomodul_ptr->media_album_name = infostr.substring(infostr.indexOf("Album: ")+7);
        audiomodul_ptr->display_update_set = true;
        audiomodul_ptr->html_update_set = true;
      }
    }
    break; // id3-data or metadata
//        case Audio::evt_lasthost:       Serial.printf("last URL: ... %s\n", m.msg); break;
    case Audio::evt_name:
      audiomodul_ptr->radio_stationname = String(m.msg);
      audiomodul_ptr->display_update_set = true;
      audiomodul_ptr->html_update_set = true;
    break; // station name or icy-name
    case Audio::evt_streamtitle:
      audiomodul_ptr->radio_streamtitle = String(m.msg);
      audiomodul_ptr->display_update_set = true;
      audiomodul_ptr->html_update_set = true;
    break;
    case Audio::evt_icylogo:
      write2log(LOG_WEB,2,"icy logo:",m.msg);
    break;
//        case Audio::  evt_icydescription: Serial.printf("icy descr: .. %s\n", m.msg); break;
//        case Audio::evt_image: for(int i = 0; i < m.vec.size(); i += 2){
//                                        Serial.printf("cover image:  segment %02i, pos %07lu, len %05lu\n", i / 2, m.vec[i], m.vec[i + 1]);} break; // APIC
//        case Audio::evt_lyrics:         Serial.printf("sync lyrics:  %s\n", m.msg); break;
//        case Audio::evt_log   :         Serial.printf("audio_logs:   %s\n", m.msg); break;
//    default:                        Serial.printf("message:..... %s\n", m.msg); break;
  }
}



/*
#ifdef USE_AUDIO_MEDIA
#define JPG_SCALE              4
#define BMPSIZE                32000      //40*40*2
#define BMPSIZE                  12800      //80*80*2
uint16_t* bmpBuffer;
#include <TJpg_Decoder.h>
// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  for(uint8_t xb=0; xb<w; xb++) {
    for(uint8_t yb=0; yb<h; yb++) {
      bmpBuffer[x+xb+((y+yb)*160)] = bitmap[xb+(yb*h)];
//      Serial.println(x+xb+((y+yb)*160));
    }
  }
  Serial.printf("tft_output: x:%d y:%d w:%d h:%d\n", x, y, w, h);
  display.drawRGBBitmap(x+80, y+10, bitmap, w, h);
  return 1;
}
#endif
*/

void AudioModul::begin(const char* html_place, const char* label, const char* keyword, bool _show_diagramm)  {
// Startet als Schalter ohne HW-Pin ohne Diagramm => Fall 1  
  Switch_OnOff::begin(html_place, label, keyword, false, true, true, _show_diagramm);
  this->html_init_set = true;
  this->html_info_set = true;
  audio = new Audio();
  audiomodul_ptr = this;
#ifdef USE_DISPLAY_GC9A01A
  display = new AudioDisplay(TFT_CS, TFT_DC, TFT_RES, TFT_ROT);
  display->begin();
  display->bootMessage(0, "Radio", false);
#endif
#ifdef USE_ROTARY
  rotary = new AiEsp32RotaryExtention(ROT_S1, ROT_S2, ROT_SW, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS, ROTARY_ENCODER_R_PULLDOWN);
#endif
#ifdef DEBUG_SERIAL
  Serial.println("Auiomodul begin");
#endif
#ifdef USE_ROTARY
  rotary->areEncoderPinsPulldownforEsp32=false;
  rotary->begin();
  rotary->setup(intrSRV);
  rotary->setBoundaries(0, 100, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotary->setAcceleration(250);
// Applications einrichten
// 1) Radio
  //  Ebene 0 Lautstärke einstellen
  this->this_app = rotary->app_add(0,100,0);
  // Ebene 1 Sender auswählen
  this->this_lev = rotary->lev_add(this->this_app,0,MAXSTATIONS-1,0);
  // Ebene 2 Sender abspielen
  this->this_lev = rotary->lev_add(this->this_app,0,1,0);
// 2) Mediaplayer
  // Ebene 0 Lautstärke einstellen
  this->this_app = rotary->app_add(0,100,0);
  // Ebene 1 Album auswählen
  this->this_lev = rotary->lev_add(this->this_app,0,100,0);
  // Ebene 2 Musikstück auswählen
  this->this_lev = rotary->lev_add(this->this_app,0,100,0);
  // Ebene 3 Musikstück abspielen 
  this->this_lev = rotary->lev_add(this->this_app,0,1,0);
// 3) Speaker
  // Ebene 0 Lautstärke einstellen
  this->this_app = rotary->app_add(0,100,0);
  // Ebene 1 Keine Aktion - nur Dummy
  this->this_lev = rotary->lev_add(this->this_app,0,1,0);
// 4) Settings
  // Ebene 0 App auswählen
  this->this_app = rotary->app_add(0,last_app-1,0);
  // Ebene 1 App starten
  this->this_lev = rotary->lev_add(this->this_app,0,1,0);
// 5) Update Music Library
// TODO: Prüfen ob wirklich benötigt
  this->this_app = rotary->app_add(0,1,0);
  this->this_lev = rotary->lev_add(this->this_app,0,1,0);
// Ende Rotary Initialisierung
  rotary->app_set(0,0); // Application 0, Level 0 aktivieren 
#endif
  Audio::audio_info_callback = my_audio_info; // optional
#ifdef USE_AUDIO_RADIO
  app_no_max++;
  app_no_radio = app_no_max;
  if (this->app_no_last == 0) this->app_no_last = app_no_radio;
  radio_load_stations();
  write2log(LOG_MODULE,1,"Radio Stations loaded");
#ifdef USE_DISPLAY
  display->bootMessage(1, "OK", true);
#endif
#endif
#ifdef USE_AUDIO_MEDIA
  app_no_max++;
  app_no_media = app_no_max;
  if (this->last_mode == 0) this->last_mode = app_no_media;
#ifdef USE_DISPLAY
  display->bootMessage(0, "SD Card", false);
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
#ifdef USE_DISPLAY
    char sdinfo[20];
    snprintf(sdinfo,19,"SD: %llu/%llu GB",sd_cardsize/1073741824, sd_usedbytes/1073741824);
    display->bootMessage(1, sdinfo, true);
#endif
  } else {
#ifdef USE_DISPLAY
    display->bootMessage(2, "Error", true);
#endif
#if defined(DEBUG_SERIAL)
    Serial.println("Error mounting SD Card");
#endif        
  }
#endif  // USE_AUDIO_MEDIA

  if (audio->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT)) {
    write2log(LOG_MODULE,6,"Init I2S: BCLK:",String(I2S_BCLK).c_str()," LRC:",String(I2S_LRC).c_str()," OUT:",String(I2S_DOUT).c_str());
  } else {
    write2log(LOG_SYSTEM,6,"ERROR I2S: BCLK:",String(I2S_BCLK).c_str()," LRC:",String(I2S_LRC).c_str()," OUT:",String(I2S_DOUT).c_str());
  }
  //audio->setBufsize(30000,600000);
  audio->setVolumeSteps(100);
  audio->setVolume(this->vol);

#ifdef USE_AUDIO_MEDIA
#ifdef USE_FTP
  ftp.begin("ftp","ftp");    //username, password for ftp.   (default 21, 50009 for PASV)
#ifdef USE_DISPLAY
    display->bootMessage(1,"FTP Server started");
#endif
#endif
#endif // USE_AUDIO_MEDIA

//  audio.setAudioTaskCore(1);
#ifdef USE_DISPLAY                   
//  display.cp437(true);
//  html_info += display.html_info;
#endif
//  audio_set_mode(Off);  
#ifdef USE_AUDIO_SPEAKER
  default_mode = Speaker;
#endif
#ifdef USE_AUDIO_MEDIA
  default_mode = Media;
#endif
#ifdef USE_AUDIO_RADIO
  app_no_default = app_no_radio;
#endif

#ifdef USE_AUDIO_MEDIA
  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
//  TJpgDec.setJpgScale(JPG_SCALE);
  // The byte order can be swapped (set true for TFT_eSPI)
//  TJpgDec.setSwapBytes(false);
  // The decoder must be given the exact name of the mcu buffer function above
//  TJpgDec.setCallback(tft_output);

//  convert_jpg_files();
#endif
  write2log(LOG_MODULE,1,"End audiomodul.begin()");
  Serial.printf("audio.begin ENDE => Rotary app: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->val(), rotary->min(), rotary->max(), this->rot_last_val);
#ifdef DEBUG_SERIAL
  Serial.println("Auiomodul begin ende");
#endif
}

bool AudioModul::set(const String& _cmnd, const String& _val) {
  bool retval = false;
  String myvalue = _val;
  if ( Switch_OnOff::set(_cmnd, _val) ) {
    // Ein- und Ausschalten erfolgt im Elternobjekt
    // Schalter ist aus und app_no ist nicht 0
    if ((! this->switch_is_on) && (this->app_no != app_no_off)) {
#ifdef USE_AUDIO_RADIO    
      if (this->app_no == app_no_radio) {
        this->radio_off();
      }
#endif
#ifdef USE_AUDIO_MEDIA
      if (this->app_no == app_no_media) {
        this->media_off();
      }
#endif
      this->app_no_last = this->app_no;
      this->app_no = app_no_off;
      this->vol = 0;
#ifdef USE_ROTARY
      rotary->app_set(0,0,0,100,0);
#endif
      write2log(LOG_MODULE,1,"audio_set_app: case Off");
    // Schalter ist ein und app_no ist 0
    }
    if (( this->switch_is_on) && (this->app_no == app_no_off)) { 
      this->vol = AUDIO_ON_MIN_VOL; 
      this->app_no = app_no_default;
      this->display_update_now = true;
      this->html_update_set = true;
#ifdef USE_AUDIO_RADIO
      if (this->app_no == app_no_radio) {
        this->radio_on();
      }
#endif
#ifdef USE_AUDIO_MEDIA
      if (this->app_no == app_no_media) {
        this->media_on();
      }
#endif
    }
#ifdef USE_ROTARY
    rotary->app_set(this->app_no,0,0,100,this->vol);
#endif
    this->display_update_now = true;
    this->html_update_set = true;
    retval = true;
  } else {
    std::replace(myvalue.begin(),myvalue.end(),'\n',' ');
    // Lautstärke ändern
    if ( _cmnd == String("audio_vol") ) {
      if (this->app_no == app_no_off) {
        if (myvalue.toInt() >= AUDIO_ON_MIN_VOL) {
          this->app_no = this->app_no_last;
          this->vol = myvalue.toInt();
          do_switch(true);
#ifdef USE_AUDIO_RADIO
          if (this->app_no == app_no_radio) {
            this->radio_on();
          }
#endif //USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
          if (this->app_no == app_no_media) {
            this->media_on();
          }
#endif //USE_AUDIO_MEDIA
        }
        this->display_update_now = true;
      // Mode ist nicht OFF  
      } else {
        if (myvalue.toInt() < AUDIO_ON_MIN_VOL) {
#ifdef USE_AUDIO_RADIO
          if (this->app_no == app_no_radio) {
            this->radio_off();
          }
#endif
#ifdef USE_AUDIO_MEDIA
          if (this->app_no_last == app_no_media) {
            this->media_off();
          }
#endif
          this->app_no_last = this->app_no;
          this->app_no = app_no_off;
          this->vol = 0;
          do_switch(false);
          this->display_update_now = true;
        // Nur Lautstärke ändern, ohne die Anlage aus- oder einzuschalten
        } else {
          this->vol = myvalue.toInt();
          // Audiomodul einstellen
          audio->setVolume(this->vol);
          this->display_update_vol = true;
        }
      }
#ifdef USE_ROTARY
      // Rotary einstellen
      this->rotary->app_set(this->app_no,0,0,100,this->vol);
#endif
      this->html_update_set = true;
      retval = true;
    } // cmnd audio_vol
#ifdef USE_AUDIO_RADIO
    // Set for radio
    // Radio einschalten => Wiiedergabe
    if ( _cmnd == String("audio_radio") ) {
      if ( this->vol < AUDIO_ON_MIN_VOL ) this->vol = AUDIO_ON_MIN_VOL;
      this->app_no = app_no_radio;
      this->radio_on();
#ifdef USE_ROTARY
      rotary->app_set(1,0,0,100,this->vol);
#endif      
      this->display_update_now = true;
      this->html_update_set = true;
      retval = true;
    }
    // In den Settings für das Radio werden die Sender eingestellt.
    if ( (_cmnd == String("audio_get_settings")) &&  ( this->app_no == app_no_radio ) ) {
      this->radio_send_stn2web();
      retval = true;
    }
    // Radio: Sender einstellen
    if ( _cmnd == String("audio_radio_set_stn") ) {
      for (uint8_t i=0; i<MAXSTATIONS; i++) {
        if (strcmp(this->radio_station[i].url, myvalue.c_str()) == 0) {
          this->radio_cur_station = i;    
          retval = true;
          this->html_update_set = true;
        }
      }
      this->radio_on();
    }
    // Radio: Sender Name speichern ueber Webinterface
    if ( _cmnd == String("audio_radio_save_stn_name") ) {
      snprintf(this->radio_station[this->radio_cur_station].name,STATION_NAME_LENGTH,"%s",_val.c_str());
      this->radio_save_stations();
      retval = true;
    }
    // Radio: Sender URL speichern ueber Webinterface
    if ( _cmnd == String("audio_radio_save_stn_url") ) {
      snprintf(this->radio_station[this->radio_cur_station].url,STATION_URL_LENGTH,"%s",_val.c_str());
      this->radio_save_stations();
      retval = true;
    }
#endif
#ifdef USE_AUDIO_MEDIA
    // Set for mediaplayer
    // Schaltet den Mediaplayer an, falls er nicht an ist und spielt das aktuelle Lied ab.
    if ( _cmnd == String("audio_media") ) {
      if ( this->app_no != app_no_media ) audio_set_mode(app_no_media);
      audio_media_play(audio_media_cur_album, audio_media_cur_song);
      retval = true;
    }
    // In den Settings für den Mediaplayer werden die Alben dargestellt.
    if ( (_cmnd == String("audio_get_settings")) && (this->app_no == app_no_media) ) {
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
      display->screen_media();
      rotary->app_set(2,1);
      rotary->max_set(100); //Durch die maximale Anzahl der Alben ersetzen
      retval = true;
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
      this->media_play(this->media_cur_album,this->media_cur_song);
      retval = true;
    }
#endif
  }
  this->app_changed = false;
  return retval;
}

void AudioModul::html_init(String& _html_init) {
  Switch_OnOff::html_init(_html_init);
  this->append_comma(_html_init);
//  _html_init += String("\"audio_show\":1,\"audio_trebas_enable\":0,\"audio_media_active\":1,\"audio_settings\":1");
  _html_init += String("\"audio_show\":1,\"audio_trebas_enable\":0,\"audio_settings\":1");
  _html_init += String(",\"audio_vol\":") + String(this->vol);
  #ifdef USE_AUDIO_RADIO
  _html_init += String(",\"audio_radio_show\":1");
  if (this->app_no == app_no_radio) {
    _html_init += String(",\"audio_radio_sw\":1") +
                  String(",\"audiomsg1\":\"") + this->radio_stationname + String("\"") +
                  String(",\"audiomsg2\":\"") + this->radio_streamtitle + String("\"") +
                  String(",\"audiomsg4\":\"") + this->bps + String("\"");
  } else { 
    _html_init += String(",\"audio_radio_sw\":0"); 
  }
  #else
  _html_init += String(",\"audio_radio_show\":0");
  #endif
  #ifdef USE_AUDIO_MEDIA
  _html_init += String(",\"audio_media_show\":1");
  if (this->app_no == app_no_media) {
    _html_init += String(",\"audio_media_sw\":1") +
                 String(",\"audiomsg1\":\"Artist: ") + this->media_artist_name + String("\"") +
                 String(",\"audiomsg2\":\"Song: ") + this->media_song_name + String("\"") +
                 String(",\"audiomsg3\":\"Album: ") + this->media_album_name + String("\"") +
                 String(",\"audiomsg4\":\"") + this->kbs + String("\"");
  } else {
    _html_init += String(",\"audio_media_sw\":0");
  }
  #else
  _html_init += String(",\"audio_media_show\":0");
  #endif
  #ifdef USE_AUDIO_SPEAKER
  _html_init += String(",\"audio_speak_show\":1");
  #else
  _html_init += String(",\"audio_speak_show\":0");
  #endif
  this->html_update_set = true;
//  html_update(_html_init);
}

void AudioModul::html_info(String& _html_info) {
  Switch_OnOff::html_info(_html_info);
  this->append_comma(_html_info);
  _html_info += String("\"tab_head_audio\":\"I2S: MA98357\"") +
                String(",\"tab_line1_audio\":\"DOUT:#GPIO: ") + String(I2S_DOUT)+ String("\"") +
                String(",\"tab_line2_audio\":\"BCLK:#GPIO: ") + String(I2S_BCLK)+ String("\"") +
                String(",\"tab_line3_audio\":\"LRC:#GPIO: ") + String(I2S_LRC)+ String("\"");
#ifdef USE_ROTARY  
  this->append_comma(_html_info);
  _html_info += String("\"tab_head_rotary\":\"Rotary\"") +
                String(",\"tab_line1_rotary\":\"A-Pin:#GPIO: ") + String(ROT_S1)+ String("\"") +
                String(",\"tab_line2_rotary\":\"B-Pin:#GPIO: ") + String(ROT_S2)+ String("\"") +
                String(",\"tab_line3_rotary\":\"SW-Pin:#GPIO: ") + String(ROT_SW)+ String("\"") +
                String(",\"tab_line4_rotary\":\"Resistor:#") + String(ROTARY_ENCODER_R_PULLDOWN? "pulldown" : "pullup")+ String("\"");
#endif
#ifdef USE_DISPLAY
  this->append_comma(_html_info);
  display->html_info(_html_info);
#endif
#ifdef USE_AUDIO_MEDIA
  this->append_comma(_html_info);
  _html_info += String("\"tab_head_sdcard\":\"SD Card\"") +
                String(",\"tab_line1_sdcard\":\"MOSI:#GPIO: ") + String(SD_MOSI)+ String("\"") +
                String(",\"tab_line2_sdcard\":\"MISO:#GPIO: ") + String(SD_MISO)+ String("\"") +
                String(",\"tab_line3_sdcard\":\"SCK:#GPIO: ") + String(SD_SCK)+ String("\"") +
                String(",\"tab_line4_sdcard\":\"CS:#GPIO: ") + String(SD_CS)+ String("\"") +
                String(",\"tab_line5_sdcard\":\"Size/Used:# ") + String(sd_cardsize/1073741824)+ String(" GB / ") + String(sd_usedbytes/1073741824) + String(" GB \"");
#endif
}

void AudioModul::html_update(String& _html_update) {
  Switch_OnOff::html_update(_html_update);
  this->append_comma(_html_update);
  _html_update += String("\"audio_vol\":") + String(this->vol);
#ifdef USE_AUDIO_RADIO
  if (this->app_no == app_no_radio) {
    _html_update += String(",\"audio_radio_sw\":1") +
                    String(",\"audiomsg1\":\"") + this->radio_stationname + String("\"") +
                    String(",\"audiomsg2\":\"") + this->radio_streamtitle + String("\"") +
                    String(",\"audiomsg4\":\"") + this->bps + String("\"");
      }
#endif //USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
  else if (this->app_no == app_no_media) {
      _html_update += String(",\"audio_media_sw\":1") +
                   String(",\"audiomsg1\":\"Artist: ") + this->media_artist_name + String("\"") +
                   String(",\"audiomsg2\":\"Song: ") + this->media_song_name + String("\"") +
                   String(",\"audiomsg3\":\"Album: ") + this->media_album_name + String("\"") +
                   String(",\"audiomsg4\":\"") + this->kbs + String("\"");
  }
#endif //USE_AUDIO_MEDIA
  else {
    _html_update += String(",\"audio_radio_sw\":0") +
                    String(",\"audio_media_sw\":0") +
                    String(",\"audiomsg1\":\"\"") +
                    String(",\"audiomsg2\":\"\"") +
                    String(",\"audiomsg3\":\"\"") +
                    String(",\"audiomsg4\":\"\"");
  }
}

void AudioModul::start_timeout(time_t now) {
  this->timeout_set = true;
  this->timeout_start = now;
}

void AudioModul::loop(time_t now) {
// loop Funktion des Elternobjektes aufrufen
  Switch_OnOff::loop(now);
// loop Funktionen aller eingebundenen Objekte aufruen
#ifdef USE_DISPLAY  
  display->loop(now);
#endif
#ifdef USE_FTP
  ftp.handleFTP();
#endif
// Das Audio loop wird nur aufgerufen wenn Audio auch aktiv ist
  if (this->app_no != app_no_off) {
    audio->loop();
    vTaskDelay(1);

#ifdef USE_AUDIO_MEDIA
    if (this->app_no == app_no_media) {
      if (song_started) {
        song_starttime = now;
        song_started = false;
      } 
      if ( song_eof && (now - song_starttime) < 2 ) {
        song_eof = false;
        write2log(LOG_MODULE,1,"AudioModul::loop(): Media playing - within first 2s, ignore EOF");
      }
      if ( song_eof ) {
        write2log(LOG_MODULE,1,"AudioModul::loop(): Song EOF detected");
        song_eof = false;
        this->audio_media_cur_song++;
        if (! this->getSongByNumber(SD, this->audio_media_cur_album, this->audio_media_cur_song)) {
          this->media_cur_album++;
          this->media_cur_song = 0;
          if (! this->getSongByNumber(SD, this->audio_media_cur_album, this->audio_media_cur_song)) {
            this->media_cur_album = 0;
            this->media_cur_song = 0;
          }
        } 
        this->audio_media_play(this->audio_media_cur_album, this->audio_media_cur_song);
      }
    }
#endif
  } 
    
#ifdef USE_ROTARY
  rotary->loop(now);
  // Hier wird der Klickstream für die Bedienung mittels Drehregler definiert
  // Für alle Funktionen, die mit einen Klick auf den Drehregler ausgeführt werden,
  // wird die Variable timeout_set auf true gesetzt. Findet innerhalb der Zeit KLICK_TIMEOUT
  // kein weiterer Klick statt, wird die AKtion zurückgesetzt und die Anzeige auf den
  // zuletzt aktiven Stand zurückgesetzt.
  if ( this->timeout_set ) {
    if ((now - this->timeout_start) > KLICK_TIMEOUT) {
      rotary->app_set(this->last_app,0);
      this->timeout_set = false;
      switch(rotary->app()) {
        case 1:
          this->app_no = app_no_radio;
          break;
        case 2:
          this->app_no = app_no_media;
          break;
      }
      this->display_update_now = true;
      Serial.println("Timeout expired, reset rotary to app: " + String(rotary->app()) + " lev: 0");
    }
  }
  // Der Drehregler wurde gedreht.
  // Die darauf folgende Aktion ist abhängig von der App und der Ebene.
  if (rotary->valChanged()) {
#ifdef DEBUG_SERIAL
    Serial.printf("Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->val(), rotary->min(), rotary->max(), rot_last_val);
#endif
    if (rotary->app() == app_no_off) {
      // Wenn die App 0 (Modus OFF) ist, wird die Lautstärke erhöht und damit das Audio eingeschaltet.
      Serial.println("Rotary changed in App 0 (Off): set volume");
      this->set("audio_vol",String(this->rotary->val()));
    } 
#ifdef USE_AUDIO_RADIO
    if (rotary->app() == app_no_radio) {
      switch(rotary->lev()) {
        case 0:
          // Lautstärke einstellen
          this->set("audio_vol",String(this->rotary->val()));
          this->display_update_now = true;
        break;
        case 1:
          // Sender Auswahl
          start_timeout(now);
          this->rot_last_val = rotary->val();
          this->display_update_now = true;
        break;
      }
    }
#endif // USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
      if (rotary->app() == app_no_media) {
        switch(rotary->lev()) {
          case 0:
        // Lautstärke einstellen
            this->change_from_rotary = true;
            this->set("audio_vol",String(this->rotary->val()));
            this->change_from_rotary = false;
          break;
          case 1: {
        // Album auswählen
            start_timeout(now);
            audio_media_sel_album = rotary->val();
            audio_media_sel_song = 0;
            getSongByNumber(SD, audio_media_sel_album, 0);
#ifdef USE_DISPLAY
            display->media_select_album(audio_media_album_name, cd_bmp);
#endif
          }
          break;
          case 2: {
          // Musikstück auswählen
            start_timeout(now);
            audio_media_sel_song = rotary->val();
            getSongByNumber(SD, audio_media_sel_album, audio_media_sel_song);
#ifdef USE_DISPLAY
            display->media_select_song(audio_media_album_name, audio_media_song_name, cd_bmp);

#endif
          }
          break;
          case 3:
          // Album wechseln
          break;
        }
      }
      break;  // case Media
#endif // USE_AUDIO_MEDIA
// Settimgs
    if (rotary->app() == app_no_max+1) {
#ifdef DEBUG_SERIAL
      Serial.printf("---Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->val(), rotary->min(), rotary->max(), rot_last_val);
#endif
      start_timeout(now);
      switch(rotary->lev()) {
        case 0: {
          this->app_no_new = rotary->val();
          Serial.println("New mode selected: " + String(this->app_no_new));
          display_update_now = true;
        }
        break;
      } // switch rotary->lev()
    }
#ifdef DEBUG_SERIAL
    Serial.printf(">>>Rotary changed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->val(), rotary->min(), rotary->max(), rot_last_val);
#endif
  }  //Ende rotary.valChanged
//  Der Button wurde kurz gedrückt
//  Hier wird eine Aktion innerhalb der App ausgeführt. 
//  Die Art der Aktion hängt von der App und der Ebene ab.
  if (rotary->buttonShortPressed) {
    uint8_t new_lev;
    rotary->lev_up();
    rotary->buttonShortPressed = false;
#ifdef DEBUG_SERIAL
    Serial.printf("Rotary short Pressed App: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->get_max_lev(rotary->app()), rotary->val(), rotary->min(), rotary->max(), rot_last_val);
#endif
    start_timeout(now);
    app_no_last = rotary->app(); 
#ifdef USE_AUDIO_RADIO
    // Aktionen nach kurzen Druck beim Radio
    if (rotary->app() == app_no_radio) {
      switch(rotary->lev()) {
        // Radio Ebene 1: Neuen Sender auswählen
        case 1:
          rotary->min_set(0);
          rotary->max_set(MAXSTATIONS-1);
          rotary->val_set(radio_cur_station);
          this->app_no = app_no_radio_select;
          this->display_update_now = true;
        break;
        // Radio Ebene 2: Neuen Sender übernehmen und Level zurück auf 0 => Play
        case 2:
#ifdef DEBUG_SERIAL
          Serial.print("Radio neuer Sender: ");
          Serial.println(radio_station[rot_last_val].name);
          Serial.print("Radio alter Sender: ");
          Serial.println(radio_station[radio_cur_station].name);
#endif
          if (radio_cur_station != rot_last_val) {
            set(String("audio_radio_set_stn"),String(radio_station[rot_last_val].url));
            this->html_update_set = true;
            this->display_update_now = true;
          }
          this->app_no = app_no_radio;
          rotary->app_set(rotary->app(),0,0,100,this->vol);
          this->timeout_set = false;
        break;
      }
    }
#endif //USE_AUDIO_RADIO
#ifdef USE_AUDIO_MEDIA
      case MODE_MEDIA: {
        switch(rotary->lev_up()) {
          // Media Ebene 1: Album auswählen
          case 1: {
            media_changemode = true;
            media_sel_album = media_cur_album;
            media_sel_song = media_cur_song;
            rotary->val_set(media_sel_album);
#ifdef DEBUG_SERIAL
            Serial.printf("Media change Album Lev 1: App: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->get_max_lev(rotary->app()), rotary->val(), rotary->min(), rotary->max(), rot_last_val);
#endif
#ifdef USE_DISPLAY
            getSongByNumber(SD, media_sel_album, 0);
            display->media_select_album(media_album_name, cd_bmp);
#endif
          }
          break;
          // Media Ebene 2: Musikstück des Albums auswählen
          case 2: {
          //  audio_media_sel_album = rot_last_val;
            rotary->val_set(audio_media_sel_song);
            getSongByNumber(SD, media_sel_album, media_sel_song);
#ifdef DEBUG_SERIAL
            Serial.printf("Media change Song App: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->get_max_lev(rotary->app()), rotary->val(), rotary->min(), rotary->max(), rot_last_val);
#endif
#ifdef USE_DISPLAY
//            display->media_select_song(media_album_name, media_song_name, cd_bmp);
#endif
          }
          break;
          // Media Ebene 3: Player mit dem ausgewählten Stück starten
          case 3: {
            media_cur_album = media_sel_album;
            media_cur_song = media_sel_song;
#ifdef DEBUG_SERIAL
            Serial.printf("Media change execute: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->get_max_lev(rotary->app()), rotary->val(), rotary->min(), rotary->max(), rot_last_val);
#endif
            rotary->app_set(2,0);
#ifdef DEBUG_SERIAL
            Serial.printf("Test nach app_set: %u Lev: %u (Max: %u) Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->get_max_lev(rotary->app()), rotary->val(), rotary->min(), rotary->max(), rot_last_val);
#endif
            set(String("audio_media_play"),String("1"));
          }
          break;
        }
      }
      break; //case Media
#endif //USE_AUDIO_MEDIA
// Settings
    if (rotary->app() == app_no_max+1) {
      switch(rotary->lev()) {
        // Off: Ausschalten
        case 1: {
          if (app_no_new == app_no_off) {
            this->app_no = app_no_off;
            this->timeout_set = false;
            set(keyword, "0");
          }
#ifdef USE_AUDIO_RADIO
          if (app_no_new == app_no_radio) {
            this->app_no = app_no_radio;
            this->timeout_set = false;
            set(String("audio_radio"),String("1"));
          }
#endif
#ifdef USE_AUDIO_MEDIA
          if (new_mode == app_no_media) {
            set(String("audio_media"),String("1"));
          }
          if (new_mode == app_no_media_update) {
            set(String("audio_media_update"),String("1"));
          }
#endif
        }
        break; // case Settings
      } //switch(rotary->lev_up())
    } //rotary->app() == app_no_max+1
  }
// Button wurde lang gedrückt
// Einleitung zum Wechsel der App
// Hier wird nur das Symbol der aktuellen App angezeigt und der Rotary auf Settings gesetzt.
// Die App wird erst gewechselt, wenn der Button nach Auswahl (Drehen) erneut gedrückt wird.
  if (rotary->buttonLongPressed) {
    rotary->buttonLongPressed = false;
#ifdef DEBUG_SERIAL
    Serial.printf("Rotary Long Pressed App: %u Lev: %u Val: %u (Min: %u Max: %u) Last_val: %u\n", rotary->app(), rotary->lev(), rotary->val(), rotary->min(), rotary->max(), rot_last_val);
#endif
    start_timeout(now);
    this->app_no = app_no_max+1;
    rotary->app_set(app_no_max+1,0,0,app_no_max,0);
    this->display_update_now = true;
  }
#endif //USE_ROTARY
// Ende Klickstream Definition
} // loop


/*********************************************************************************************************
 * 
 *  Ab hier alles fürs Radio
 * 
 * 
**********************************************************************************************************/
#ifdef USE_AUDIO_RADIO

void AudioModul::radio_off() {
  write2log(LOG_MODULE,1,"Radio off");
  audio->stopSong();
  this->radio_streamtitle = String("");
  this->radio_stationname = String("");
  this->bps = String("");
}

void AudioModul::radio_on() {
  write2log(LOG_MODULE,1,"Radio on");
  radio_load_stations();
  if ( strlen(radio_station[radio_cur_station].url) > 10 ) {
    this->radio_stationname = "";
    this->radio_streamtitle = "";
    this->bps = "";      
    audio->connecttohost(radio_station[radio_cur_station].url);
    this->radio_stationname = radio_station[radio_cur_station].name;
    write2log(LOG_MODULE,2,"Switch to ",radio_station[radio_cur_station].url);
  }
}

void AudioModul::radio_send_stn2web() {
  String myjson = String("{");
  for (int i=0; i<MAXSTATIONS; i++) {
    if ( i > 0 ) myjson += String(",");
    myjson += String("\"audio_radio_add_stn") + String(i) + String("\":\"") +
             String(radio_station[i].url) + String(";") +
             String(radio_station[i].name) + String("\"");
  }
  myjson += String("}");
  sendWsMessage(myjson);
}

void AudioModul::radio_load_stations() {
  File f = LittleFS.open( "/sender.txt", "r" );
  if (f) {
    for (int i=0; i<MAXSTATIONS; i++) {
      snprintf(radio_station[i].name,STATION_NAME_LENGTH,"%s",f.readStringUntil('\n').c_str());
      snprintf(radio_station[i].url,STATION_URL_LENGTH,"%s",f.readStringUntil('\n').c_str());
      write2log(LOG_MODULE,2,radio_station[i].name,radio_station[i].url);
    }
    f.close();
  }
}

void AudioModul::radio_save_stations() {
  File f = LittleFS.open( "/sender.txt", "w" );
  if (f) {
    for (int i=0; i<MAXSTATIONS; i++) {
      write2log(LOG_MODULE,3,"Save Station: ",radio_station[i].name,radio_station[i].url);
      f.printf("%s\n",radio_station[i].name);
      f.printf("%s\n",radio_station[i].url);
    }
    delay(5);
    f.close();
  }
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
#ifdef USE_DISPLAY
  display->screen_media();
  display->setCursor(20,120);
  display->print(audio_media_album_name);
  display->setCursor(30,150);
  display->print(audio_media_song_name);
#endif
  String myjson = String("{\"audio_media_sw\":1}");
  sendWsMessage(myjson);
}

void AudioModul::media_off() {
  audio->stopSong();
  this->audio_media_album_name = String("");
  this->audio_media_artist_name = String("");
  this->audio_media_song_name = String("");
  this->audio_kbs = String("");
}

void AudioModul::audio_media_get_album_for_web() {
  String myjson = String("{");
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
      if ( setKomma ) myjson += String(",");
      myjson += String("\"audio_media_add_album_d") + String(dirNo) + String("f0") + String("\":\"A#") + String(dirNo) + 
               String("#0#") + String(dir.name()) + String("\"");
      if (myjson.length() > 500) {
        myjson += String("}");
        sendWsMessage(myjson);
        myjson = String("{");
        setKomma = false;
      } else {
        setKomma = true;
      }
      dirNo++;
    }
    dir = root.openNextFile();
  }
  myjson += String("}");
  sendWsMessage(myjson);
  if ( dir ) dir.close();
  root.close();
}

void AudioModul::audio_media_get_songs_for_web(uint16_t reqDirNo) {
  String myjson = String("{");
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
            if (fileNo > 0) myjson += String(",");
            // Datei gefunden, jetzt den Eintrag für die Weboberfläche erstellen
            // Der Eintrag hat das Format: "audio_media_add_album_d0f0":"A#0#0#filename.mp3"
            // Dabei steht A für Album, 0 für die Albumnummer, 0 für die Dateinummer und filename.mp3 für den Dateinamen
            myjson += String("\"audio_media_add_album_d") + String(dirNo) + String("f") + String(fileNo) + String("\":\"T#") + 
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
  myjson += "}";
  sendWsMessage(myjson);
}

void AudioModul::audio_media_play(uint16_t _albumNo, uint16_t _songNo) {
  if (getSongByNumber(SD, _albumNo, _songNo)) {
    write2log(LOG_MODULE,2,"Play song: ",audio_media_song_name.c_str());
    String songPath = String("/") + audio_media_album_name + String("/") + audio_media_song_name;
    write2log(LOG_MODULE,2,"Song path: ",songPath.c_str());
#ifdef USE_DISPLAY
    display->screen_media();
#endif
    if (audio->connecttoFS(SD,songPath.c_str())) {
      write2log(LOG_MODULE,2,"audio_media_play: Playing ", songPath.c_str());
      song_started = true;
    }
  }
}

bool AudioModul::getSongByNumber(fs::FS &fs, uint16_t albumNo, uint16_t songNo) {
  bool retval = false;
  bool dir_found = false;
  bool file_found = false;
  String tmpstr;
  uint16_t dirNo = 0;
  uint16_t songFileNo = 0;
  File root = fs.open("/");
  if (!root) {
#ifdef DEBUG_SERIAL
    Serial.println("Failed to open directory");
#endif
    return false;
  }
  root.rewindDirectory();
  File dir = root.openNextFile();
  while (dir) {
    if (dir.isDirectory()) {
      if ( dirNo == albumNo ) {
        audio_media_album_name = String(dir.name());
        // Jetzt die Dateien in dem Verzeichnis durchgehen
        dir.rewindDirectory();
        songFileNo = 0;
        // Alle Dateien im Verzeichnis durchgehen
        File file = dir.openNextFile();
        while (file) {
          tmpstr = String(file.name());
          if ( tmpstr.endsWith(".mp3") ) {
            if ( songFileNo == songNo ) {
              audio_media_song_name = String(file.name());
              write2log(LOG_MODULE,4,"Found song:", String(songNo).c_str(), ":",audio_media_song_name.c_str());
              file_found = true;
            }
            songFileNo++;
          }
          file = dir.openNextFile();
        }
        dir_found = true;
        audio_media_album_name = String(dir.name());
        break;
      }
      dirNo++;
    }
    dir = root.openNextFile();
  }
  root.close();
  if (! (dir_found && file_found) ) {
    return false;
  }
  return true;
}

#endif //USE_AUDIO_MEDIA
#endif //USE_ESP32
