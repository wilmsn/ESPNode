#include "sensor_18B20.h"
#include "common.h"

void Sensor_18B20::begin(const char* _html_place, const char* _html_label, const char* _keyword) {
  this->html_label = _html_label;
  this->html_place = _html_place;
  this->keyword = _keyword;
  this->oneWire = new OneWire(ONEWIREBUS);
  this->sensor = new DallasTemperature(this->oneWire);
  this->sensor->begin();
  if (!this->sensor->getAddress(this->myThermometer, 0)) {
    write2log(LOG_CRITICAL,1,"ERROR: Temeratursensor 18B20 nicht gefunden");
  }
  this->sensor->setResolution(this->myThermometer, RESOLUTION_18B20);

  this->mqtt_info_set = true;
  this->mqtt_stat_set = true;
  this->html_info_set = true;
  this->html_init_set = true;

  this->measure_starttime = 0;
}

void Sensor_18B20::start_measure(time_t now) {
  this->measure_starttime = now;
  this->measure_started = true;
  this->sensor->requestTemperatures(); 
}

void Sensor_18B20::html_init(String& _html_init) {
  _html_init += String("\"") + this->html_place + String("\":\"") + this->html_label + String(": ") + this->tempC + String(" °C\"");
}

void Sensor_18B20::html_update(String& _html_update) {
  _html_update += String("\"") + this->html_place + String("\":\"") + this->html_label + String(": ") + this->tempC + String(" °C\"");
}

void Sensor_18B20::html_info(String& _html_info) {
  _html_info += String("\"tab_head_18b20\":\"Sensor 18B20\"")+
                String(",\"tab_line1_18b20\":\"GPIO:# ") + String(ONEWIREBUS) + String("\"")+
                String(",\"tab_line2_18b20\":\"Resolution:# ") + String(RESOLUTION_18B20) + String("\"")+
                String(",\"tab_line3_18b20\":\"Refreshtime:# ") + String(REFRESHTIME) + String(" Sek.\"")+
                String(",\"tab_line4_18b20\":\"Measuredelaytime:# ") + String(MEASUREDELAY) + String(" Sek.\"")+
                String(",\"tab_line5_18b20\":\"Deviceaddress:# 0x");
  for (uint8_t i = 0; i < 8; i++) {
    if (this->myThermometer[i] < 16) _html_info += String("0");
    _html_info += String(this->myThermometer[i], HEX);
  }
  _html_info += String("\"");
}

void Sensor_18B20::mqtt_stat(String& _mqtt_stat) {
  _mqtt_stat += String("\"") + this->html_place + String("\":\"") + this->html_label + String(": ") + this->tempC + String(" °C\"");
}

void Sensor_18B20::mqtt_info(String& _mqtt_info) {
  _mqtt_info += String("\"Sensor\":\"18B20\"")+
                String(",\"Resolution\":\"") + String(RESOLUTION_18B20) + String("\"")+
                String(",\"Refreshtime\":\"") + String(REFRESHTIME) + String(" Sek.\"");
}

void Sensor_18B20::loop(time_t now) {
  if (this->measure_starttime == 0) {
    this->start_measure(now);
  }
  if (this->measure_started) {
    if ((now - this->measure_starttime) > MEASUREDELAY) {
      float tempCread;
      tempCread = this->sensor->getTempC(this->myThermometer);
      if (tempCread == DEVICE_DISCONNECTED_C) {
        this->start_measure(now); 
      } else {
        this->tempC = String(tempCread,1);
        this->measure_started = false;
        this->html_update_set = true;
        this->mqtt_stat_changed = true;
      }
    }
  } else {
    if ((now - this->measure_starttime) > REFRESHTIME) {
      this->start_measure(now);
    }
  } 
}
