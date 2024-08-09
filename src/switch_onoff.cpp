#include "config.h"
#ifdef USE_SWITCH_ONOFF
#include "switch_onoff.h"
#include "common.h"

Switch_OnOff::Switch_OnOff(){
  obj_hw_pin1 = 0;
  obj_hw_pin2 = 0;
  obj_hw_pin1_used = false;
  obj_hw_pin2_used = false;
}

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

  obj_html_stat = "{\"";
  obj_html_stat += obj_html_place;
  obj_html_stat += "\":";
  obj_html_stat += state? "1":"0";
  if (obj_slider_used) {
    obj_html_stat += ",\"slider";
    obj_html_stat += String(obj_slider_no);
    obj_html_stat += "val\":";
    obj_html_stat += String(obj_slider_val);
  }
  obj_html_stat += "}";
  ws.textAll(obj_html_stat.c_str());
  write2log(LOG_MODULE,1,obj_html_stat.c_str());

  obj_mqtt_stat = String("\"")+obj_mqtt_name+String("\":\"");
  if (obj_slider_used) {
    if (state) {
      obj_mqtt_stat += String("Ein (")+String(obj_slider_val)+String(")\"");
    } else {
      obj_mqtt_stat += String("Aus (0)\"");
    }
  } else {
    if (state) {
      obj_mqtt_stat += String("Ein\"");
    } else {
      obj_mqtt_stat += String("Aus\"");
    }
  }

  if (obj_hw_pin1_used && obj_hw_pin2_used ) {
    obj_html_info = String("{\"tab_head_")+obj_html_place+String("\":\"Switch on Off\"")+
                    String(",\"tab_line1\":\"")+obj_label+String(":#GPIO: ")+String(obj_hw_pin1)+String("\"")+
                    String(",\"tab_line2\":\"")+obj_label+String(":#GPIO: ")+String(obj_hw_pin2)+String("\"");
                    String(",\"tab_line3\":\"")+obj_label+String(":#Zustand: ")+String(obj_value?"ein ":"aus ")+String("\"}");
  } else {
    if (obj_hw_pin1_used ) {
      obj_html_info = String(",\"tab_head_")+obj_html_place+String("\":\"Switch on Off\"")+
                            String(",\"tab_line1_")+obj_html_place+String("\":\"")+obj_label+String(":#GPIO:")+String(obj_hw_pin1)+String("\"");
//                            String(",\"tab_line2\":\"")+obj_label+String(":#Zustand: ")+String(obj_value?1:0);
      if (obj_slider_used) {
//        obj_html_info += String("(")+String(obj_slider_val)+String("%)");
      }                      
//      obj_html_info += String("\"");
    }
  }
}

bool Switch_OnOff::set(const String& keyword, const String& value) {
//  Serial.println(keyword+String(" :mod-set: ")+value);
  bool retval = false;
  if ( keyword_match(keyword) ) {
    if ( (value == "0") || (value == "aus") || (value == "Aus") || (value == "off") | (value == "Off") ) {
      do_switch(false);
      retval = true;
    }
    if ( (value == "1") || (value == "ein") || (value == "Ein") || (value == "on") | (value == "On") ) {
      do_switch(true);
      retval = true;
    }
    if ( (value == "2") || (value == "umschalten") || (value == "Umschalten") || (value == "toggle") | (value == "Toggle") ) {
      do_switch(! obj_value);
      retval = true;
    }
    if ( value.startsWith("S:") ) {
      obj_slider_val = value.substring(2,value.length()).toInt();
      do_switch(obj_value); 
      retval = true;
    }
  }
  return retval;
}

void Switch_OnOff::html_create(String& tmpstr) {
  tmpstr += String("\"") + obj_html_place + String("\":") + String(obj_value?"1":"0");
  tmpstr += String(",\"") + obj_html_place + String("_label\":\"") + obj_label + String("\"");
  tmpstr += String(",\"") + obj_html_place + String("_format\":\"x\"");
  if (obj_slider_used) {
    tmpstr += String(",\"slider") + String(obj_slider_no) + String("\":1");
    tmpstr += String(",\"slider") + String(obj_slider_no) + String("label\":\"") + obj_slider_label + String("\"");
    tmpstr += String(",\"slider") + String(obj_slider_no) + String("name\":\"") + obj_keyword + String("\"");
    tmpstr += String(",\"slider") + String(obj_slider_no) + String("val\":\"") + String(obj_slider_val) + String("\"");
    tmpstr += String(",\"slider") + String(obj_slider_no) + String("max\":\"") + String(obj_slider_max_val) + String("\"");
  }
}

void Switch_OnOff::set_hw_pin(uint8_t pin1) {
  obj_hw_pin1 = pin1;
  obj_hw_pin1_used = true;
}

void Switch_OnOff::set_hw_pin(uint8_t pin1, uint8_t pin2) {
  obj_hw_pin1 = pin1;
  obj_hw_pin1_used = true;
  obj_hw_pin2 = pin2;
  obj_hw_pin2_used = true;
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
}

void Switch_OnOff::set_slider_max_value(uint8_t val) {
  obj_slider_max_val = val;
}

#endif