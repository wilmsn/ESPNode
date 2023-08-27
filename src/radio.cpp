#include "radio.h"

#include <mydisplay.h>
#include <myencoder.h>
#include <myaudio_ESP8266.h>

MyDisplay mydisplay;
MyEncoder myencoder;
MyAudio   myaudio;

void Radio::stationLayout() {
  Serial.println(tmpStation);
  Serial.println(station[tmpStation].name);
  mydisplay.clear();
  mydisplay.writeLine(3,station[tmpStation].name,ST77XX_GREEN, false);
  mydisplay.writeLine(4,station[tmpStation+1].name,ST77XX_RED, false);
  mydisplay.writeLine(5,station[tmpStation+2].name,ST77XX_RED, false);
  if (tmpStation > 0) {
    mydisplay.writeLine(2,station[tmpStation-1].name,ST77XX_RED, false);
  }
  if (tmpStation > 1) {
    mydisplay.writeLine(1,station[tmpStation-2].name,ST77XX_RED, false);
  }
}

void Radio::radioLayout() {
  mydisplay.clear();
  mydisplay.displayVol(curVol);
  mydisplay.writeLine(3,station[curStation].name,ST77XX_GREEN, false);

}

void Radio::begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         bool start_value, bool on_value, uint8_t slider_no,
                         const char* slider_mqtt_name) {
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, start_value, on_value, curVol << 2, slider_no, slider_mqtt_name);
  
  isOn = true;
  strncpy(station[0].url,"http://stream.lokalradio.nrw/4459m27",150);
  strncpy(station[0].name,"Radio Kiepenkerl",32);

  strncpy(station[1].url,"http://streams.radio21.de/nrw/mp3-192/web",150);
  strncpy(station[1].name,"Radio 21 NRW",32);

  strncpy(station[2].url,"http://icecast.ndr.de/ndr/njoy/live/mp3/128/stream.mp3",150);
  strncpy(station[2].name,"N-JOY",32);

  strncpy(station[3].url,"http://rnrw.cast.addradio.de/rnrw-0182/dein90er/low/stream.mp3",150);
  strncpy(station[3].name,"NRW 90er",32);

  myencoder.begin();
  myencoder.setPos(curVol);
  myencoder.setLimits(0,21);

  mydisplay.begin();
  mydisplay.displayVol(curVol);
  mydisplay.writeLine(3,station[curStation].name,ST77XX_GREEN, false);

  myaudio.begin();
  myaudio.setVol(curVol);
  myaudio.startUrl(station[curStation].url);
}

void Radio::loop(void) {
  myaudio.loop();
  myencoder.loop();
  mydisplay.loop();
  if (myaudio.newTitle()) {
    mydisplay.writeLine(4, myaudio.showTitle(), ST77XX_YELLOW, true);
  }
  if (myencoder.newPos()) {
    curVol = myencoder.readPos();
    Serial.print("##>Volume: ");
    Serial.println(curVol);
    Serial.println(myaudio.showTitle());
    myaudio.setVol(curVol);
  }
  if (isOn != get_switch_val()) {
    curVol = get_slider_val()/10;
    myencoder.setPos(curVol);
    myaudio.setVol(curVol);
    if ( isOn != get_switch_val() ) {
      if ( get_switch_val() ) {
        myaudio.startUrl(station[curStation].url);
        isOn = true;
      } else {
        myaudio.off();
        isOn = false;
      }
    }

    Serial.print("Schalter: ");
    Serial.println(get_switch_val()?"Ein":"Aus");
    obj_changed = false;
  }
  if (curVol != (get_slider_val() >> 2) ) {
    curVol = get_slider_val() >> 2;
    myencoder.setPos(curVol);
    myaudio.setVol(curVol);
    Serial.print("Slider: ");
    Serial.println(get_slider_val() >>2);
    obj_changed = false;
  }
  Serial.print(get_switch_val()?"Ein ":"Aus ");
  Serial.print(curVol);
  Serial.print("  ");
  Serial.println(get_slider_val() >>2);
}

