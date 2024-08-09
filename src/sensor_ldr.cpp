#include "config.h"
#ifdef USE_SENSOR_LDR
#include "sensor_ldr.h"
#include "config.h"
#include "common.h"

void Sensor_LDR::begin(const char* html_place, const char* label, uint32_t messinterval) {
  Base_Generic::begin(html_place, label);
  obj_measure_interval = messinterval;
  obj_html_info = String(",\"tab_head_ldr\":\"Sensor\",\"tab_line1_ldr\":\"LDR:#GPIO: A0\"");
}

void Sensor_LDR::loop(time_t now) {
  if ((now - obj_measure_starttime) > obj_measure_interval) {
    obj_measure_starttime = now;
    obj_value = analogRead(A0);
    obj_html_stat = "\"";
    obj_html_stat += obj_html_place;
    obj_html_stat += "\":\"";
    obj_html_stat += obj_label;
    obj_html_stat += ": ";
    obj_html_stat += String(obj_value);
    obj_html_stat += "\"";
//    write2log(LOG_MODULE,1,obj_html_stat.c_str());
//    ws.textAll(obj_html_stat.c_str());

                    
//    obj_mqtt_info = "\"";
//    obj_mqtt_info += obj_mqtt_name;
//    obj_mqtt_info += "\": ";
//    obj_mqtt_info += String(obj_value);

  }
}

#endif
