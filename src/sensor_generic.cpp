#include "sensor_generic.h"

String& Sensor_Generic::sensorinfo_html() {
  return obj_sensorinfo_html;
}

String& Sensor_Generic::sensorinfo_mqtt() {
  return obj_sensorinfo_mqtt;
}

void Sensor_Generic::start_measure() {}

