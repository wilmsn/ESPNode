#include "config.h"
#ifdef USE_SENSOR_18B20
#include "sensor_18B20.h"
#include "common.h"

OneWire oneWire(ONEWIREBUS);
DallasTemperature sensors(&oneWire);

void Sensor_18B20::begin(const char* _html_place, const char* _html_label, const char* _keyword) {
  html_label = _html_label;
  html_place = _html_place;
  keyword = _keyword;
  sensors.begin();
  if (!sensors.getAddress(myThermometer, 0)) {
    write2log(LOG_CRITICAL,1,"ERROR: Temeratursensor 18B20 nicht gefunden");
  }
  sensors.setResolution(myThermometer, RESOLUTION_18B20);

  mqtt_info_set = true;
  mqtt_stat_set = true;
  html_info_set = true;
  html_init_set = true;

  measure_starttime = 0;
}

void Sensor_18B20::start_measure(time_t now) {
  measure_starttime = now;
  measure_started = true;
  sensors.requestTemperatures(); 
}

void Sensor_18B20::html_init(String& _html_init) {
  _html_init += String("\"") + html_place + String("\":\"") + html_label + String(": ") + tempC + String(" °C\"");
}

void Sensor_18B20::html_update(String& _html_update) {
  _html_update += String("\"") + html_place + String("\":\"") + html_label + String(": ") + tempC + String(" °C\"");
}

void Sensor_18B20::html_info(String& _html_info) {
  _html_info += String("\"tab_head_18b20\":\"Sensor 18B20\"")+
                String(",\"tab_line1_18b20\":\"GPIO:# ") + String(ONEWIREBUS) + String("\"")+
                String(",\"tab_line2_18b20\":\"Resolution:# ") + String(RESOLUTION_18B20) + String("\"")+
                String(",\"tab_line3_18b20\":\"Refreshtime:# ") + String(REFRESHTIME) + String(" Sek.\"")+
                String(",\"tab_line4_18b20\":\"Measuredelaytime:# ") + String(MEASUREDELAY) + String(" Sek.\"")+
                String(",\"tab_line5_18b20\":\"Deviceaddress:# 0x");
  for (uint8_t i = 0; i < 8; i++) {
    if (myThermometer[i] < 16) _html_info += String("0");
    _html_info += String(myThermometer[i], HEX);
  }
  _html_info += String("\"");
}

void Sensor_18B20::mqtt_stat(String& _mqtt_stat) {
  _mqtt_stat += String("\"") + html_place + String("\":\"") + html_label + String(": ") + tempC + String(" °C\"");
}

void Sensor_18B20::mqtt_info(String& _mqtt_info) {
  _mqtt_info += String("\"Sensor\":\"18B20\"")+
                String(",\"Resolution\":\"") + String(RESOLUTION_18B20) + String("\"")+
                String(",\"Refreshtime\":\"") + String(REFRESHTIME) + String(" Sek.\"");
}

void Sensor_18B20::loop(time_t now) {
  if (measure_starttime == 0) {
    start_measure(now); 
  }
  if (measure_started) {
    if ((now - measure_starttime) > MEASUREDELAY) {
      float tempCread;
      tempCread = sensors.getTempC(myThermometer);
      if (tempCread == DEVICE_DISCONNECTED_C) {
        start_measure(now); 
      } else {
        tempC = String(tempCread,1);
        measure_started = false;
        html_update_set = true;
        mqtt_stat_changed = true;
      }
    }
  } else {
    if ((now - measure_starttime) > REFRESHTIME) {
      start_measure(now);
    }
  }
}

#endif