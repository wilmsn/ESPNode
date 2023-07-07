#include "sensor_generic.h"

Sensor_Generic::Sensor_Generic(){
  obj_keyword = "";
  obj_hw_pin1 = 0;
  obj_hw_pin2 = 0;
}

void Sensor_Generic::begin(const char* html_place, const char* label) {
  obj_keyword = label;
  obj_html_place = html_place;
  obj_label = label;
  obj_mqtt_name = label;
}

void Sensor_Generic::begin(const char* html_place, const char* label, const char* mqtt_name) {
  obj_keyword = label;
  obj_html_place = html_place;
  obj_label = label;
  obj_mqtt_name = mqtt_name;
}

void Sensor_Generic::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword) {
  obj_keyword = keyword;
  obj_html_place = html_place;
  obj_label = label;
  obj_mqtt_name = mqtt_name;
}

void Sensor_Generic::set_label(const char* label) {
  obj_label = label;
}

void Sensor_Generic::set_keyword(const char* keyword) {
  obj_keyword = keyword;
}

void Sensor_Generic::set_mqtt_name(const char* mqtt_name) {
  obj_mqtt_name = mqtt_name;
}

void Sensor_Generic::set_hw_pin(uint8_t pin1) {
  obj_hw_pin1 = pin1;
  obj_hw_pin1_used = true;
}

void Sensor_Generic::set_hw_pin(uint8_t pin1, uint8_t pin2) {
  obj_hw_pin1 = pin1;
  obj_hw_pin1_used = true;
  obj_hw_pin2 = pin2;
  obj_hw_pin2_used = true;
}

bool Sensor_Generic::keyword_match(const String& keyword) {
  bool retval = false;
  if ( keyword == obj_keyword || keyword == obj_html_place || keyword == obj_mqtt_name ) {
    retval = true;
  }
  return retval;
}

const char* Sensor_Generic::mqtt_json_part(void) {
  return obj_mqtt_json.c_str();
}

const char* Sensor_Generic::html_stat_json(void) {
  obj_changed = false;
  return obj_html_stat_json.c_str();
}

bool Sensor_Generic::changed() {
  return obj_changed;
}

void Sensor_Generic::loop() {
}

String& Sensor_Generic::sensorinfo() {
  return obj_sensorinfo;
}

void Sensor_Generic::html_create_json_part(String& json) {
  json += " ";
}

String& Sensor_Generic::show_keyword() {
  return obj_keyword;
}

String& Sensor_Generic::show_label() {
  return obj_label;
}

const char* Sensor_Generic::show_mqtt_name() {
  return obj_mqtt_name.c_str();
}

//tbd überprüfen !!! Wird es benötigt???
const char* Sensor_Generic::show_value() {
  return obj_values_str.c_str();
}
