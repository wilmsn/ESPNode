#include "config.h"
#ifdef USE_SENSOR_BOSCH
#include "BMX_sensor.h"
#include <Wire.h>
#include "sensor_bosch.h"
#include "common.h"

#define REFRESHTIME    300
#define MEASUREDELAY   2

BMX_SENSOR bmx_sensor;

void Sensor_Bosch::begin(const char* html_place, const char* label, const char* mqtt_name,
                         const char* html_place2, const char* label2, const char* mqtt_name2,
                         const char* html_place3, const char* label3, const char* mqtt_name3) {
  obj_label3 = label3;
  obj_html_place3 = html_place3;
  obj_mqtt_name3 = mqtt_name3;
  begin(html_place, label, mqtt_name, html_place2, label2, mqtt_name2);
}

void Sensor_Bosch::begin(const char* html_place, const char* label, const char* mqtt_name,
                         const char* html_place2, const char* label2, const char* mqtt_name2) {
  obj_label2 = label2;
  obj_html_place2 = html_place2;
  obj_mqtt_name2 = mqtt_name2;
  obj_label = label;
  obj_html_place = html_place;
  obj_mqtt_name1 = mqtt_name;

  bmx_sensor.begin();

  String bmx;
  if (bmx_sensor.isBMP180()) bmx = String("BMP180");
  if (bmx_sensor.isBMP280()) bmx = String("BMP280");
  if (bmx_sensor.isBME280()) bmx = String("BME280");

  obj_mqtt_info = String("\"Sensor-HW\":\"")+bmx+String("\"");
  obj_mqtt_has_info = true;

  obj_html_stat = String("\"")+String(obj_html_place)+String("\":\"")+obj_label+String(": --- °C\"")+
                  String(",\"")+obj_html_place2+String("\":\"")+obj_label2+String(": --- hPa\"");

  obj_html_info = String("\"tab_head_bosch\":\"Sensor\"")+
                  String(",\"tab_line1_bosch\":\"HW: ")+bmx+String(":#GPIO: ");
#ifdef ESP8266
  obj_html_info += String("D1/D2 SDA/SCL\"");
#endif
//  obj_html_info += String("\"")+
  obj_html_info += String(",\"tab_line2_bosch\":\"Refreshtime:# ")+String(REFRESHTIME)+String(" Sek.\"")+
                   String(",\"tab_line3_bosch\":\"Chip ID:# ")+String(bmx_sensor.getChipId())+String("\"")+
                   String(",\"tab_line4_bosch\":\"I2C Addr:# ")+String(bmx_sensor.getI2Cadr())+String("\"");

  obj_mqtt_name = bmx;

  start_measure(0);
}

void Sensor_Bosch::start_measure(time_t now) {
  obj_measure_starttime = now;
  obj_measure_started = true;
  bmx_sensor.startSingleMeasure();
}

void Sensor_Bosch::loop(time_t now) {
 if (obj_measure_started) {
    if ((now - obj_measure_starttime) > MEASUREDELAY) {
      char tempstr[6];
      obj_html_stat =  String("\"")+String(obj_html_place)+String("\":\"")+obj_label+String(": ");
      snprintf(tempstr,5,"%.1f",bmx_sensor.getTemperature());
      obj_html_stat += String(tempstr)+String(" °C\",\"")+obj_html_place2+String("\":\"")+obj_label2+String(": ");
      snprintf(tempstr,5,"%.0f",bmx_sensor.getPressure());
      obj_html_stat += String(tempstr)+String(" hPa\"");
      if ( bmx_sensor.hasHumidity() ) {
        obj_html_stat += String(",\"")+obj_html_place3+String("\":\"")+obj_label3+String(": ");
        snprintf(tempstr,5,"%.1f",bmx_sensor.getHumidity());
        obj_html_stat += String(tempstr)+String(" %\"");
      }

      obj_mqtt_stat =  String("{\"")+obj_mqtt_name1+String("\":\"");
      snprintf(tempstr,5,"%.1f",bmx_sensor.getTemperature());
      obj_mqtt_stat += String(tempstr)+String("\",\"")+obj_mqtt_name2+String("\":\"");
      snprintf(tempstr,5,"%.0f",bmx_sensor.getPressure());
      obj_mqtt_stat += String(tempstr)+String("\"");
      if ( bmx_sensor.hasHumidity() ) {
        obj_mqtt_stat += String(",\"")+obj_mqtt_name3+String("\":\"");
        snprintf(tempstr,5,"%.1f",bmx_sensor.getHumidity());
        obj_mqtt_stat += String(tempstr)+String("\"");
      }
      obj_mqtt_stat += String("}");
      obj_mqtt_stat_changed = true;
      obj_measure_started = false;
    }
  } else {
    if ((now - obj_measure_starttime) > REFRESHTIME) {
      start_measure(now);
    }
  }

}
#endif
