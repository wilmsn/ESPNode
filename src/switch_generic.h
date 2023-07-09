#ifndef _SWITCH_GENERIC_H_
#define _SWITCH_GENERIC_H_
#include "base_generic.h"
//#define DEBUG_SERIAL_MODULE
/***************************************************************************************
 * Ein leeres Objekt als Grundlage für alle Sensoren.
 * Jedes spezialisierte Objekt wird als Ableitung dieses Objektes erstellt.
 ***************************************************************************************/

/// @brief Ein generisches Objekt für einen Sensor. Nicht zum Einbau bestimmt, sondern nur als Vererbungsobjekt.\n 
/// Hier werden Grundfähigkeiten für jeden Sensor erstellt.\n \n 

class Switch_Generic : public Base_Generic {
public:



    /// @brief Zeigt den gesetzten MQTT Namen
    /// @return den MQTT Namen als Zeiger auf einen String
    const char* show_mqtt_name();

    /// @brief Gibt den Wert des Sensors aus. Wird hauptsächlich für den Wert von Schaltern benötigt.
    /// @return den MQTT Namen als Zeiger auf einen String
    const char* show_value();

};

#endif
