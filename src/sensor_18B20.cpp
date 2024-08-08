#include "config.h"
#ifdef USE_SENSOR_18B20
#include "sensor_18B20.h"
#include "common.h"
#define PIN_18B20      4

const int oneWireBus = PIN_18B20;   
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void Sensor_18B20::begin(const char* html_place, const char* label, const char* mqtt_name) {
  obj_label = label;
  obj_html_place = html_place;
  obj_mqtt_name = mqtt_name;
  obj_resolution = 12;
  obj_html_has_info = true;
  sensors.begin();
  sensors.setResolution(obj_resolution);

  obj_mqtt_info = "\"Sensor-HW\":\"18B20\",\"Sensor-Resolution\":";
  obj_mqtt_info += String(obj_resolution);
  obj_mqtt_info += ",\"Sensor-Refreshtime\":";
  obj_mqtt_info += String(obj_measure_interval);
  obj_mqtt_info += " Sek.\"";

  obj_html_info =  "{\"sensorinfo1\":\"Hardware:#18B20\"";
  obj_html_info += ",\"sensorinfo2\":\"Resolution:#";
  obj_html_info += String(obj_resolution);
  obj_html_info += "\"";
  obj_html_info += ",\"sensorinfo3\":\"Refreshtime:#";
  obj_html_info += String(obj_measure_interval);
  obj_html_info += " Sek.\"}";
}

void Sensor_18B20::start_measure(time_t now) {
  obj_measure_starttime = now;
  obj_measure_started = true;
  sensors.requestTemperatures(); 
}

void Sensor_18B20::html_create() {
  write2log(LOG_MODULE,1,obj_html_stat.c_str());
  ws.textAll(obj_html_stat.c_str());
}

void Sensor_18B20::loop(time_t now) {
  if (obj_measure_starttime == 0) {start_measure(now); }
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

      obj_html_stat ="{\"";
      obj_html_stat += obj_html_place;
      obj_html_stat += "\":\"";
      obj_html_stat += obj_label;
      obj_html_stat += ": ";
      obj_html_stat += String(tempstr);
      obj_html_stat += " °C\"}";
      write2log(LOG_MODULE,1,obj_html_stat.c_str());
      ws.textAll(obj_html_stat.c_str());

      obj_mqtt_stat = String(tempstr);

      obj_measure_started = false;
    }
  } else {
    if ((now - obj_measure_starttime) > obj_measure_interval) {
      start_measure(now);
    }
  }

}

#endif