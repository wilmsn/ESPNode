#include "config.h"
#ifdef USE_SENSOR_LDR
#include "sensor_ldr.h"
#include "config.h"

void Sensor_LDR::begin(const char* html_place, const char* label) {
  Base_Generic::begin(html_place, label);
}

void Sensor_LDR::html_create_json_part(String& json) {
  json += ",\"";
  json += "\"sensorinfo1\":\"Hardware:#LDR an A0\"";
}

void Sensor_LDR::start_measure(time_t now) {
  obj_measure_starttime = now;
  obj_measure_started = true;
}

void Sensor_LDR::loop(time_t now) {
  if (obj_measure_started) {
    if ((now - obj_measure_starttime) > obj_measure_delay) {
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

      set_changed(true);
      obj_measure_started = false;
    }
  } else {
    if ((now - obj_measure_starttime) > obj_measure_interval) {
      start_measure(now);
    }
  }

}

#endif
