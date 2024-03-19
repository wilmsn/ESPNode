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
  mystate = true;
}

void Webradio::webradio_off() {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("webadio_off");
#endif
    audio.setVolume(0);
    audio.stopSong();
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
    char volstr[20];
    slider_val_old = obj_slider_val;
    myvol = (uint8_t)(obj_slider_val / 11);
    sprintf(volstr,"New Volume %u",myvol);
    write2log(LOG_SENSOR,1,volstr);
    audio.setVolume(myvol);
}

void Webradio::loop() {
  audio.loop();
  if (!audio.isRunning()) {
    webradio_off();
    webradio_on();
  }
  if (slider_val_old != obj_slider_val ) {
    set_vol();
  }
  if (obj_value != mystate) {
    if (obj_value) { 
      write2log(LOG_SENSOR,1,"Radio on");
      webradio_on();
    } else {
      write2log(LOG_SENSOR,1,"Radio off");
      webradio_off();
    }
    mystate = obj_value;
  }
}

void audio_info(const char *info){
    write2log(LOG_SENSOR,2,"Info:", info);
}

void audio_showstreamtitle(const char *info){
    write2log(LOG_SENSOR,2,"Titel:", info);
}

void audio_bitrate(const char *info) {
    char bpsInfo[5];
    bpsInfo[0] = info[0];
    bpsInfo[1] = info[1];
    bpsInfo[2] = info[2];
    bpsInfo[3] = 'K';
    bpsInfo[4] = 0;
    write2log(LOG_SENSOR,2,"Bitrate:", info);
}

void audio_showstation(const char *info){
    String sinfo = String(info);
    sinfo.replace("|", "\n");
    write2log(LOG_SENSOR,2,"Station:", info);
}






#endif
