#ifdef USE_SENSOR_18B20
#ifndef _SENSOR_18B20_H_
#define _SENSOR_18B20_H_
//#define DEBUG_SERIAL_MODULE

#include <OneWire.h>
#include <DallasTemperature.h>
#include "base_generic.h"

/// @brief Ein abgeleitetes Objekt für einen 18B20 Sensor.\n 
/// Benötigt: OneWire und DallasTemperature als Gerätetreiber.\n 

class Sensor_18B20 : public Base_Generic {

public:
    /// @brief Initialisierung des Sensorobjektes
    /// @param html_place Der Einbauort der HTML Seite
    /// @param label Ein Bezeichner für den Messwert
    /// @param mqtt_name Der MQTT Bezeichner für den Messwert
    void begin(const char* html_place, const char* label, const char* mqtt_name);

    /// @brief Startet eine Messung. Nach der Erzeugung der Messwerte wird das Changed Flag gesetzt.
    void start_measure();

private:
    /// @brief Die Auflösung des Sensors
    uint8_t obj_resolution = 12;

};

#endif
#endif