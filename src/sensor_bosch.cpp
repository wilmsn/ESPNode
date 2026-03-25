#include "sensor_bosch.h"
#include "common.h"

void Sensor_Bosch::begin(const char* _html_place, const char* _html_label, const char* _keyword,
                         const char* _html_place2, const char* _html_label2, const char* _keyword2,
                         const char* _html_place3, const char* _html_label3, const char* _keyword3) {
  html_label3 = _html_label3;
  html_place3 = _html_place3;
  keyword3 = _keyword3;
  begin(_html_place, _html_label, _keyword, _html_place2, _html_label2, _keyword2);
}

void Sensor_Bosch::begin(const char* _html_place, const char* _html_label, const char* _keyword,
                         const char* _html_place2, const char* _html_label2, const char* _keyword2) {
  html_label2 = _html_label2;
  html_place2 = _html_place2;
  keyword2 = _keyword2;
  html_label = _html_label;
  html_place = _html_place;
  keyword = _keyword;
  this->bmx_sensor = new BMX_SENSOR();
  this->bmx_sensor->begin();

  if (this->bmx_sensor->isBMP180()) bmx = String("BMP180");
  if (this->bmx_sensor->isBMP280()) bmx = String("BMP280");
  if (this->bmx_sensor->isBME280()) bmx = String("BME280");

  mqtt_info_set = true;

  start_measure(0);
}

void Sensor_Bosch::html_init(String& _html_init) {
  _html_init = String("\"") + html_place + String("\":\"") + html_label + String(": ") + temp + String(" °C\"") +
              String(",\"") + html_place2 + String("\":\"") + html_label2 + String(": ") + pres + String(" hPa\"");
  if (this->bmx_sensor->isBME280()) {
    _html_init += String(",\"") + html_place3 + String("\":\"") + html_label3 + String(": ") + humi +String(" %\"");
  }            
}

void Sensor_Bosch::html_info(String& _html_info) {
  _html_info += String("\"tab_head_bosch\":\"Sensor\"") + String(",\"tab_line1_bosch\":\"HW: ") + bmx + String(":#GPIO: ");
#ifdef ESP8266
  _html_info += String("D1/D2 SDA/SCL\"");
#endif
  _html_info += String(",\"tab_line2_bosch\":\"Refreshtime:# ") + String(REFRESHTIME) + String(" Sek.\"")+
                String(",\"tab_line3_bosch\":\"Chip ID:# ") + String(this->bmx_sensor->getChipId()) + String("\"")+
                String(",\"tab_line4_bosch\":\"I2C Addr:# ") + String(this->bmx_sensor->getI2Cadr()) + String("\"");
}

void Sensor_Bosch::html_update(String& _html_update) {
  _html_update = String("\"") + html_place + String("\":\"") + html_label + String(": ") + temp + String(" °C\"") +
              String(",\"") + html_place2 + String("\":\"") + html_label2 + String(": ") + pres + String(" hPa\"");
  if (this->bmx_sensor->isBME280()) {
    _html_update += String(",\"") + html_place3 + String("\":\"") + html_label3 + String(": ") + humi +String(" %\"");
  }            
} 

void Sensor_Bosch::start_measure(time_t now) {
  measure_starttime = now;
  measure_started = true;
  this->bmx_sensor->startSingleMeasure();
}

void Sensor_Bosch::mqtt_info(String& _mqtt_info) {
  _mqtt_info += String("\"Sensor-HW\":\"") + bmx + String("\"");
}

void Sensor_Bosch::mqtt_stat(String& _mqtt_stat) {
  _mqtt_stat += String("\"") + keyword + String("\":\"") + String(this->bmx_sensor->getTemperature(),1) +
                String("\",\"") + keyword2 + String("\":\"") + String(this->bmx_sensor->getPressure(),0) + String("\"");
  if ( this->bmx_sensor->hasHumidity() ) {
    _mqtt_stat += String(",\"") + keyword3 + String("\":\"") + String(this->bmx_sensor->getHumidity(),0) + String("\"");
  }
}

void Sensor_Bosch::loop(time_t now) {
 if (measure_started) {
    if ((now - measure_starttime) > MEASUREDELAY) {
      temp = String(this->bmx_sensor->getTemperature(),1);
      pres = String(this->bmx_sensor->getPressure(),0);
      if ( this->bmx_sensor->hasHumidity() ) {
        humi = String(this->bmx_sensor->getHumidity(),0);
      }
      // Hier gibt es keine spezielle Initialisierung, deshalb können init Daten auch als Updatedaten geschickt werden
      html_update_set = true;
      mqtt_stat_set = true;
      mqtt_stat_changed = true;
      measure_started = false;
    }
  } else {
    if ((now - measure_starttime) > REFRESHTIME) {
      start_measure(now);
    }
  }
}

