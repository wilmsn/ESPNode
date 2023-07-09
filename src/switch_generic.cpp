#include "switch_generic.h"

const char* Switch_Generic::show_mqtt_name() {
  return obj_mqtt_name.c_str();
}

const char* Switch_Generic::show_value() {
  return obj_values_str.c_str();
}
