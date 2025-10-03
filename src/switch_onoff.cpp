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
                         bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin1, uint8_t _slider_val, 
                         uint8_t _slider_max_val, uint8_t _slider_no, const char* _slider_label, 
                         const char* _slider_mqtt_name, const char* _slider_keyword, bool _show_diagramm) {
  hw_pin1 = _hw_pin1;                    
  pinMode(hw_pin1, OUTPUT);
  hw_pin1_used =true;
  // Imitialisierung über  Fall 4
  begin(_html_place, _label, _mqtt_name, _keyword, _start_value, _on_value, _is_state, _slider_val, _slider_max_val, _slider_no,
        _slider_label, _slider_mqtt_name, _slider_keyword, _show_diagramm);
}

// Startet als Schalter mit Regler ohne HW Bezug
// Fall 4
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _mqtt_name,  const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, uint8_t _slider_val, uint8_t _slider_max_val, uint8_t _slider_no,
                         const char* _slider_label, const char* _slider_mqtt_name, const char* _slider_keyword, bool _show_diagramm) {
  slider_used = true;
  slider_value = _slider_val;
  slider_no = _slider_no;
  slider_max_value = _slider_max_val;
  slider_label = _slider_label;
  slider_mqtt_name = _slider_mqtt_name;
  slider_keyword = _slider_keyword;
  // Initialisierung über Fall 1
  begin(_html_place, _label, _mqtt_name, _keyword, _start_value, _on_value, _is_state, _show_diagramm);
}

// Startet als Schalter der zwei HW-Pins steuert
// Fall 3
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _mqtt_name,  const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin1, uint8_t _hw_pin2,
                         bool _show_diagramm) {
  hw_pin2 = _hw_pin2;
  hw_pin2_used = true;
  pinMode(hw_pin2, OUTPUT);
  // Initialisierung über Fall 2
  begin(_html_place, _label, _mqtt_name, _keyword, _start_value, _on_value, _is_state, _hw_pin1, _show_diagramm);
}

// Startet als Schalter der einen HW-Pin steuert
// Fall 2
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _mqtt_name,  const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin1, bool _show_diagramm) {
  hw_pin1 = _hw_pin1;
  pinMode(hw_pin1, OUTPUT);
  hw_pin1_used = true;
  // Initialisierung über Fall 1
  begin(_html_place, _label, _mqtt_name, _keyword, _start_value, _on_value, _is_state, _show_diagramm);
}

// Startet als Schalter ohne HW-Pin
// Fall 1
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, bool _show_diagramm) {
  Base_Generic::begin(_html_place, _label, _mqtt_name, _keyword);
  switch_mqtt_name = _mqtt_name;
  mqtt_has_stat = true;
  on_value = _on_value;
  switch_value = _start_value;
  is_state = _is_state;
  if ( _show_diagramm ) {
    diagramm_used = true;
    diagrammstore = malloc(24);
    memset(diagrammstore,0,24);
  }
  do_switch(switch_value);
  if (hw_pin1_used && hw_pin2_used ) {

    html_info = String("\"tab_head_") + html_place+String("\":\"Switch On Off\"") +
                String(",\"tab_line1_") + html_place+String("\":\"") + label + String(":#GPIO:") + String(hw_pin1);
    if (slider_used) { html_info += String(" (PWM)\""); } else { html_info += String("\""); }
    html_info = String(",\"tab_line2_") + html_place+String("\":\"") + label + String(":#GPIO:") + String(hw_pin2);
    if (slider_used) { html_info += String(" (PWM)\""); } else { html_info += String("\""); }
    html_has_info = true;

    mqtt_info = String("\"GPIO_") + mqtt_name + String("\":\"") + String(hw_pin1) + String("; ") + String(hw_pin2);
    if (slider_used) { mqtt_info += String(" (PWM)\""); } else { mqtt_info += String("\""); }
    mqtt_has_info = true;

  } else {
    if (hw_pin1_used ) {

      html_info = String("\"tab_head_") + html_place + String("\":\"Switch On Off\"") +
                  String(",\"tab_line1_") + html_place + String("\":\"") + label + String(":#GPIO:") + String(hw_pin1);
      if (slider_used) { html_info += String(" (PWM)\""); } else { html_info += String("\""); }
      html_has_info = true;

      mqtt_info = String("\"GPIO_") + mqtt_name + String("\":\"") + String(hw_pin1);
      if (slider_used) { mqtt_info += String(" (PWM)\""); } else { mqtt_info += String("\""); }
      mqtt_has_info = true;

    }
  }
}

void Switch_OnOff::do_switch(bool new_state) {
  if ( new_state ) {
    store_diagramm(new_state);
    if ( slider_used ) {
      if ( hw_pin1_used ) {
        analogWrite(hw_pin1, slider_value);
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
  switch_value = new_state;
  state = String(switch_value?"1":"0");
  
  String myjson = String("{\"") + html_place + String("\":") + String(switch_value?"1":"0");
  if (slider_used) {
    myjson += String(",\"slider") + String(slider_no) + String("val\":\"") + String(slider_value) + String("\"");
  }
  if (timer_min > 0) {
    if ( new_state ) {
      myjson += String(",\"") + html_place + String("_progress\":\"") + String(timer_progress()) + String("\"");
    } else {
      myjson += String(",\"") + html_place + String("_progress\":\"0\"");
      timer_min = 0;
    }
  }
  myjson += String("}");
  sendWsMessage(myjson);
  
  mqtt_stat = String("\"") + switch_mqtt_name+String("\":") + String(switch_value? "1":"0");
  if (slider_used) {
    mqtt_stat += String(",\"") + slider_mqtt_name + String("\":") + String(slider_value);
  }
  mqtt_stat_changed = true;
}

bool Switch_OnOff::set(const String& _cmnd, const String& _val) {
  bool retval = false;
  if ( keyword_match(_cmnd) || _cmnd == mqtt_name ) {
// Ausschalten
    if ( (_val == "0") || (_val == String("aus")) || (_val == String("Aus")) || (_val == String("off")) | (_val == String("Off")) ) {
      do_switch(false);
      off_minute = 0;
      retval = true;
    }
// Einschalten
    if ( (_val == String("1")) || (_val == String("ein")) || (_val == String("Ein")) || (_val == String("on")) | (_val == String("On")) ) {
      do_switch(true);
      off_minute = 0;
      retval = true;
    }
// Umschalten
    if ( (_val == String("2")) || (_val == String("umschalten")) || (_val == String("Umschalten")) || (_val == String("toggle")) | (_val == String("Toggle")) ) {
      do_switch(! switch_value);
      off_minute = 0;
      retval = true;
    }
// Fuer 1 Stunde einschalten
    if ( (_val == String("1h")) ) {
      off_minute = minutes + 60 + SWITCHTIMERADD;
      timer_min = 60;
      do_switch(true);
      retval = true;
    }
// Fuer 2 Stunden einschalten
    if ( (_val == String("2h")) ) {
      off_minute = minutes + 120 + SWITCHTIMERADD;
      timer_min = 120;
      do_switch(true);
      retval = true;
    }
// Fuer 3 Stunden einschalten
    if ( (_val == String("3h")) ) {
      off_minute = minutes + 180 + SWITCHTIMERADD;
      timer_min = 180;
      do_switch(true);
      retval = true;
    }
// Fuer 4 Stunden einschalten
    if ( (_val == String("4h")) ) {
      off_minute = minutes + 240 + SWITCHTIMERADD;
      timer_min = 240;
      do_switch(true);
      retval = true;
    }
// Fuer 5 Stunden einschalten
    if ( (_val == String("5h")) ) {
      off_minute = minutes + 300 + SWITCHTIMERADD;
      timer_min = 300;
      do_switch(true);
      retval = true;
    }
// Fuer 6 Stunden einschalten
    if ( (_val == String("6h")) ) {
      off_minute = minutes + 360 + SWITCHTIMERADD;
      timer_min = 360;
      do_switch(true);
      retval = true;
    }
  } else {
    if (slider_used) {
      if ( (_cmnd == switch_mqtt_name) || (_cmnd == slider_keyword) ) {
        slider_value = _val.toInt();
        do_switch(switch_value); 
        retval = true;
      }
    }
    if ( _cmnd == String("?") || _cmnd == String("help")) {
      json_stat_header(stat_str);
      stat_str += String("\"Schalten:\"");
      json_stat_header(stat_str);
      stat_str += String("\"<") + html_place + String("|") + keyword + String(">:<1|ein|0|aus|2|umschalten|<1..6>h>\"");
      if (slider_used) {
        json_stat_header(stat_str);
        stat_str += String("\"Dimmen:\"");
        json_stat_header(stat_str);
        stat_str += String("\"") + slider_keyword + String(":<0 ... 100>\"");
      }
      retval = false;
    }
  }
  return retval;
}

uint8_t Switch_OnOff::get_slider_val() {
  return slider_value;
}

bool Switch_OnOff::get_switch_val() {
  return switch_value;
}

void Switch_OnOff::set_switch(uint8_t val) {
  set(keyword, String(val));
}

void Switch_OnOff::set_slider_label(const char* _label) {
  slider_label = _label;
}

void Switch_OnOff::set_slider(uint8_t _val) {
  slider_value = _val;
  do_switch(switch_value);
}

void Switch_OnOff::set_slider_max_value(uint8_t _val) {
  slider_max_value = _val;
}

void Switch_OnOff::html_init() {
  html_json = String("\"") + html_place + String("_label\":\"") + label + String("\"") +
              String(",\"") + html_place + String("_format\":\"x\"");
  if (slider_used) {
    html_json += String(",\"slider") + String(slider_no) + String("\":1") +
                 String(",\"slider") + String(slider_no) + String("label\":\"") + slider_label + String("\"") +
                 String(",\"slider") + String(slider_no) + String("name\":\"") + slider_keyword + String("\"") +
                 String(",\"slider") + String(slider_no) + String("max\":\"") + String(slider_max_value) + String("\"");
  }
  html_json += String(",\"") + html_place + String("\":") + String(switch_value?"1":"0");
  if (slider_used) {
    html_json += String(",\"slider") + String(slider_no) + String("val\":\"") + String(slider_value) + String("\"");
  }
  diagramm2web(html_json);
  html_json_filled = true;
}

void Switch_OnOff::store_diagramm(bool invalue) {
  if (diagramm_used) {
    uint8_t myhour =  ((uint8_t *)diagrammstore)[timeinfo.tm_hour];
    if (timeinfo.tm_min >= 0 && timeinfo.tm_min <= 7) {
      if (invalue) {
        myhour = myhour | 0b00000001;  
      } else {
        myhour = myhour & 0b11111110;
      }
    }
    if (timeinfo.tm_min >= 8 && timeinfo.tm_min <= 14) {
      if (invalue) {
        myhour = myhour | 0b00000010;  
      } else {
        myhour = myhour & 0b11111101;
      }
    }
    if (timeinfo.tm_min >= 15 && timeinfo.tm_min <= 22) {
      if (invalue) {
        myhour = myhour | 0b00000100;  
      } else {
        myhour = myhour & 0b11111011;
      }
    }
    if (timeinfo.tm_min >= 23 && timeinfo.tm_min <= 29) {
      if (invalue) {
        myhour = myhour | 0b00001000;  
      } else {
        myhour = myhour & 0b11110111;
      }
    }
    if (timeinfo.tm_min >= 30 && timeinfo.tm_min <= 37) {
      if (invalue) {
        myhour = myhour | 0b00010000;  
      } else {
        myhour = myhour & 0b11101111;
      }
    }
    if (timeinfo.tm_min >= 38 && timeinfo.tm_min <= 44) {
      if (invalue) {
        myhour = myhour | 0b00100000;  
      } else {
        myhour = myhour & 0b11011111;
      }
    }
    if (timeinfo.tm_min >= 45 && timeinfo.tm_min <= 52) {
      if (invalue) {
        myhour = myhour | 0b01000000;  
      } else {
        myhour = myhour & 0b10111111;
      }
    }
    if (timeinfo.tm_min >= 53 && timeinfo.tm_min <= 59) {
      if (invalue) {
        myhour = myhour | 0b10000000;  
      } else {
        myhour = myhour & 0b01111111;
      }
    }
    ((uint8_t *)diagrammstore)[timeinfo.tm_hour] = myhour;
  }
}

void Switch_OnOff::diagramm2web(String& myjson) {
  if (diagramm_used) {
    if (myjson.length() > 3) myjson += String(",");
    myjson +=  String("\"") + html_place + String("_dia\":\"");
    for(int n=0; n<24; n++) {
      uint8_t myhour =  ((uint8_t *)diagrammstore)[n];
      if (myhour & 0b00000001) myjson += String("1"); else myjson += String("0"); 
      if (myhour & 0b00000010) myjson += String("1"); else myjson += String("0"); 
      if (myhour & 0b00000100) myjson += String("1"); else myjson += String("0"); 
      if (myhour & 0b00001000) myjson += String("1"); else myjson += String("0"); 
      if (myhour & 0b00010000) myjson += String("1"); else myjson += String("0"); 
      if (myhour & 0b00100000) myjson += String("1"); else myjson += String("0"); 
      if (myhour & 0b01000000) myjson += String("1"); else myjson += String("0"); 
      if (myhour & 0b10000000) myjson += String("1"); else myjson += String("0"); 
    }
    myjson += String("\"");
    if (timer_min > 0) {
      myjson += String(",\"") + html_place + String("_progress\":\"") + String(timer_progress()) + String("\"");
    }
  }
}

void Switch_OnOff::loop(time_t now) {
  if (timeinfo.tm_min != old_min) {
    if (off_minute > 0 && off_minute <= minutes && timer_min > 0) {
      do_switch(false);
      off_minute = 0;
      timer_min = 0;
    }
    if (diagramm_used) {
      switch (timeinfo.tm_min)  {
        case 0:
        case 8:
        case 15:
        case 23:
        case 30:
        case 38:
        case 45:
        case 53:
          store_diagramm(switch_value);
        break;
      }
      if (timer_min > 0) {
        String myjson = String("{");
        diagramm2web(myjson);
        myjson += String("}");
        sendWsMessage(myjson);
      }
    }
    old_min = timeinfo.tm_min;
  }
}

uint8_t Switch_OnOff::timer_progress() {
  uint8_t retval = 0;
  if (timer_min > 0) {
    uint16_t progress = off_minute - minutes;
    progress = (100 * progress) / timer_min;
    retval = progress;
  }
  return retval;
}

#endif