#include "config.h"
#ifdef USE_SWITCH_ONOFF
#include "switch_onoff.h"
#include "common.h"

Switch_OnOff::Switch_OnOff(){
  hw_pin1 = 0;
  hw_pin2 = 0;
  hw_pin1_used = false;
  hw_pin2_used = false;
}

// Startet als Schalter mit Regler der einen HW-Pin mittels PWM steuert
// Fall 5
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _mqtt_name,  const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin1, uint8_t _slider_val, uint8_t _slider_max_val, uint8_t _slider_no,
                         const char* _slider_label, const char* _slider_mqtt_name, const char* _slider_keyword) {
  set_hw_pin(_hw_pin1);                    
  pinMode(_hw_pin1, OUTPUT);
  // Imitialisierung über  Fall 4
  begin(_html_place, _label, _mqtt_name, _keyword, _start_value, _on_value, _is_state, _slider_val, _slider_max_val, _slider_no,
        _slider_label, _slider_mqtt_name, _slider_keyword);
}

// Startet als Schalter mit Regler ohne HW Bezug
// Fall 4
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _mqtt_name,  const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, uint8_t _slider_val, uint8_t _slider_max_val, uint8_t _slider_no,
                         const char* _slider_label, const char* _slider_mqtt_name, const char* _slider_keyword) {
  slider_used = true;
  slider_val = _slider_val;
  slider_no = _slider_no;
  slider_max_val = _slider_max_val;
  slider_label = _slider_label;
  slider_mqtt_name = _slider_mqtt_name;
  slider_keyword = _slider_keyword;
  // Initialisierung über Fall 1
  begin(_html_place, _label, _mqtt_name, _keyword, _start_value, _on_value, _is_state);
}

// Startet als Schalter der zwei HW-Pins steuert
// Fall 3
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _mqtt_name,  const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin1, uint8_t _hw_pin2) {
  set_hw_pin(_hw_pin1,_hw_pin2);
  // Initialisierung über Fall 2
  begin(_html_place, _label, _mqtt_name, _keyword, _start_value, _on_value, _is_state, _hw_pin1);
  hw_pin2_used = true;
  pinMode(_hw_pin2, OUTPUT);
}

// Startet als Schalter der einen HW-Pin steuert
// Fall 2
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _mqtt_name,  const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin1) {
  set_hw_pin(_hw_pin1);
  pinMode(_hw_pin1, OUTPUT);
  // Initialisierung über Fall 1
  begin(_html_place, _label, _mqtt_name, _keyword, _start_value, _on_value, _is_state);
}

// Startet als Schalter ohne HW-Pin
// Fall 1
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state) {
  Base_Generic::begin(_html_place, _label, _mqtt_name, _keyword);
  switch_mqtt_name = _mqtt_name;
  mqtt_has_info = true;
  mqtt_has_stat = true;
  html_has_info = true;
  on_value = _on_value;
  switch_state = _start_value;
  is_state = _is_state;
  do_switch(switch_state);
  if (hw_pin1_used && hw_pin2_used ) {
    html_info = String("\"tab_head_")+html_place+String("\":\"Switch On Off\"")+
                String(",\"tab_line1_")+html_place+String("\":\"")+label+String(":#GPIO:")+String(hw_pin1) + String("\"") +
                String(",\"tab_line2_")+html_place+String("\":\"")+label+String(":#GPIO:")+String(hw_pin2) + String("\"");
    mqtt_info = String("\"GPIO_") + mqtt_name + String("\":\"") + String(hw_pin1) + String(", ") +
                String(hw_pin2);
  } else {
    if (hw_pin1_used ) {
      html_info = String("\"tab_head_")+html_place+String("\":\"Switch On Off\"")+
                  String(",\"tab_line1_")+html_place+String("\":\"")+label+String(":#GPIO:")+String(hw_pin1);
      mqtt_info = String("\"GPIO_") + mqtt_name + String("\":\"") + String(hw_pin1);
      mqtt_has_info = true;
      if (slider_used) {
        html_info += String(" (PWM)\"");
        mqtt_info += String(" (PWM)\"");
      } else {
        html_info += String("\"");
        mqtt_info += String("\"");
      }
    }
  }
}

void Switch_OnOff::do_switch(bool new_state) {
  Serial.println("do_switch");
  if ( new_state ) {
    if ( slider_used ) {
      if ( hw_pin1_used ) {
        analogWrite(hw_pin1, slider_val);
      }
    } else {
      if ( hw_pin1_used ) {
        digitalWrite(hw_pin1, on_value);
      }
      if (hw_pin2_used) {
        digitalWrite(hw_pin2, on_value);
      }
    }
  } else {
    if ( slider_used ) {
      if ( hw_pin1_used ) {
        analogWrite(hw_pin1, 0);
      }
    } else {
      if (hw_pin1_used) {
        digitalWrite(hw_pin1, ! on_value);
      }
      if (hw_pin2_used) {
        digitalWrite(hw_pin2, ! on_value);
      }
    }
  }
  switch_state = new_state;
  state = String(switch_state?"1":"0");
  
  html_json = String("\"") + html_place + String("\":") + String(switch_state?"1":"0");
  if (slider_used) {
    html_json += String(",\"slider") + String(slider_no) + String("val\":\"") + String(slider_val) + String("\"");
  }
  html_update();
  
  mqtt_stat = String("\"") + switch_mqtt_name+String("\":") + String(switch_state? "1":"0");
  if (slider_used) {
    mqtt_stat += String(",\"") + slider_mqtt_name + String("\":") + String(slider_val);
  }
  mqtt_stat_changed = true;
}

bool Switch_OnOff::set(const String& _cmnd, const String& _val) {
  bool retval = false;
  if ( keyword_match(_cmnd) || _cmnd == mqtt_name ) {
    if ( (_val == "0") || (_val == String("aus")) || (_val == String("Aus")) || (_val == String("off")) | (_val == String("Off")) ) {
      do_switch(false);
      retval = true;
    }
    if ( (_val == String("1")) || (_val == String("ein")) || (_val == String("Ein")) || (_val == String("on")) | (_val == String("On")) ) {
      do_switch(true);
      retval = true;
    }
    if ( (_val == String("2")) || (_val == String("umschalten")) || (_val == String("Umschalten")) || (_val == String("toggle")) | (_val == String("Toggle")) ) {
      do_switch(! switch_state);
      retval = true;
    }
  } else {
    if (slider_used) {
      if ( (_cmnd == switch_mqtt_name) || (_cmnd == slider_keyword) ) {
        slider_val = _val.toInt();
        do_switch(switch_state); 
        retval = true;
      }
    }
  }
  return retval;
}

void Switch_OnOff::set_hw_pin(uint8_t _pin1) {
  hw_pin1 = _pin1;
  hw_pin1_used = true;
}

void Switch_OnOff::set_hw_pin(uint8_t _pin1, uint8_t _pin2) {
  hw_pin1 = _pin1;
  hw_pin1_used = true;
  hw_pin2 = _pin2;
  hw_pin2_used = true;
}

uint8_t Switch_OnOff::get_slider_val() {
  return slider_val;
}

bool Switch_OnOff::get_switch_val() {
  return switch_state;
}

void Switch_OnOff::set_switch(uint8_t val) {
  set(keyword, String(val));
}

void Switch_OnOff::set_slider_label(const char* _label) {
  slider_label = _label;
}

void Switch_OnOff::set_slider(uint8_t _val) {
  slider_val = _val;
  do_switch(switch_state);
}

void Switch_OnOff::set_slider_max_value(uint8_t _val) {
  slider_max_val = _val;
}

void Switch_OnOff::html_init() {
  html_json = String("\"") + html_place + String("_label\":\"") + label + String("\"") +
              String(",\"") + html_place + String("_format\":\"x\"");
  if (slider_used) {
    html_json += String(",\"slider") + String(slider_no) + String("\":1") +
                 String(",\"slider") + String(slider_no) + String("label\":\"") + slider_label + String("\"") +
                 String(",\"slider") + String(slider_no) + String("name\":\"") + slider_keyword + String("\"") +
                 String(",\"slider") + String(slider_no) + String("max\":\"") + String(slider_max_val) + String("\"");
  }
  html_json += String(",\"") + html_place + String("\":") + String(switch_state?"1":"0");
  if (slider_used) {
    html_json += String(",\"slider") + String(slider_no) + String("val\":\"") + String(slider_val) + String("\"");
  }
  html_json_filled = true;
}
#endif