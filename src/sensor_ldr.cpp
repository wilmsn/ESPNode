#include "config.h"
#ifdef USE_SENSOR_LDR
#include "sensor_ldr.h"
#include "config.h"
#include "common.h"

void Sensor_LDR::begin(const char* _html_place, const char* _html_label, const char* _keyword) {
  Base_Generic::begin(_html_place, _html_label, _keyword);
  html_init_set = true;
  mqtt_stat_set = true;
}

void Sensor_LDR::loop(time_t now) {
  if ((now - measure_starttime) > REFRESHTIME) {
    measure_starttime = now;
    value = analogRead(A0);
    html_update_set = true;
  }
}

void Sensor_LDR::html_init(String& _html_init) {
  _html_init += String("\"") + html_place + String("\":\"") + html_label + String(": ") + String(value) + String("\"");
}

void Sensor_LDR::html_update(String& _html_update) {
  _html_update += String("\"") + html_place + String("\":\"") + html_label + String(": ") + String(value) + String("\"");
}

void Sensor_LDR::html_info(String& _html_info) {
  _html_info += String("\"tab_head_ldr\":\"Sensor\",\"tab_line1_ldr\":\"LDR:#GPIO: A0\"");
}

void Sensor_LDR::mqtt_stat(String& _mqtt_stat) {
  _mqtt_stat += String("\"LDR\":") + String(value);
}

#endif
