#include "config.h"
#ifdef USE_SENSOR_18B20
#include "sensor_18B20.h"
#include "common.h"
#define REFRESHTIME    300
#define RESOLUTION     12
#define MEASUREDELAY   10

OneWire oneWire(ONEWIREBUS);
DallasTemperature sensors(&oneWire);

void Sensor_18B20::begin(const char* _html_place, const char* _label, const char* _mqtt_name) {
  label = _label;
  html_place = _html_place;
  mqtt_name = _mqtt_name;
  sensors.begin();
  if (!sensors.getAddress(myThermometer, 0)) {
    write2log(LOG_CRITICAL,1,"ERROR: Temeratursensor 18B20 nicht gefunden");
  }
  sensors.setResolution(myThermometer, RESOLUTION);

  mqtt_info =  String("\"Sensor-HW\":\"18B20\",\"Sensor-Resolution\":") + String(RESOLUTION) +
                   String(",\"Sensor-Refreshtime\":\"") + String(REFRESHTIME) + String(" Sek.\"");
  mqtt_has_info = true;
  mqtt_has_stat = true;

  html_info =  String("\"tab_head_18b20\":\"Sensor 18B20\"")+
                   String(",\"tab_line1_18b20\":\"GPIO:# ")+String(ONEWIREBUS)+String("\"")+
                   String(",\"tab_line2_18b20\":\"Resolution:# ")+String(RESOLUTION)+String("\"")+
                   String(",\"tab_line3_18b20\":\"Refreshtime:# ")+String(REFRESHTIME)+String(" Sek.\"")+
                   String(",\"tab_line4_18b20\":\"Measuredelaytime:# ")+String(MEASUREDELAY)+String(" Sek.\"")+
                   String(",\"tab_line5_18b20\":\"Deviceaddress:# 0x");
                   for (uint8_t i = 0; i < 8; i++) {
                     if (myThermometer[i] < 16) html_info += String("0");
                     html_info += String(myThermometer[i], HEX);
                   }
  html_info += String("\"");
  html_has_info = true;

  html_init = 

  html_stat =  String("\"") + html_place + String("\":\"") + label + String(": --- \"");
  html_has_stat = true;
}

void Sensor_18B20::start_measure(time_t now) {
  measure_starttime = now;
  measure_started = true;
  sensors.requestTemperatures(); 
}

void Sensor_18B20::loop(time_t now) {
  if (measure_starttime == 0) {
    start_measure(now); 
  }
  if (measure_started) {
    if ((now - measure_starttime) > MEASUREDELAY) {
      float tempC = -99;
      float tempCread;
      tempCread = sensors.getTempC(myThermometer);
      if (tempCread == DEVICE_DISCONNECTED_C) {
        start_measure(now); 
      } else {
        tempC = tempCread;
        measure_started = false;
        html_stat = String("\"") + html_place + String("\":\"") + label + String(": ") +
                        String(tempC,1) + String(" °C\"");
        html_update();
        mqtt_stat = String("\"") + mqtt_name + String("\":") + String(tempC,1);
        mqtt_stat_changed = true;
        write2log(LOG_MQTT,2,"18B20 Loop",mqtt_stat.c_str());
      }
    }
  } else {
    if ((now - measure_starttime) > REFRESHTIME) {
      start_measure(now);
    }
  }
}

#endif