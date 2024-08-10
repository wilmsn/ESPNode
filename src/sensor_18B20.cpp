#include "config.h"
#ifdef USE_SENSOR_18B20
#include "sensor_18B20.h"
#include "common.h"
#define PIN_18B20      4
#define REFRESHTIME    300
#define RESOLUTION     12
#define MEASUREDELAY   2

const int oneWireBus = PIN_18B20;   
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void Sensor_18B20::begin(const char* html_place, const char* label, const char* mqtt_name) {
  obj_label = label;
  obj_html_place = html_place;
  obj_mqtt_name = mqtt_name;
  sensors.begin();
  sensors.setResolution(RESOLUTION);

  obj_mqtt_info =  String("\"Sensor-HW\":\"18B20\",\"Sensor-Resolution\":");
  obj_mqtt_info += String(RESOLUTION);
  obj_mqtt_info += String(",\"Sensor-Refreshtime\":");
  obj_mqtt_info += String(REFRESHTIME);
  obj_mqtt_info += String(" Sek.\"");
  obj_mqtt_has_info = true;

  obj_html_info =  String(",\"tab_head_18b20\":\"Sensor\"")+
  obj_html_info += String(",\"tab_line1_18b20\":\"HW 18B20:#GPIO: ")+String(PIN_18B20)+String("\"")+
  obj_html_info += String(",\"tab_line2_18b20\":\"Resolution:# ")+String(RESOLUTION)+String("\"")+
  obj_html_info += String(",\"tab_line3_18b20\":\"Refreshtime:# ")+String(REFRESHTIME)+String(" Sek.\"");

  obj_html_stat =  String("\"");
  obj_html_stat += obj_html_place;
  obj_html_stat += String("\":\"");
  obj_html_stat += obj_label;
  obj_html_stat += String(": --- \"");
}

void Sensor_18B20::start_measure(time_t now) {
  obj_measure_starttime = now;
  obj_measure_started = true;
  sensors.requestTemperatures(); 
}

void Sensor_18B20::loop(time_t now) {
  if (obj_measure_starttime == 0) {start_measure(now); }
  if (obj_measure_started) {
    if ((now - obj_measure_starttime) > MEASUREDELAY) {
      char tempstr[6];
      float tempC = -99;
      tempC = sensors.getTempCByIndex(0);

#if defined(DEBUG_SERIAL_MODULE)
      Serial.print("Temperatur: ");
      Serial.println(tempC);
#endif

      snprintf(tempstr,5,"%.1f",tempC);

      obj_html_stat =  String("\"");
      obj_html_stat += obj_html_place;
      obj_html_stat += String("\":\"");
      obj_html_stat += obj_label;
      obj_html_stat += String(": ");
      obj_html_stat += String(tempstr);
      obj_html_stat += String(" °C\"");
      sendWsMessage(String("{")+obj_html_stat+String("}",LOG_MODULE));

      obj_mqtt_stat = String(tempstr);

      obj_measure_started = false;
    }
  } else {
    if ((now - obj_measure_starttime) > REFRESHTIME) {
      start_measure(now);
    }
  }
}

#endif