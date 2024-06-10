#include "config.h"
#ifdef USE_SENSOR_BOSCH
#include "config.h"
#include "sensor_bosch.h"
#include "BMX_sensor.h"
//#include <twi.h>
#include <Wire.h>


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
  obj_mqtt_name = mqtt_name;
  bmx_sensor.begin();
#if defined(DEBUG_SERIAL_MODULE)
  Serial.print("Chip ID:");
  Serial.println(bmx_sensor.getChipId());
  Serial.print("I2C Adr:");
  Serial.println(bmx_sensor.getI2Cadr());
  if (bmx_sensor.isBMP180()) {
    Serial.println("Sensor: BMP180");
  }
  if (bmx_sensor.isBMP280()) {
    Serial.println("Sensor: BMP280");
  }
  if (bmx_sensor.isBME280()) {
    Serial.println("Sensor: BME280");
  }
  Serial.print("Temperatur: ");
  if (bmx_sensor.hasTemperature()) {
    Serial.println(bmx_sensor.getTemperature());
  } else {
    Serial.println("Sensor unterstützt diese Messung nicht");
  }
  Serial.print("Luftdruck: ");
  if (bmx_sensor.hasPressure()) {
    Serial.println(bmx_sensor.getPressure());
  } else {
    Serial.println("Sensor unterstützt diese Messung nicht");
  }
  Serial.print("Luftfeuchte: ");
  if (bmx_sensor.hasHumidity()) {
    Serial.println(bmx_sensor.getHumidity());
  } else {
    Serial.println("Sensor unterstützt diese Messung nicht");
  }
#endif
  obj_info_mqtt = "\"Sensor-HW\":";
  if (bmx_sensor.isBMP180()) {
     obj_info_mqtt += "\"BMP180\"";
     obj_info_html = "\"sensorinfo1\":\"Hardware:#BMP180\"";
  }
  if (bmx_sensor.isBMP280()) {
    obj_info_mqtt += "\"BMP280\"";
    obj_info_html = "\"sensorinfo1\":\"Hardware:#BMP280\"";
  }
  if (bmx_sensor.isBME280()) {
    obj_info_mqtt += "\"BME280\"";
    obj_info_html = "\"sensorinfo1\":\"Hardware:#BME280\"";
  }
}

/*
void Sensor_Bosch::start_measure(time_t now) {
  char tempstr[6];
  bmx_sensor.startSingleMeasure();
  obj_html_stat_json ="{\"";
  obj_html_stat_json += obj_html_place;
  obj_html_stat_json += "\":\"";
  obj_html_stat_json += obj_label;
  obj_html_stat_json += ": ";
  snprintf(tempstr,5,"%.1f",bmx_sensor.getTemperature());
  obj_html_stat_json += String(tempstr);
  obj_html_stat_json += " °C\",\"";
  obj_html_stat_json += obj_html_place2;
  obj_html_stat_json += "\":\"";
  obj_html_stat_json += obj_label2;
  obj_html_stat_json += ": ";
  snprintf(tempstr,5,"%.0f",bmx_sensor.getPressure());
  obj_html_stat_json += String(tempstr);
  obj_html_stat_json += " hPa\"";
  if ( bmx_sensor.hasHumidity() ) {
    obj_html_stat_json += ",\"";
    obj_html_stat_json += obj_html_place3;
    obj_html_stat_json += "\":\"";
    obj_html_stat_json += obj_label3;
    obj_html_stat_json += ": ";
    snprintf(tempstr,5,"%.1f",bmx_sensor.getHumidity());
    obj_html_stat_json += String(tempstr);
    obj_html_stat_json += " %\"";
  }
  obj_html_stat_json += "}";

  obj_mqtt_json ="\"";
  obj_mqtt_json += obj_mqtt_name;
  obj_mqtt_json += "\":\"";
  snprintf(tempstr,5,"%.1f",bmx_sensor.getTemperature());
  obj_mqtt_json += String(tempstr);
  obj_mqtt_json += "\",\"";
  obj_mqtt_json += obj_mqtt_name2;
  obj_mqtt_json += "\":\"";
  snprintf(tempstr,5,"%.0f",bmx_sensor.getPressure());
  obj_mqtt_json += String(tempstr);
  obj_mqtt_json += "\"";
  if ( bmx_sensor.hasHumidity() ) {
    obj_mqtt_json += ",\"";
    obj_mqtt_json += obj_mqtt_name3;
    obj_mqtt_json += "\":\"";
    snprintf(tempstr,5,"%.1f",bmx_sensor.getHumidity());
    obj_mqtt_json += String(tempstr);
    obj_mqtt_json += " %\"";
  }

  obj_values_str = obj_mqtt_name;
  obj_values_str += ":";
  snprintf(tempstr,5,"%.1f",bmx_sensor.getTemperature());
  obj_values_str += String(tempstr);
  obj_values_str += "; ";
  obj_values_str += obj_mqtt_name2;
  obj_values_str += ":";
  snprintf(tempstr,5,"%.0f",bmx_sensor.getPressure());
  obj_values_str += String(tempstr);
  //obj_values_str += " hPa";
  if ( bmx_sensor.hasHumidity() ) {
    obj_values_str += "; ";
    obj_values_str += obj_mqtt_name3;
    obj_values_str += ":";
    snprintf(tempstr,5,"%.1f",bmx_sensor.getHumidity());
    obj_values_str += String(tempstr);
  //  obj_values_str += " %";
  }

  set_changed(true);
}
*/

void Sensor_Bosch::start_measure(time_t now) {
  obj_measure_starttime = now;
  obj_measure_started = true;
  bmx_sensor.startSingleMeasure();
}

void Sensor_Bosch::loop(time_t now) {
 if (obj_measure_started) {
    if ((now - obj_measure_starttime) > obj_measure_delay) {
      char tempstr[6];
      obj_html_stat_json ="{\"";
      obj_html_stat_json += obj_html_place;
      obj_html_stat_json += "\":\"";
      obj_html_stat_json += obj_label;
      obj_html_stat_json += ": ";
      snprintf(tempstr,5,"%.1f",bmx_sensor.getTemperature());
      obj_html_stat_json += String(tempstr);
      obj_html_stat_json += " °C\",\"";
      obj_html_stat_json += obj_html_place2;
      obj_html_stat_json += "\":\"";
      obj_html_stat_json += obj_label2;
      obj_html_stat_json += ": ";
      snprintf(tempstr,5,"%.0f",bmx_sensor.getPressure());
      obj_html_stat_json += String(tempstr);
      obj_html_stat_json += " hPa\"";
      if ( bmx_sensor.hasHumidity() ) {
        obj_html_stat_json += ",\"";
        obj_html_stat_json += obj_html_place3;
        obj_html_stat_json += "\":\"";
        obj_html_stat_json += obj_label3;
        obj_html_stat_json += ": ";
        snprintf(tempstr,5,"%.1f",bmx_sensor.getHumidity());
        obj_html_stat_json += String(tempstr);
        obj_html_stat_json += " %\"";
      }
      obj_html_stat_json += "}";

      obj_mqtt_json ="\"";
      obj_mqtt_json += obj_mqtt_name;
      obj_mqtt_json += "\":\"";
      snprintf(tempstr,5,"%.1f",bmx_sensor.getTemperature());
      obj_mqtt_json += String(tempstr);
      obj_mqtt_json += "\",\"";
      obj_mqtt_json += obj_mqtt_name2;
      obj_mqtt_json += "\":\"";
      snprintf(tempstr,5,"%.0f",bmx_sensor.getPressure());
      obj_mqtt_json += String(tempstr);
      obj_mqtt_json += "\"";
      if ( bmx_sensor.hasHumidity() ) {
        obj_mqtt_json += ",\"";
        obj_mqtt_json += obj_mqtt_name3;
        obj_mqtt_json += "\":\"";
        snprintf(tempstr,5,"%.1f",bmx_sensor.getHumidity());
        obj_mqtt_json += String(tempstr);
        obj_mqtt_json += " %\"";
      }

      obj_values_str = obj_mqtt_name;
      obj_values_str += ":";
      snprintf(tempstr,5,"%.1f",bmx_sensor.getTemperature());
      obj_values_str += String(tempstr);
      obj_values_str += "; ";
      obj_values_str += obj_mqtt_name2;
      obj_values_str += ":";
      snprintf(tempstr,5,"%.0f",bmx_sensor.getPressure());
      obj_values_str += String(tempstr);
      if ( bmx_sensor.hasHumidity() ) {
        obj_values_str += "; ";
        obj_values_str += obj_mqtt_name3;
        obj_values_str += ":";
        snprintf(tempstr,5,"%.1f",bmx_sensor.getHumidity());
        obj_values_str += String(tempstr);
      }

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
