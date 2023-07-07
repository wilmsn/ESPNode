#include "sensor_ldr.h"

void Sensor_LDR::begin(const char* html_place, const char* label) {
  Sensor_Generic::begin(html_place, label);
  obj_value = analogRead(A0);
  obj_sensorinfo = "\"sensorinfo1\":\"Hardware:#LDR an A0\",\"sensorinfo2\":\"Präzision:#12\",\"sensorinfo3\":\"Empfindlichkeit:#100lx\",\"sensorinfo4\":\"sonst:#test\"";
}

void Sensor_LDR::start_measure() {
  obj_value = analogRead(A0);
  obj_html_stat_json = "{\"";
  obj_html_stat_json += obj_html_place;
  obj_html_stat_json += "\":\"";
  obj_html_stat_json += obj_label;
  obj_html_stat_json += ": ";
  obj_html_stat_json += String(obj_value);
  obj_html_stat_json += "\"}";

  obj_mqtt_json = "\"";
  obj_mqtt_json += obj_mqtt_name;
  obj_mqtt_json += "\": ";
  obj_mqtt_json += String(obj_value);

  obj_changed = true;
}
