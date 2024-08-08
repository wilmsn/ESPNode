
#ifdef USE_SENSOR_18B20
#ifndef _SENSOR_18B20_H_
#define _SENSOR_18B20_H_

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

    /// @brief Die loop Funktion wird gegelmäßig vom Hauptprogramm aufgerufen
    /// @param now Die aktuelle Zeit in Unix Sekunden
    void loop(time_t now);

    void html_create();
private:
    /// @brief Startet den Bosch Sensor im single Measure Mode. Nach der Erzeugung der Messwerte wird das Changed Flag gesetzt.
    void start_measure(time_t now);
    /// @brief Die Auflösung des Sensors
    uint8_t    obj_resolution = 12;
    /// @brief Ein Flag ob die Messung gestartet wurde
    bool       obj_measure_started = false;
    /// @brief Die Startzeit in Unix Sekunden - Initialwert 0 sorgt für sofortige Messung beim Start
    time_t     obj_measure_starttime = 0;
    /// @brief Das Intervall zwischen zwei Messungen in Sekunden
    time_t     obj_measure_interval = 300;
    /// @brief Der Abstand zwischen dem Start der Messung und dem Auslesen der Werte
    time_t     obj_measure_delay = 2;

};

#endif
#endif