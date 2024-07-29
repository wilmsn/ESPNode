#include "config.h"
#ifdef USE_SWITCH_ONOFF
#include "switch_onoff.h"
#include "common.h"

// Startet als Schalter mit Regler der einen HW-Pin mittels PWM steuert
// Fall 5
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         bool start_value, bool on_value, uint8_t hw_pin1, uint8_t slider_val, uint8_t slider_max_val, uint8_t slider_no,
                         const char* slider_mqtt_name, const char* slider_label) {
  obj_slider_used = true;
  obj_slider_val = slider_val;
  obj_slider_no = slider_no;
  obj_slider_max_val = slider_max_val;
  obj_slider_mqtt_name = slider_mqtt_name;
  obj_slider_label = slider_label;
  // Imitialisierung über  Fall 2
  begin(html_place, label, mqtt_name, keyword, start_value, on_value, hw_pin1);
}

// Startet als Schalter mit Regler ohne HW Bezug
// Fall 4
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         bool start_value, bool on_value, uint8_t slider_val, uint8_t slider_max_val, uint8_t slider_no,
                         const char* slider_mqtt_name, const char* slider_label) {
  obj_slider_used = true;
  obj_slider_val = slider_val;
  obj_slider_no = slider_no;
  obj_slider_max_val = slider_max_val;
  obj_slider_mqtt_name = slider_mqtt_name;
  obj_slider_label = slider_label;
  // Initialisierung über Fall 1
  begin(html_place, label, mqtt_name, keyword, start_value, on_value);
}

// Startet als Schalter der zwei HW-Pins steuert
// Fall 3
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         bool start_value, bool on_value, uint8_t hw_pin1, uint8_t hw_pin2) {
  set_hw_pin(hw_pin1,hw_pin2);
  // Initialisierung über Fall 2
  begin(html_place, label, mqtt_name, keyword, start_value, on_value, hw_pin1);
  obj_hw_pin2_used = true;
  pinMode(hw_pin2, OUTPUT);
}

// Startet als Schalter der einen HW-Pin steuert
// Fall 2
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         bool start_value, bool on_value, uint8_t hw_pin1) {
  set_hw_pin(hw_pin1);                    
  pinMode(hw_pin1, OUTPUT);
  // Initialisierung über Fall 1
  begin(html_place, label, mqtt_name, keyword, start_value, on_value);
}

// Startet als Schalter ohne HW-Pin
// Fall 1
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
                         bool start_value, bool on_value) {
  Base_Generic::begin(html_place, label, mqtt_name, keyword);
  obj_on_value = on_value;
  obj_value = start_value;
  do_switch(start_value);
  set_changed(false);
}

void Switch_OnOff::do_switch(bool state) {
  if ( state ) {
    if ( obj_slider_used ) {
      if ( obj_hw_pin1_used ) {
        analogWrite(obj_hw_pin1, obj_slider_val);
      }
    } else {
      if ( obj_hw_pin1_used ) {
        digitalWrite(obj_hw_pin1, obj_on_value);
      }
      if (obj_hw_pin2_used) {
        digitalWrite(obj_hw_pin2, obj_on_value);
      }
    }
  } else {
    if (obj_hw_pin1_used) {
      digitalWrite(obj_hw_pin1, ! obj_on_value);
    }
    if (obj_hw_pin2_used) {
      digitalWrite(obj_hw_pin2, ! obj_on_value);
    }
  }
  obj_value = state;

  obj_html_stat_json = "{\"";
  obj_html_stat_json += obj_html_place;
  obj_html_stat_json += "\":";
  obj_html_stat_json += state? "1":"0";

  if (obj_slider_used) {
    obj_html_stat_json += ",\"slider";
    obj_html_stat_json += String(obj_slider_no);
    obj_html_stat_json += "val\":";
    obj_html_stat_json += String(obj_slider_val);
  }

  obj_html_stat_json += "}";

  if (obj_slider_used) {
    obj_mqtt_json = "\"";
    obj_mqtt_json += obj_slider_mqtt_name;
    obj_mqtt_json += "\":";
    obj_mqtt_json += String(obj_slider_val);
  }

  obj_values_str = state? "1":"0";
}

bool Switch_OnOff::set(const String& keyword, const String& value) {
  bool retval = false;
  if ( keyword_match(keyword) ) {
    if ( (value == "0") || (value == "aus") || (value == "Aus") || (value == "off") | (value == "Off") ) {
      do_switch(false);
      set_changed(true);
      retval = true;
    }
    if ( (value == "1") || (value == "ein") || (value == "Ein") || (value == "on") | (value == "On") ) {
      do_switch(true);
      set_changed(true);
      retval = true;
    }
    if ( (value == "2") || (value == "umschalten") || (value == "Umschalten") || (value == "toggle") | (value == "Toggle") ) {
      do_switch(! obj_value);
      set_changed(true);
      retval = true;
    }
    if ( value.startsWith("S:") ) {
      obj_slider_val = value.substring(2,value.length()).toInt();
      do_switch(obj_value); 
      set_changed(true);
      retval = true;
    }
  }
  return retval;
}

void Switch_OnOff::html_create() {
#define TMPSTR_SIZE    50
  char tmpstr[TMPSTR_SIZE+1];
  snprintf(tmpstr,TMPSTR_SIZE,"{\"%s\":%s}",obj_html_place.c_str(),obj_value? "1" : "0");
  ws.textAll(tmpstr);
  snprintf(tmpstr,TMPSTR_SIZE,"{\"%s_label\":\"%s\"}",obj_html_place.c_str(),obj_label.c_str());
  ws.textAll(tmpstr);
  snprintf(tmpstr,TMPSTR_SIZE,"{\"%s_format\":\"x\"}",obj_html_place.c_str());
  ws.textAll(tmpstr);
  if (obj_slider_used) {
    snprintf(tmpstr,TMPSTR_SIZE,"{\"slider%u\":1}",obj_slider_no);
    ws.textAll(tmpstr);
    snprintf(tmpstr,TMPSTR_SIZE,"{\"slider%ulabel\":\"%s\"}",obj_slider_no,obj_slider_label.c_str());
    ws.textAll(tmpstr);
    snprintf(tmpstr,TMPSTR_SIZE,"{\"slider%uname\":\"%s\"}",obj_slider_no,obj_keyword.c_str());
    ws.textAll(tmpstr);
    snprintf(tmpstr,TMPSTR_SIZE,"{\"slider%uval\":%u}",obj_slider_no,obj_slider_val);
    ws.textAll(tmpstr);
    snprintf(tmpstr,TMPSTR_SIZE,"{\"slider%umax\":%u}",obj_slider_no,obj_slider_max_val);
    ws.textAll(tmpstr);
  }
}

uint8_t Switch_OnOff::get_slider_val() {
  return obj_slider_val;
}

bool Switch_OnOff::get_switch_val() {
  return obj_value;
}

void Switch_OnOff::set_switch(uint8_t val) {
  set(obj_keyword, String(val));
}

void Switch_OnOff::set_slider_label(const char* label) {
  obj_slider_label = label;
}

void Switch_OnOff::set_slider(uint8_t val) {
  obj_slider_val = val;
  do_switch(obj_value);
  set_changed(true);
}

void Switch_OnOff::set_slider_max_value(uint8_t val) {
  obj_slider_max_val = val;
}

#endif