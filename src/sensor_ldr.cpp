#include "sensor_ldr.h"
#include "common.h"

void Sensor_LDR::begin(const char* _html_place, const char* _html_label, const char* _keyword) {
  Base_Generic::begin(_html_place, _html_label, _keyword);
  this->html_init_set = true;
  this->mqtt_stat_set = true;
}

void Sensor_LDR::loop(time_t now) {
  if ((now - this->measure_starttime) > REFRESHTIME) {
    this->measure_starttime = now;
    this->value = analogRead(A0);
    this->html_update_set = true;
  }
}

void Sensor_LDR::html_init(String& _html_init) {
  _html_init += String("\"") + this->html_place + String("\":\"") + this->html_label + String(": ") + String(this->value) + String("\"");
}

void Sensor_LDR::html_update(String& _html_update) {
  _html_update += String("\"") + this->html_place + String("\":\"") + this->html_label + String(": ") + String(this->value) + String("\"");
}

void Sensor_LDR::html_info(String& _html_info) {
  _html_info += String("\"tab_head_ldr\":\"Sensor\",\"tab_line1_ldr\":\"LDR:#GPIO: A0\"");
}

void Sensor_LDR::mqtt_stat(String& _mqtt_stat) {
  _mqtt_stat += String("\"LDR\":") + String(this->value);
}
