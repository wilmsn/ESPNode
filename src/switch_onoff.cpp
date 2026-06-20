#include "switch_onoff.h"
#include "common.h"

Switch_OnOff::Switch_OnOff(){
  this->hw_pin1 = 0;
  this->hw_pin2 = 0;
  this->hw_pin1_used = false;
  this->hw_pin2_used = false;
  this->html_init_set = true;
  this->html_info_set = true;
}

void Switch_OnOff::begin(const char* _html_place, const char* _html_label,  const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, 
                         uint8_t _hw_pin_relais, bool _taster_ruhezustand, 
                         uint8_t _hw_pin2_taster, bool _show_diagramm) {
  this->hw_pin2 = _hw_pin2_taster;                    
  pinMode(this->hw_pin2, INPUT);
  this->hw_pin2_used = false;
  this->taster_ruhezustand = _taster_ruhezustand;
  this->taster_used = true;
  // Imitialisierung über  Fall 2
  begin(_html_place, _html_label, _keyword, _start_value, _on_value, _is_state, this->hw_pin1, _show_diagramm);
}


// Startet als Schalter mit Regler der einen HW-Pin mittels PWM steuert
// Fall 5
void Switch_OnOff::begin(const char* _html_place, const char* _html_label, const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, 
                         uint8_t _hw_pin1, uint8_t _slider_val, 
                         uint8_t _slider_max_val, uint8_t _slider_no, const char* _slider_html_label, 
                         const char* _slider_keyword, bool _show_diagramm) {
  this->hw_pin1 = _hw_pin1;
  pinMode(this->hw_pin1, OUTPUT);
  this->hw_pin1_used =true;
  // Imitialisierung über  Fall 4
  begin(_html_place, _html_label, _keyword, _start_value, _on_value, _is_state, _slider_val, _slider_max_val, _slider_no,
        _slider_html_label, _slider_keyword, _show_diagramm);
}

// Startet als Schalter mit Regler ohne HW Bezug
// Fall 4
void Switch_OnOff::begin(const char* _html_place, const char* _html_label, const char* _keyword, 
                         bool _start_value, bool _on_value, bool _is_state, 
                         uint8_t _slider_val, uint8_t _slider_max_val, uint8_t _slider_no,
                         const char* _slider_html_label, const char* _slider_keyword, bool _show_diagramm) {
  this->slider_used = true;
  this->slider_value = _slider_val;
  this->slider_no = _slider_no;
  this->slider_max_value = _slider_max_val;
  this->slider_html_label = _slider_html_label;
  this->slider_keyword = _slider_keyword;
  // Initialisierung über Fall 1
  begin(_html_place, _html_label, _keyword, _start_value, _on_value, _is_state, _show_diagramm);
}

// Startet als Schalter der zwei HW-Pins steuert
// Fall 3
void Switch_OnOff::begin(const char* _html_place, const char* _label, const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, 
                         uint8_t _hw_pin1, uint8_t _hw_pin2,
                         bool _show_diagramm) {
  this->hw_pin2 = _hw_pin2;
  this->hw_pin2_used = true;
  pinMode(this->hw_pin2, OUTPUT);
  // Initialisierung über Fall 2
  begin(_html_place, _label, _keyword, _start_value, _on_value, _is_state, _hw_pin1, _show_diagramm);
}

// Startet als Schalter der einen HW-Pin steuert
// Fall 2
void Switch_OnOff::begin(const char* _html_place, const char* _html_label, const char* _keyword,
                        bool _start_value, bool _on_value, bool _is_state, 
                        uint8_t _hw_pin1, bool _show_diagramm) {
  this->hw_pin1 = _hw_pin1;
  pinMode(this->hw_pin1, OUTPUT);
  this->hw_pin1_used = true;
  // Initialisierung über Fall 1
  begin(_html_place, _html_label, _keyword, _start_value, _on_value, _is_state, _show_diagramm);
}

// Startet als Schalter ohne HW-Pin
// Fall 1
void Switch_OnOff::begin(const char* _html_place, const char* _html_label, const char* _keyword,
                         bool _start_value, bool _on_value, bool _is_state, 
                         bool _show_diagramm) {
  Base_Generic::begin(_html_place, _html_label, _keyword);
  this->mqtt_stat_set = true;
  this->on_value = _on_value;
  this->switch_is_on = _start_value;
  this->is_state = _is_state;
  do_switch(switch_is_on);
  if ( _show_diagramm ) {
    this->diagramm_used = true;
    this->diagrammstore = malloc(24);
    memset(this->diagrammstore,0,24);
  }
  if (this->hw_pin1_used ) {
    this->mqtt_info_set = true;
  }
}

void Switch_OnOff::do_switch(bool new_state) {
  if ( this->switch_is_on != new_state ) {
    store_diagramm(new_state);
    if ( this->slider_used ) {
      if ( this->hw_pin1_used ) {
        analogWrite(this->hw_pin1, this->slider_value);
      }
    } else {
      if ( this->hw_pin1_used ) {
        digitalWrite(this->hw_pin1, this->on_value);
      }
      if (this->hw_pin2_used) {
        digitalWrite(this->hw_pin2, this->on_value);
      }
    }
  } else {
    if ( this->slider_used ) {
      if ( this->hw_pin1_used ) {
        analogWrite(this->hw_pin1, 0);
      }
    } else {
      if (this->hw_pin1_used) {
        digitalWrite(this->hw_pin1, ! this->on_value);
      }
      if (this->hw_pin2_used) {
        digitalWrite(this->hw_pin2, ! this->on_value);
      }
    }
  }
  this->switch_is_on = new_state;
  this->state = String(this->switch_is_on?"1":"0");
  this->html_update_set = true;

  this->mqtt_stat_str = String("\"") + this->keyword + String("\":") + String(this->switch_is_on?"1":"0");
  if (this->slider_used) {
    this->mqtt_stat_str += String(",\"") + this->slider_keyword + String("\":") + String(this->slider_value);
  }
  this->mqtt_stat_changed = true;
}

bool Switch_OnOff::set(const String& _cmnd, const String& _val) {
  bool retval = false;
  if ( keyword_match(_cmnd) ) {
// Ausschalten
    if ( (_val == "0") || (_val == String("aus")) || (_val == String("Aus")) || (_val == String("off")) | (_val == String("Off")) ) {
      do_switch(false);
      this->off_minute = 0;
      retval = true;
    }
// Einschalten
    if ( (_val == String("1")) || (_val == String("ein")) || (_val == String("Ein")) || (_val == String("on")) | (_val == String("On")) ) {
      do_switch(true);
      this->off_minute = 0;
      retval = true;
    }
// Umschalten
    if ( (_val == String("2")) || (_val == String("umschalten")) || (_val == String("Umschalten")) || (_val == String("toggle")) | (_val == String("Toggle")) ) {
      do_switch(! this->switch_is_on);
      this->off_minute = 0;
      retval = true;
    }
// Fuer 1 Stunde einschalten
    if ( (_val == String("1h")) ) {
      this->off_minute = minutes + 60 + SWITCHTIMERADD;
      this->timer_min = 60;
      do_switch(true);
      retval = true;
    }
// Fuer 2 Stunden einschalten
    if ( (_val == String("2h")) ) {
      this->off_minute = minutes + 120 + SWITCHTIMERADD;
      this->timer_min = 120;
      do_switch(true);
      retval = true;
    }
// Fuer 3 Stunden einschalten
    if ( (_val == String("3h")) ) {
      this->off_minute = minutes + 180 + SWITCHTIMERADD;
      this->timer_min = 180;
      do_switch(true);
      retval = true;
    }
// Fuer 4 Stunden einschalten
    if ( (_val == String("4h")) ) {
      this->off_minute = minutes + 240 + SWITCHTIMERADD;
      this->timer_min = 240;
      do_switch(true);
      retval = true;
    }
// Fuer 5 Stunden einschalten
    if ( (_val == String("5h")) ) {
      this->off_minute = minutes + 300 + SWITCHTIMERADD;
      this->timer_min = 300;
      do_switch(true);
      retval = true;
    }
// Fuer 6 Stunden einschalten
    if ( (_val == String("6h")) ) {
      this->off_minute = minutes + 360 + SWITCHTIMERADD;
      this->timer_min = 360;
      do_switch(true);
      retval = true;
    }
  } else {
    if (slider_used) {
      if ( (_cmnd == slider_keyword) ) {
        slider_value = _val.toInt();
        do_switch(switch_is_on); 
        retval = true;
      }
    }
    if ( _cmnd == String("?") || _cmnd == String("help")) {
      json_stat_header(stat_str);
      stat_str += String("\"Schalten:\"");
      json_stat_header(stat_str);
      stat_str += String("\"<") + this->keyword + String(">:<1|ein|0|aus|2|umschalten|<1..6>h>\"");
      if (slider_used) {
        json_stat_header(stat_str);
        stat_str += String("\"Dimmen:\"");
        json_stat_header(stat_str);
        stat_str += String("\"") + this->slider_keyword + String(":<0 ... 100>\"");
      }
      retval = false;
    }
  }
  return retval;
}

void Switch_OnOff::html_info(String& _html_info) {
  if (hw_pin1_used && hw_pin2_used ) {
    _html_info += String("\"tab_head_") + this->html_place + String("\":\"Switch On Off\"") +
                  String(",\"tab_line1_") + this->html_place + String() + String("\":\"") + 
                  this->html_label + String(" (") + this->keyword + String("):#GPIO ") + String(this->hw_pin1);
    if (slider_used) { 
      _html_info += String(" (PWM)\""); 
    } else { 
      _html_info += String("\""); 
    }
    _html_info += String(",\"tab_line2_") + this->html_place + String("\":\"") + this->html_label + String(":#GPIO ") + String(this->hw_pin2);
    if (slider_used) { 
      _html_info += String(" (PWM)\""); 
    } else {
      _html_info += String("\""); 
    }
  } else {
    if (hw_pin1_used ) {
      _html_info += String("\"tab_head_") + this->html_place + String("\":\"Switch On Off\"") +
                    String(",\"tab_line1_") + this->html_place + String("\":\"") + 
                    this->html_label + String(" (") + this->keyword + String("):#GPIO ") + String(this->hw_pin1);
      if (slider_used) { 
        _html_info += String(" (PWM)\""); 
      } else { 
        _html_info += String("\""); 
      }
    }
  }
}

bool Switch_OnOff::get_switch_val() {
  return switch_is_on;
}

uint8_t Switch_OnOff::get_slider_val() {
  return slider_value;
}

void Switch_OnOff::set_switch(uint8_t val) {
  set(keyword, String(val));
}

void Switch_OnOff::html_init(String& _html_init) {
  _html_init += String("\"") + this->html_place + String("_label\":\"") + this->html_label + String("\"") +
                String(",\"") + this->html_place + String("_format\":\"x\"");
  if (slider_used) {
    _html_init += String(",\"slider") + String(this->slider_no) + String("\":1") +
                  String(",\"slider") + String(this->slider_no) + String("label\":\"") + this->slider_html_label + String("\"") +
                  String(",\"slider") + String(this->slider_no) + String("name\":\"") + this->slider_keyword + String("\"") +
                  String(",\"slider") + String(this->slider_no) + String("max\":\"") + String(this->slider_max_value) + String("\"");
  }
  _html_init += String(",");
  html_update(_html_init);
  diagramm2web(_html_init );
}

void Switch_OnOff::html_update(String& _html_update) {
  _html_update += String("\"") + this->html_place + String("\":") + String(this->switch_is_on?"1":"0");
  if (slider_used) {
    _html_update += String(",\"slider") + String(this->slider_no) + String("val\":\"") + String(this->slider_value) + String("\"");
  }
  if (timer_min > 0) {
    if ( this->switch_is_on ) {
      _html_update += String(",\"") + this->html_place + String("_progress\":\"") + String(this->timer_progress()) + String("\"");
      _html_update += String(",\"") + this->html_place + String("_timer\":\"") + String(this->timer_min/60) + String("\"");
    } else {
      _html_update += String(",\"") + this->html_place + String("_progress\":\"0\"");
      this->timer_min = 0;
    }
  }
}

void Switch_OnOff::mqtt_stat(String& _mqtt_stat) {
  _mqtt_stat += String("\"") + this->keyword + String("\":") + String(this->switch_is_on?"1":"0");
}

void Switch_OnOff::mqtt_info(String& _mqtt_info) {
  if (hw_pin1_used && hw_pin2_used ) {

    _mqtt_info = String("\"GPIO_") + this->keyword + String("\":\"") + String(this->hw_pin1) + String("; ") + String(this->hw_pin2);
    if (slider_used) { _mqtt_info += String(" (PWM)\""); } else { _mqtt_info += String("\""); }
    mqtt_info_set = true;

  } else {
    if (hw_pin1_used ) {

      _mqtt_info = String("\"GPIO_") + keyword + String("\":\"") + String(hw_pin1);
      if (slider_used) { _mqtt_info += String(" (PWM)\""); } else { _mqtt_info += String("\""); }
      mqtt_info_set = true;

    }
  }
}

bool Switch_OnOff::do_dia_store(int min) {
  switch (min)  {
    case 0:
    case 8:
    case 15:
    case 23:
    case 30:
    case 38:
    case 45:
    case 53:
      return true;
    break;
    default:
      return false;
  }
}

void Switch_OnOff::store_diagramm(bool invalue) {
  bool ovfl1 = false;
  bool ovfl2 = invalue;
  if (diagramm_used) {
    if (do_dia_store(timeinfo.tm_min)) {
      for (int i=0; i<24; i++) {
        ovfl1 = ((uint8_t *)diagrammstore)[i] & 0b10000000;
        ((uint8_t *)diagrammstore)[i] = (((uint8_t *)diagrammstore)[i] << 1) | ovfl2;
        ovfl2 = ovfl1;
      }
    } else {
      ((uint8_t *)diagrammstore)[0] |= invalue;
    }
  }
}

void Switch_OnOff::diagramm2web(String& myjson) {
  if (diagramm_used) {
    if (myjson.length() > 5) myjson += String(",");
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
  }
}

void Switch_OnOff::loop(time_t now) {
  if (taster_used) {
    if ((digitalRead(hw_pin2) != taster_ruhezustand ) && (now - taster_pressed_time > 30)) {
      do_switch(! switch_is_on);
      taster_pressed_time = now;
    }
  }
  if (timeinfo.tm_min != old_min) {
    String tmpjson = String("{");
    if (off_minute > 0 && off_minute <= minutes && timer_min > 0) {
      do_switch(false);
      off_minute = 0;
      timer_min = 0;
    }
    if (timer_min > 0) {
      tmpjson += String("\"") + html_place + String("_progress\":\"") + String(timer_progress()) + String("\"");
    }
    if (diagramm_used) {
      if (do_dia_store(timeinfo.tm_min)) {
        store_diagramm(switch_is_on);
        diagramm2web(tmpjson);
      }
    }
    if ((timer_min > 0 || diagramm_used) && tmpjson.length() > 5) {
        tmpjson += String("}");
        sendWsMessage(tmpjson);
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

