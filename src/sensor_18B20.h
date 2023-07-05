#ifndef _SENSOR_18B20_H_
#define _SENSOR_18B20_H_
//#define DEBUG_SERIAL_MODULE

#include <OneWire.h>
#include <DallasTemperature.h>
#include "sensor_generic.h"

/// @brief Ein abgeleitetes Objekt für einen 18B20 Sensor.\n 
/// Benötigt: OneWire und DallasTemperature als Gerätetreiber.\n 

class Sensor_18B20 : public Sensor_Generic {

public:
    /// @brief 
    /// @param html_place 
    /// @param label 
    /// @param mqtt_name 
    /// @param pin 
    /// @param resolution 
//    void begin(const char* html_place, const char* label, const char* mqtt_name, uint8_t pin, uint8_t resolution);
    void begin(const char* html_place, const char* label, const char* mqtt_name);

    /// @brief Startet eine Messung. Nach der Erzeugung der Messwerte wird das Changed Flag gesetzt.
    void start_measure();

private:
    uint8_t obj_resolution = 12;

};

#endif