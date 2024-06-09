#ifdef USE_SENSOR_LDR
#ifndef _SENSOR_LDR_H_
#define _SENSOR_LDR_H_
/***************************************************************************************
 ***************************************************************************************/
#include "base_generic.h"

//Die folgende Zeile sorgt dafür das der Eingang A0 nicht genutzt wird um die interne Spannung zu messen!
#define ANALOGINPUT

/// @brief Ein abgeleitetes Objekt um einen LDR auszulesen\n 
/// Achtung: Da der analoge Eingang hier genutzt wird muss die Nutzung der Spannungüberwachung im Hauptprogramm ausgeschaltet werden.
/// Dies geschieht durch **#define ANALOGINPUT**
class Sensor_LDR : public Base_Generic {

public:
    /// @brief Die normale Initialisierung aus Sensor_Generic erweitert um eine Messung des LDR damit dieser sofort nach dem Start verfügbar ist.
    /// @param html_place Der Einbauort des Messwertes 
    /// @param label Der Bezeichner des Messwertes
    void begin(const char* html_place, const char* label);

    void html_create_json_part(String& json);

    /// @brief Startet eine Messung
    /// @param now Die aktuelle Zeit in Unix Sekunden
    void start_measure(time_t now);

    /// @brief Die loop Funktion wird gegelmäßig vom Hauptprogramm aufgerufen
    /// @param now Die aktuelle Zeit in Unix Sekunden
    void loop(time_t now);

private:

    /// @brief Der numerische Messwert des LDR
    int            obj_value;
    /// @brief Ein Flag ob die Messung gestartet wurde
    bool           obj_measure_started = false;
    /// @brief Die Startzeit in Unix Sekunden - Initialwert 0 sorgt für sofortige Messung beim Start
    time_t         obj_measure_starttime = 0;
    /// @brief Das Intervall zwischen 2 Messungen in Sekunden
    time_t         obj_measure_interval = 60;
    /// @brief Der Abstand zwischen dem Start der Messung und dem Auslesen der Werte
    time_t         obj_measure_delay = 2;

};

#endif
#endif