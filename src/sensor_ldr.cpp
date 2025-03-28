#include "config.h"
#ifdef USE_SENSOR_LDR
#include "sensor_ldr.h"
#include "config.h"
#include "common.h"

void Sensor_LDR::begin(const char* html_place, const char* label, uint32_t messinterval) {
  Base_Generic::begin(html_place, label);
  obj_measure_interval = messinterval;
  obj_html_info = String("\"tab_head_ldr\":\"Sensor\",\"tab_line1_ldr\":\"LDR:#GPIO: A0\"") +
                  String(",\"GPIO_LDR\":\"A0\"");
  obj_mqtt_has_info = true; 
}

void Sensor_LDR::loop(time_t now) {
  if ((now - obj_measure_starttime) > obj_measure_interval) {
    obj_measure_starttime = now;
    obj_value = analogRead(A0);
    obj_html_stat = String("\"") + obj_html_place + String("\":\"") + obj_label + String(": ") +
                    String(obj_value) + String("\"");
    html_refresh();
  }
}

#endif
