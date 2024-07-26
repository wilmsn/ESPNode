#include "base_generic.h"

Base_Generic::Base_Generic(){
  obj_keyword = "";
  obj_hw_pin1 = 0;
  obj_hw_pin2 = 0;
  obj_hw_pin1_used = false;
  obj_hw_pin2_used = false;
}

void Base_Generic::begin(const char* html_place, const char* label) {
  obj_keyword = label;
  obj_html_place = html_place;
  obj_label = label;
  obj_mqtt_name = label;
}

void Base_Generic::begin(const char* html_place, const char* label, const char* mqtt_name) {
  obj_keyword = label;
  obj_html_place = html_place;
  obj_label = label;
  obj_mqtt_name = mqtt_name;
}

void Base_Generic::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword) {
  obj_keyword = keyword;
  obj_html_place = html_place;
  obj_label = label;
  obj_mqtt_name = mqtt_name;
}

void Base_Generic::set_label(const char* label) {
  obj_label = label;
}

void Base_Generic::set_keyword(const char* keyword) {
  obj_keyword = keyword;
}

void Base_Generic::set_mqtt_name(const char* mqtt_name) {
  obj_mqtt_name = mqtt_name;
}

void Base_Generic::set_hw_pin(uint8_t pin1) {
  obj_hw_pin1 = pin1;
  obj_hw_pin1_used = true;
}

void Base_Generic::set_hw_pin(uint8_t pin1, uint8_t pin2) {
  obj_hw_pin1 = pin1;
  obj_hw_pin1_used = true;
  obj_hw_pin2 = pin2;
  obj_hw_pin2_used = true;
}

bool Base_Generic::keyword_match(const String& keyword) {
  bool retval = false;
  if ( keyword == obj_keyword || keyword == obj_html_place || keyword == obj_mqtt_name ) {
    retval = true;
  }
  return retval;
}

String& Base_Generic::mqtt_json_part(void) {
  return obj_mqtt_json;
}

String& Base_Generic::html_stat_json(void) {
  obj_changed_w = false;
  return obj_html_stat_json;
}

bool Base_Generic::changed() {
//  if ( obj_changed_a ) Serial.println("Base_Generic::changed => true");
  bool retval = obj_changed_a;
  obj_changed_a = false;
  return retval;
}

bool Base_Generic::webChange() {
//  if ( obj_changed_w ) Serial.println("Base_Generic::webChange => true");
  bool retval = obj_changed_w;
  obj_changed_w = false;
  return retval;
}

bool Base_Generic::set(const String& keyword, const String& value) {
  return false;
}

void Base_Generic::set_changed(bool val) {
  obj_changed_w = val;
  obj_changed_a = val;
}

void Base_Generic::loop(time_t now) {
}

void Base_Generic::html_create_json_part(String& json) {
  json += " ";
}

String& Base_Generic::show_keyword() {
  return obj_keyword;
}

String& Base_Generic::show_label() {
  return obj_label;
}

String& Base_Generic::show_mqtt_name() {
  return obj_mqtt_name;
}

String& Base_Generic::show_value() {
  return obj_values_str;
}
/*
String& Base_Generic::info_html() {
  return obj_info_html;
}
*/
String& Base_Generic::info_mqtt() {
  return obj_info_mqtt;
}
