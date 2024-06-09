#include "config.h"
#ifdef USE_ROTARYTEST

#define USE_ROTARYMODUL
#include "rotarymodul.h"

RotaryModul rotarymodul;

void RotaryTest::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword)  {
/*
   void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               bool start_value, bool on_value, uint8_t slider_val, uint8_t slider_no,
               const char* slider_mqtt_name, const char* slider_label);
*/
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, true, true, 0, 1, "volume", "Lautstärke");
  set_slider_max_value(100);
  set_slider(20);
  rotarymodul.begin();
  rotarymodul.initLevel(0,0,20,100);
  rotarymodul.initLevel(1,0,0,10);
  rotarymodul.initLevel(2,0,0,2);
  
}

void RotaryTest::loop() {
  rotarymodul.loop();
  switch (rotarymodul.changed()) {
  case 1:
    if ( rotarymodul.curLevel() == 0 ) set_slider(rotarymodul.curVal());
    if ( ! obj_value && (rotarymodul.curVal() > 0) ) set_switch(1);
    if ( obj_value && (rotarymodul.curVal() == 0) ) set_switch(0);
//    Serial.printf("Rotary Level %u Position: %u Slider: %u\n", rotarymodul.curLevel(), rotarymodul.curVal(), get_slider_val());
    break;
  case 2:
    Serial.printf("Rotary Level %u Position: %u Slider: %u\n", rotarymodul.curLevel(), rotarymodul.curVal(), get_slider_val());
  break;
  case 3: 
    Serial.printf("Rotary long press \n");
    set_switch(2);
    rotarymodul.setLevel(0);
    rotarymodul.setValue(0);
  break;
  default:
  break;
  }
  if ( changed() ) {
//    Serial.printf("Web Slider Position: %u \n", get_slider_val());
    if ( rotarymodul.curLevel() == 0 ) {
      rotarymodul.setValue( get_slider_val() );  
      Serial.printf("Rotary Level %u Position: %u Slider: %u\n", rotarymodul.curLevel(), rotarymodul.curVal(), get_slider_val());
    }
  }
}

#endif