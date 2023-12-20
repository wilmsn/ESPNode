#ifndef _SENSOR_GENERIC_H_
#define _SENSOR_GENERIC_H_
#include "base_generic.h"
//#define DEBUG_SERIAL_MODULE
/***************************************************************************************
 * Ein leeres Objekt als Grundlage für alle Sensoren.
 * Jedes spezialisierte Objekt wird als Ableitung dieses Objektes erstellt.
 ***************************************************************************************/

/// @brief Ein generisches Objekt für einen Sensor. Nicht zum Einbau bestimmt, sondern nur als Vererbungsobjekt.\n 
/// Hier werden Grundfähigkeiten für jeden Sensor erstellt.\n \n 

class Sensor_Generic : public Base_Generic {
public:

    /// @brief Gibt json formatierte Sensorinformationen für die Webseite aus. Diese Funktion muss innerhalb des abgeleiteten Objektes befüllt werden.
    /// @return Einen json Teilstring als String.
    String& sensorinfo_html();

    /// @brief Gibt json formatierte Sensorinformationen für MQTT Übertragung aus. Diese Funktion muss innerhalb des abgeleiteten Objektes befüllt werden.
    /// @return Einen json Teilstring als String.
    String& sensorinfo_mqtt();

    /// @brief Informationen zum Sensor für die Webseite als json abgespeichert;
    String     obj_sensorinfo_html;
    /// @brief Informationen zum Sensor für MQTT als json abgespeichert;
    String     obj_sensorinfo_mqtt;

    void start_measure();

};

#endif
