#include "config.h"
#ifdef USE_SENSOR_LDR
#include "sensor_ldr.h"
#include "config.h"
#include "common.h"

void Sensor_LDR::begin(const char* html_place, const char* label, uint32_t messinterval) {
  Base_Generic::begin(html_place, label);
  measure_interval = messinterval;
  html_info = String("\"tab_head_ldr\":\"Sensor\",\"tab_line1_ldr\":\"LDR:#GPIO: A0\"") +
              String(",\"GPIO_LDR\":\"A0\"");
  html_has_info = true;
  fill_html_stat();
  html_has_stat = true;
  mqtt_has_stat = true; 
  value = analogRead(A0);
  fill_mqtt_stat();
}

void Sensor_LDR::fill_mqtt_stat() {
  mqtt_stat = String("\"LDR\":") + String(value);
}

void Sensor_LDR::fill_html_stat() {
  html_stat = String("\"") + html_place + String("\":\"") + label + String(": ") + String(value) + String("\"");
}

void Sensor_LDR::loop(time_t now) {
  if ((now - measure_starttime) > measure_interval) {
    measure_starttime = now;
    value = analogRead(A0);
    fill_mqtt_stat();
    fill_html_stat();
    html_update();
  }
}

#endif
