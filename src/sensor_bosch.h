#ifdef USE_SENSOR_BOSCH
#ifndef _SENSOR_BOSCH_H_
#define _SENSOR_BOSCH_H_
//#define DEBUG_SERIAL_MODULE

#include "base_generic.h"

/// @brief Ein abgeleitetes Objekt für einen Bosch Sensor.\n 
/// Benötigt: https://github.com/wilmsn/BMX_sensor als Gerätetreiber.\n 
/// Das Objekt kann entweder für 2 Meßwerte (Temperatur und Luftdruck) beim BMP180/BMP280 \n
/// oder für 3 Meßwerte (+ Luftfeuchte) beim BME280 initialisiert werden.\n 

class Sensor_Bosch : public Base_Generic {

public:
    /// @brief Initialisierung für 2 Messwerte (inkl. Luftfeuchte)
    /// @param html_place Einbauort in der HTML Seite für Messwert1
    /// @param label Bezeichner in der HTML Seite für Messwert1 
    /// @param mqtt_name Bezeichner für Messwert1 in der MQTT Übertragung
    /// @param html_place2 Einbauort in der HTML Seite für Messwert2
    /// @param label2 Bezeichner in der HTML Seite für Messwert2
    /// @param mqtt_name2 Bezeichner für Messwert2 in der MQTT Übertragung
    void begin(const char* html_place, const char* label, const char* mqtt_name,
               const char* html_place2, const char* label2, const char* mqtt_name2);

    /// @brief Initialisierung für 3 Messwerte (inkl. Luftfeuchte)
    /// @param html_place Einbauort in der HTML Seite für Messwert1
    /// @param label Bezeichner in der HTML Seite für Messwert1 
    /// @param mqtt_name Bezeichner für Messwert1 in der MQTT Übertragung
    /// @param html_place2 Einbauort in der HTML Seite für Messwert2
    /// @param label2 Bezeichner in der HTML Seite für Messwert2
    /// @param mqtt_name2 Bezeichner für Messwert2 in der MQTT Übertragung
    /// @param html_place3 Einbauort in der HTML Seite für Messwert3
    /// @param label3 Bezeichner in der HTML Seite für Messwert3
    /// @param mqtt_name3 Bezeichner für Messwert3 in der MQTT Übertragung
    void begin(const char* html_place, const char* label, const char* mqtt_name,
               const char* html_place2, const char* label2, const char* mqtt_name2,
               const char* html_place3, const char* label3, const char* mqtt_name3);

    /// @brief Die loop Funktion wird gegelmäßig vom Hauptprogramm aufgerufen
    /// @param now Die aktuelle Zeit in Unix Sekunden
    void loop(time_t now);

private:

    /// @brief Startet den Bosch Sensor im single Measure Mode. Nach der Erzeugung der Messwerte wird das Changed Flag gesetzt.
    void start_measure(time_t now);
    /// @brief Eine Beschriftung des zweiten Meßwertes für die Webseite. Wird sie nicht gesetzt, wird hier das Schlüsselwort genutzt. 
    String     obj_label2;
    /// @brief Der Einbauort des zweiten Meßwertes für diesen Sensor, dient auch als Schlüsselwort wenn die Änderung durch die Webseite verursacht wird.
    String     obj_html_place2;
    /// @brief Der Bezeichner innerhalb des JSON für die MQTT Übertragung für Messwert2
    String     obj_mqtt_name2;
    /// @brief Eine Beschriftung des dritten Meßwertes für die Webseite. Wird sie nicht gesetzt, wird hier das Schlüsselwort genutzt. 
    String     obj_label3;
    /// @brief Der Einbauort des dritten Meßwertes für diesen Sensor, dient auch als Schlüsselwort wenn die Änderung durch die Webseite verursacht wird.
    String     obj_html_place3;
    /// @brief Der Bezeichner innerhalb des JSON für die MQTT Übertragung für Messwert3
    String     obj_mqtt_name3;
    /// @brief Ein Flag ob die Messung gestartet wurde
    bool       obj_measure_started = false;
    /// @brief Die Startzeit in Unix Sekunden - Initialwert 0 sorgt für sofortige Messung beim Start
    time_t     obj_measure_starttime = 0;

};

#endif
#endif