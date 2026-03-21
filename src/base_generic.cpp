#include "base_generic.h"
#include "common.h"

Base_Generic::Base_Generic(){
  keyword = "";
  html_info_set = false;
}

void Base_Generic::begin(const char* _html_place, const char* _label) {
  keyword = _label;
  html_place = _html_place;
  label = _label;
  mqtt_name = _label;
}

void Base_Generic::begin(const char* _html_place, const char* _label, const char* _mqtt_name) {
  keyword = _label;
  html_place = _html_place;
  label = _label;
  mqtt_name = _mqtt_name;
}

void Base_Generic::begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword) {
  keyword = _keyword;
  html_place = _html_place;
  label = _label;
  mqtt_name = _mqtt_name;
}

void Base_Generic::loop(time_t now) {
}

bool Base_Generic::set(const String& _cmnd, const String& _val) {
  return false;
}

bool Base_Generic::keyword_match(const String& _keyword) {
  bool retval = false;
  if ( _keyword == keyword || _keyword == html_place || _keyword == mqtt_name  || _keyword == label ) {
    retval = true;
  }
  return retval;
}

void Base_Generic::html_init(String& _html_init) {
}

void Base_Generic::html_update(String& _html_update) {
}

void Base_Generic::html_info(String& _html_info) {
}

void Base_Generic::mqtt_stat(String& _mqtt_stat) {
}
