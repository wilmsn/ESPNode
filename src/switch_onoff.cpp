#include "switch_onoff.h"

// Startet als Schalter mit Regler der einen HW-Pin steuert
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         uint8_t hw_pin1, bool start_value, bool on_value, uint8_t intensity, uint8_t slider_no,
                         const char* slider_mqtt_name) {
  obj_slider_used = true;
  obj_slider_val = intensity;
  obj_slider_no = slider_no;
  obj_slider_mqtt_name = slider_mqtt_name;
  begin(html_place, label, mqtt_name, keyword, hw_pin1, start_value, on_value);
}

// Startet als Schalter mit Regler ohne HW Bezug
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         bool start_value, bool on_value, uint8_t intensity, uint8_t slider_no,
                         const char* slider_mqtt_name) {
  obj_slider_used = true;
  obj_slider_val = intensity;
  obj_slider_no = slider_no;
  obj_slider_mqtt_name = slider_mqtt_name;
  begin(html_place, label, mqtt_name, keyword, start_value, on_value);
}

// Startet als Schalter der zwei HW-Pins steuert
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         uint8_t hw_pin1, uint8_t hw_pin2, bool start_value, bool on_value) {
  set_hw_pin(hw_pin1,hw_pin2);
  begin(html_place, label, mqtt_name, keyword, hw_pin1, start_value, on_value);
  obj_hw_pin2_used = true;
  pinMode(hw_pin2, OUTPUT);
}

// Startet als Schalter der einen HW-Pin steuert
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         uint8_t hw_pin1, bool start_value, bool on_value) {
  begin(html_place, label, mqtt_name, keyword, start_value, on_value);
  set_hw_pin(hw_pin1);                    
  pinMode(hw_pin1, OUTPUT);
}

// Startet als Schalter ohne HW-Pin
void Switch_OnOff::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
                         bool start_value, bool on_value) {
  Base_Generic::begin(html_place, label, mqtt_name, keyword);
  obj_on_value = on_value;
  obj_value = start_value;
  do_switch(start_value);
  obj_changed = false;
}

void Switch_OnOff::do_switch(bool state) {
  if ( state ) {
    if ( obj_slider_used ) {
      if ( obj_hw_pin1_used ) {
        analogWrite(obj_hw_pin1, obj_slider_val);
      }
    } else {
      digitalWrite(obj_hw_pin1, obj_on_value);
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

    obj_mqtt_json = "\"";
    obj_mqtt_json += obj_slider_mqtt_name;
    obj_mqtt_json += "\":";
    obj_mqtt_json += String(obj_slider_val);
  }
  obj_html_stat_json += "}";

  obj_values_str = state? "1":"0";
}

bool Switch_OnOff::set(const String& keyword, const String& value) {
  bool retval = false;
  if ( keyword_match(keyword) ) {
    if ( (value == "0") || (value == "aus") || (value == "Aus") || (value == "off") | (value == "Off") ) {
      do_switch(false);
      obj_changed = true;
      retval = true;
    }
    if ( (value == "1") || (value == "ein") || (value == "Ein") || (value == "on") | (value == "On") ) {
      do_switch(true);
      obj_changed = true;
      retval = true;
    }
    if ( (value == "2") || (value == "umschalten") || (value == "Umschalten") || (value == "toggle") | (value == "Toggle") ) {
      do_switch(! obj_value);
      obj_changed = true;
      retval = true;
    }
    if ( value.startsWith("S:") ) {
      obj_slider_val = value.substring(2,value.length()).toInt();
      do_switch(obj_value); 
      obj_changed = true;
      retval = true;
    }
  }
  return retval;
}

void Switch_OnOff::html_create_json_part(String& json) {
  json += ",\"";
  json += String(obj_html_place);
  json += "\":\"";
  json += obj_value? "1" : "0"; 
  json += "\",\"";
  json += String(obj_html_place);
  json += "_label\":\"";
  json += obj_label;
  json += "\",\"";
  json += String(obj_html_place);
  json += "_format\":\"x\"";
  if (obj_slider_used) {
    json += ",\"slider";
    json += String(obj_slider_no);
    json += "\":1,\"slider";
    json += String(obj_slider_no);
    json += "label\":\"Intensität\",\"slider";
    json += String(obj_slider_no);
    json += "name\":\"";
    json += obj_keyword;
    json += "\",\"slider";
    json += String(obj_slider_no);
    json += "val\":";
    json += obj_slider_val;
  }
}

uint8_t Switch_OnOff::get_slider_val() {
  return obj_slider_val;
}

bool Switch_OnOff::get_switch_val() {
  return obj_value;
}