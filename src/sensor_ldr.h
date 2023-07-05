#ifndef _SENSOR_LDR_H_
#define _SENSOR_LDR_H_
/***************************************************************************************
 ***************************************************************************************/
#include "sensor_generic.h"

//Die folgende Zeile sorgt dafür das der Eingang A0 nicht genutzt wird um die interne Spannung zu messen!
#define ANALOGINPUT

/// @brief Ein abgeleitetes Objekt um einen LDR auszulesen\n 
/// Achtung: Da der analoge Eingang hier genutzt wird muss die Nutzung der Spannungüberwachung im Hauptprogramm ausgeschaltet werden.
/// Dies geschieht durch **#define ANALOGINPUT**
class Sensor_LDR : public Sensor_Generic {

public:
    /// @brief Die normale Initialisierung aus Sensor_Generic erweitert um eine Messung des LDR damit dieser sofort nach dem Start verfügbar ist.
    /// @param html_place Der Einbauort des Messwertes 
    /// @param label Der Bezeichner des Messwertes
    void begin(const char* html_place, const char* label);

    /// @brief Startet eine Messung
    void start_measure();

private:

    int obj_value;

};

#endif