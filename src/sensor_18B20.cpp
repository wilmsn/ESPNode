#include "config.h"

#ifdef _SENSOR_18B20_H_

const int oneWireBus = 4;   
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void Sensor_18B20::begin(const char* html_place, const char* label, const char* mqtt_name) {
  obj_label = label;
  obj_html_place = html_place;
  obj_mqtt_name = mqtt_name;
  obj_hw_pin1 = 4;
  obj_resolution = 12;
  sensors.begin();
  sensors.setResolution(obj_resolution);
  start_measure();
#if defined(DEBUG_SERIAL_MODULE)
  Serial.print("Temperatur: ");
  Serial.println(sensors.getTempCByIndex(0));
#endif
  obj_sensorinfo_mqtt = "\"Sensor-HW\":\"18B20\",\"Sensor-Resolution\":";
  obj_sensorinfo_mqtt += String(obj_resolution);
  obj_sensorinfo_html =  "\"sensorinfo1\":\"Hardware:#18B20\"";
  obj_sensorinfo_html += ",\"sensorinfo2\":\"Resolution:#";
  obj_sensorinfo_html += String(obj_resolution);
  obj_sensorinfo_html += "\"";
}

void Sensor_18B20::start_measure() {

  char tempstr[6];
  float tempC = -99;
  sensors.requestTemperatures(); 
  tempC = sensors.getTempCByIndex(0);

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
  obj_mqtt_json += " °C\"";

  obj_values_str = obj_mqtt_name;
  obj_values_str += ":";
  obj_values_str += String(tempstr);
  obj_values_str += " °C";

  obj_changed = true;
#if defined(DEBUG_SERIAL_MODULE)
  Serial.print("Temperatur: ");
  Serial.println(tempC);
#endif

}

#endif