#include "config.h"
#ifdef USE_SENSOR_18B20
#include "sensor_18B20.h"
#include "config.h"
#define PIN_18B20      4

const int oneWireBus = PIN_18B20;   
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void Sensor_18B20::begin(const char* html_place, const char* label, const char* mqtt_name) {
  obj_label = label;
  obj_html_place = html_place;
  obj_mqtt_name = mqtt_name;
  obj_hw_pin1 = PIN_18B20;
  obj_resolution = 12;
  sensors.begin();
  sensors.setResolution(obj_resolution);

  obj_info_mqtt = "\"Sensor-HW\":\"18B20\",\"Sensor-Resolution\":";
  obj_info_mqtt += String(obj_resolution);
  obj_info_mqtt += ",\"Sensor-Refreshtime\":";
  obj_info_mqtt += String(obj_measure_interval);
  obj_info_mqtt += " Sek.\"";

  obj_info_html =  "\"sensorinfo1\":\"Hardware:#18B20\"";
  obj_info_html += ",\"sensorinfo2\":\"Resolution:#";
  obj_info_html += String(obj_resolution);
  obj_info_html += "\"";
  obj_info_html += ",\"sensorinfo3\":\"Refreshtime:#";
  obj_info_html += String(obj_measure_interval);
  obj_info_html += " Sek.\"";
}

void Sensor_18B20::start_measure(time_t now) {
  obj_measure_starttime = now;
  obj_measure_started = true;
  sensors.requestTemperatures(); 
}

void Sensor_18B20::loop(time_t now) {
 if (obj_measure_started) {
    if ((now - obj_measure_starttime) > obj_measure_delay) {
      char tempstr[6];
      float tempC = -99;
      tempC = sensors.getTempCByIndex(0);

#if defined(DEBUG_SERIAL_MODULE)
      Serial.print("Temperatur: ");
      Serial.println(tempC);
#endif

      snprintf(tempstr,5,"%.1f",tempC);

      obj_html_stat_json ="{\"";
      obj_html_stat_json += obj_html_place;
      obj_html_stat_json += "\":\"";
      obj_html_stat_json += obj_label;
      obj_html_stat_json += ": ";
      obj_html_stat_json += String(tempstr);
      obj_html_stat_json += " °C\"}";

      obj_mqtt_json ="\"";
      obj_mqtt_json += obj_mqtt_name;
      obj_mqtt_json += "\":\"";
      obj_mqtt_json += String(tempstr);
      obj_mqtt_json += "\"";

      obj_values_str = obj_mqtt_name;
      obj_values_str += ":";
      obj_values_str += String(tempstr);

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