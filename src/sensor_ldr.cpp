#include "config.h"
#ifdef USE_SENSOR_LDR
#include "sensor_ldr.h"
#include "config.h"
#include "common.h"

void Sensor_LDR::begin(const char* _html_place, const char* _label) {
  Base_Generic::begin(_html_place, _label);
  html_info = String("\"tab_head_ldr\":\"Sensor\",\"tab_line1_ldr\":\"LDR:#GPIO: A0\"");
  html_has_info = true;
  mqtt_has_stat = true;
}

void Sensor_LDR::loop(time_t now) {
  if ((now - measure_starttime) > REFRESHTIME) {
    measure_starttime = now;
    value = analogRead(A0);
    mqtt_stat = String("\"LDR\":") + String(value);
    html_json = String("\"") + html_place + String("\":\"") + label + String(": ") + String(value) + String("\"");
    html_json_filled = true;
    html_update();
  }
}

void Sensor_LDR::html_init() {
  html_json = String("\"") + html_place + String("\":\"") + label + String(": ") + String(value) + String("\"");
  html_json_filled = true;
}

#endif
