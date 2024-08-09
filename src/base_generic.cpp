#include "base_generic.h"
#include "common.h"

Base_Generic::Base_Generic(){
  obj_keyword = "";
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

void Base_Generic::loop(time_t now) {
}

bool Base_Generic::set(const String& keyword, const String& value) {
  return false;
}

bool Base_Generic::keyword_match(const String& keyword) {
  bool retval = false;
  if ( keyword == obj_keyword || keyword == obj_html_place || keyword == obj_mqtt_name  || keyword == obj_label ) {
    retval = true;
  }
  return retval;
}

void Base_Generic::html_create(String& tmpstr) {
  tmpstr += obj_html_stat;
}

String& Base_Generic::html_info() {
  return obj_html_info;
}

String& Base_Generic::mqtt_name() {
  return obj_mqtt_name;
}

String& Base_Generic::mqtt_stat() {
  obj_mqtt_stat_changed = false;
  return obj_mqtt_stat;
}

bool Base_Generic::mqtt_has_stat() {
  return obj_mqtt_has_stat;
}

bool Base_Generic::mqtt_stat_changed() {
  return obj_mqtt_stat_changed;
}

bool Base_Generic::mqtt_has_info() {
  return obj_mqtt_has_info;
}

String& Base_Generic::mqtt_info() {
  return obj_mqtt_info;
}
