#include "config.h"
#ifdef USE_SENSOR_BOSCH
#include "BMX_sensor.h"
#include <Wire.h>
#include "sensor_bosch.h"
#include "common.h"

BMX_SENSOR bmx_sensor;

void Sensor_Bosch::begin(const char* _html_place, const char* _label, const char* _mqtt_name,
                         const char* _html_place2, const char* _label2, const char* _mqtt_name2,
                         const char* _html_place3, const char* _label3, const char* _mqtt_name3) {
  label3 = _label3;
  html_place3 = _html_place3;
  mqtt_name3 = _mqtt_name3;
  begin(_html_place, _label, _mqtt_name, _html_place2, _label2, _mqtt_name2);
}

void Sensor_Bosch::begin(const char* _html_place, const char* _label, const char* _mqtt_name,
                         const char* _html_place2, const char* _label2, const char* _mqtt_name2) {
  label2 = _label2;
  html_place2 = _html_place2;
  mqtt_name2 = _mqtt_name2;
  label = _label;
  html_place = _html_place;
  mqtt_name1 = _mqtt_name;

  bmx_sensor.begin();

  if (bmx_sensor.isBMP180()) bmx = String("BMP180");
  if (bmx_sensor.isBMP280()) bmx = String("BMP280");
  if (bmx_sensor.isBME280()) bmx = String("BME280");

  mqtt_info = String("\"Sensor-HW\":\"")+bmx+String("\"");
  mqtt_has_info = true;

  html_info = String("\"tab_head_bosch\":\"Sensor\"") +
              String(",\"tab_line1_bosch\":\"HW: ") + bmx + String(":#GPIO: ");
#ifdef ESP8266
  html_info += String("D1/D2 SDA/SCL\"");
#endif
  html_info += String(",\"tab_line2_bosch\":\"Refreshtime:# ")+String(REFRESHTIME)+String(" Sek.\"")+
                   String(",\"tab_line3_bosch\":\"Chip ID:# ")+String(bmx_sensor.getChipId())+String("\"")+
                   String(",\"tab_line4_bosch\":\"I2C Addr:# ")+String(bmx_sensor.getI2Cadr())+String("\"");
  html_has_info = true;
  
  mqtt_name = bmx;

  start_measure(0);
}

void Sensor_Bosch::html_init() {
  html_json = String("\"") + html_place + String("\":\"") + label + String(": ") + temp + String(" °C\"") +
              String(",\"") + html_place2 + String("\":\"") + label2 + String(": ") + pres + String(" hPa\"");
  if (bmx == String("BME280")) {
    html_json += String(",\"") + html_place3 + String("\":\"") + label3 + String(": ") + humi +String(" %\"");
  }            
  html_json_filled = true;
}

void Sensor_Bosch::start_measure(time_t now) {
  measure_starttime = now;
  measure_started = true;
  bmx_sensor.startSingleMeasure();
}

void Sensor_Bosch::loop(time_t now) {
 if (measure_started) {
    if ((now - measure_starttime) > MEASUREDELAY) {
      temp = String(bmx_sensor.getTemperature(),1);
      pres = String(bmx_sensor.getPressure(),0);
      if ( bmx_sensor.hasHumidity() ) {
        humi = String(bmx_sensor.getHumidity(),0);
      }
      // Hier gibt es keine spezielle Initialisierung, deshalb können init Daten auch als Updatedaten geschickt werden
      html_init();
      html_update();
      mqtt_stat = String("\"") + mqtt_name1 + String("\":\"") + String(bmx_sensor.getTemperature(),1) +
                  String("\",\"") + mqtt_name2 + String("\":\"") + String(bmx_sensor.getPressure(),0) + String("\"");
      if ( bmx_sensor.hasHumidity() ) {
        mqtt_stat += String(",\"") + mqtt_name3 + String("\":\"") + String(bmx_sensor.getHumidity(),0) + String("\"");
      }
      mqtt_stat_changed = true;
      mqtt_has_stat = true;
      measure_started = false;
    }
  } else {
    if ((now - measure_starttime) > REFRESHTIME) {
      start_measure(now);
    }
  }
}
#endif
