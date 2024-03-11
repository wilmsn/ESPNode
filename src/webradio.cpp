#include "config.h"

#ifdef _WEBRADIO_H_
//#include "webradio.h"
#include "Audio.h"

#define I2S_DOUT     25
#define I2S_BCLK     27
#define I2S_LRC      26

Audio audio;

void Webradio::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword) {
  bool start_value = true;
  bool on_value = true;
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
  audio.setBufsize(30000,-1);
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, start_value, on_value, 100, 1, "Lautstärke");
  webradio_on();
//  Serial.println("---- on ------");
//  audio.connecttohost("http://stream.lokalradio.nrw/4459m27");
  mystate = true;
}

void Webradio::webradio_off() {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("webadio_off");
#endif
    audio.setVolume(0);
}

void Webradio::webradio_on() {
  if (audio.connecttohost("http://stream.lokalradio.nrw/4459m27")) {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("Playing Radio Kiepenkerl");
#endif
  }
  set_vol();  
}

void Webradio::set_vol() {
    slider_val_old = obj_slider_val;
    myvol = (uint8_t)(obj_slider_val / 11);
    Serial.print("New Volume: ");
    Serial.println(myvol);
    audio.setVolume(myvol);
}

void Webradio::loop() {
  audio.loop();
  if (slider_val_old != obj_slider_val ) {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("Volume");
#endif
    set_vol();
  }
  if (obj_value != mystate) {
    if (obj_value) { 
#if defined(DEBUG_SERIAL_MODULE)
      Serial.println("Radio ON");
#endif
      webradio_on();
    } else {
#if defined(DEBUG_SERIAL_MODULE)
      Serial.println("Radio OFF");
#endif
      webradio_off();
    }
    mystate = obj_value;
  }
}

void audio_info(const char *info){
#ifdef DEBUG_SERIAL_MODULE
    Serial.println("****Info****");
    Serial.println(info);
    Serial.println("---------------");
#endif
}

void audio_showstreamtitle(const char *info){
#ifdef DEBUG_SERIAL_MODULE
    Serial.println("####Titel####");
    Serial.println(String(info));
    Serial.println("-------------");
#endif
}

void audio_bitrate(const char *info) {
    char bpsInfo[5];
    bpsInfo[0] = info[0];
    bpsInfo[1] = info[1];
    bpsInfo[2] = info[2];
    bpsInfo[3] = 'K';
    bpsInfo[4] = 0;
#ifdef DEBUG_SERIAL_MODULE
    Serial.println("####Bitrate####");
    Serial.println(String(bpsInfo));
    Serial.println("-------------");
#endif
}

void audio_showstation(const char *info){
    String sinfo = String(info);
    sinfo.replace("|", "\n");
#ifdef DEBUG_SERIAL_MODULE
    Serial.println("####Station####");
    Serial.println(String(info));
    Serial.println("-------------");
#endif
}






#endif
