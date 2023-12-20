#ifndef _BASE_GENERIC_H_
#define _BASE_GENERIC_H_
#include <Arduino.h>
//#define DEBUG_SERIAL_MODULE
/***************************************************************************************
 * Ein leeres Objekt als Grundlage für alle Sensoren und Actoren.
 * Jedes spezialisierte Objekt wird als Ableitung dieses Objektes erstellt.
 ***************************************************************************************/

/// @brief Ein generisches Objekt für einen Sensor. Nicht zum Einbau bestimmt, sondern nur als Vererbungsobjekt.\n 
/// Hier werden Grundfähigkeiten für jeden Sensor oder Switch erstellt.\n \n 
/// Funktionsfähig sind folgende Funktionen:\n 
/// **begin**: zum Einbau ins "setup" des Hauptprogrammes. Diese ist ggf. in einem abgeleiteten Objekt um zusätzliche Parameter zu erweitern.\n 
/// **set_label** zum nachträglichen setzen oder verändern eines Labels.\n 
/// **set_keyword** zum nachträglichen setzen oder verändern eines Keywords.\n 
/// **set_mqtt_name** um den Namen für die Übertragung im JSON String festzulegen.\n 
/// **set_hw_pin** um bis zu 2 Hardwareanschlüsse festzulegen.\n 
/// **changed** um eine Statusveränderung zu erkennen.\n
/// **keyword_match** um zu prüfen ob der Sensor durch das übergebene Keyword angesprochen wird.\n 
/// **[*]_json** Funktionen um (Teil)Stücke eines JSON Statements zu erzeugen.\n 
/// **show_[*]** Gibt jeweils einen Zeiger auf die gespeicherte Inhalte aus.\n \n 
/// Als folgende Funktionen sind aus Gründen der Kompatibilität leer (als Dummy) implementiert:\n
/// **loop** Zum Aufruf in der Hauptschleife des Hauptprogrammes. Hier können in einem abgeleitetem Objekt regelmäßige Tätigkeiten implementiert werden.\n 
/// **web_create_json** Hier wird durch das abgeleitete Objekt das JSON zur Erzeugung des Sensors/Actors auf der Webseite erstellt.\n 

class Base_Generic {
public:
    /// @brief Der Initiator.\n 
    /// @brief Hier werden alle Variablen auf ihre Anfangswerte gesetzt.
    Base_Generic(void);

    /// @brief Legt die Startkonfiguration des Sensors fest. Die übergebenen Variabelen werden in die entsprechenden "obj_*" Variabelen gespeichert.\n 
    /// @param html_place wird in obj_html_place gespeichert.\n 
    /// @param label wird in obj_label gespeichert\n 
    void begin(const char* html_place, const char* label);
    
    /// @brief Legt die Startkonfiguration des Sensors fest. Die übergebenen Variabelen werden in die entsprechenden "obj_*" Variabelen gespeichert.
    /// @param html_place wird in obj_html_place gespeichert.
    /// @param label wird in obj_label gespeichert
    /// @param mqtt_name wird in obj_mqtt_name gespeichert
    void begin(const char* html_place, const char* label, const char* mqtt_name);

    /// @brief Legt die Startkonfiguration des Sensors fest. Die übergebenen Variabelen werden in die entsprechenden "obj_*" Variabelen gespeichert.
    /// @param html_place wird in obj_html_place gespeichert.
    /// @param label wird in obj_label gespeichert
    /// @param mqtt_name wird in obj_mqtt_name gespeichert
    /// @param keyword wird in obj_keyword gespeichert
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword);

    /// @brief Hier kann das Label separat gesetzt/geändert werden.
    void set_label(const char* label);
    
    /// @brief Hier kann das Keyword separat gesetzt/geändert werden.
    void set_keyword(const char* keyword);

    /// @brief Setzt den Namen für die MQTT Übertragung
    /// @param mqtt_name Der Name fürs json
    void set_mqtt_name(const char* mqtt_name);
    
    /// @brief Setzt den Hardware Pin
    /// @param pin1 Der Hardware Pin
    void set_hw_pin(uint8_t pin1);

    /// @brief Setzt zwei Hardware Pins
    /// @param pin1 Der erste Hardware Pin
    /// @param pin2 Der zweite Hardware Pin
    void set_hw_pin(uint8_t pin1, uint8_t pin2);
    
    /// @brief Eine Kontrollfunktion ob sich ein Wechsel eingestellt hat.
    /// @brief Beim Sensor: Neue Werte sind erstellt worden
    /// @brief Beim Actor: Ein Ereignis hat den Zustand geändert
    /// @return true: geändert; false unverändert
    bool changed();

    /// @brief Prüft ob das übergebene keyword dem hinterlegten keyword entspricht
    /// @param keyword Das zu prüfende keyword
    /// @return true bei Übereinstimmung sonst false
    bool keyword_match(const String& keyword);

    /// @brief Dummy, Erstelle ein Teil json für die MQTT Übertragung in einem abgeleiteten Objekt
    /// @return Ein JSON formatierter String
    String& mqtt_json_part();
    
    /// @brief Dummy, erstellt ein Teil-JSON zur initialen Konfiguration der Webseite in einem abgeleiteten Objekt
    /// @param json Ein übergebener String der erweitert wird
    void html_create_json_part(String& json);
    
    /// @brief Erstellt einen JSON der den aktuellen Wert inklusive evtl. benötigter HTML-Syntax beinhaltet
    /// @return json Ein übergebener String der komplett mit json Syntax gefüllt wird
    String& html_stat_json(void);

    /// @brief Zeigt das gesetzte keyword
    /// @return das keyword als Zeiger auf einen String
    String& show_keyword();

    /// @brief Zeigt das gesetzte label
    /// @return das label als Zeiger auf einen String
    String& show_label();

    /// @brief Eine Funktion die im Hauptprogramm im loop regelmäßig aufgerufen wird. Hier können in den abgeleiteten Modulen regelmäßige Aufrufe hinterlegt werden.
    void loop();

    /// @brief Das Schlüsselword für diesen Sensor.
    String     obj_keyword;
    /// @brief Der Einbauort für diesen Sensor, dient auch als Schlüsselwort wenn die Änderung durch die Webseite verursacht wird.
    String     obj_html_place;
    /// @brief Eine Beschriftung für die Webseite. Wird sie nicht gesetzt, wird hier das Schlüsselwort genutzt. 
    String     obj_label;
    /// @brief Der MQTT Name für diesen Sensor. Wird er nicht gesetzt wird hier das Schlüsselwort übertragen.
    String     obj_mqtt_name;
    /// @brief Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
    /// @brief "{"+"obj_html_placeX"+":"+"obj_labelX"+"MesswertX"+"EinheitX", ... +"}"
    /// @brief Hier muss immer ein komplettes, gültiges JSON stehen.
    String     obj_html_stat_json;
    /// @brief Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
    /// @brief "mqtt_nameX"+":"+"MesswertX",...
    /// @brief Hier steht immer nur ein abgeschlossenes Teil-JSON ohne Klammern.
    String     obj_mqtt_json;
    /// @brief Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
    /// @brief "mqtt_nameX"+":"+"MesswertX",...
    String     obj_values_str;
    /// @brief Flag das festlegt ob HW-Pin1 genutzt wird (true = wird genutzt)
    bool       obj_hw_pin1_used = false;
    /// @brief Flag das festlegt ob HW-Pin2 genutzt wird (true = wird genutzt)
    bool       obj_hw_pin2_used = false;
    /// @brief Optional: Der Hardwarepin1 für diesen Sensor.
    uint8_t    obj_hw_pin1;
    /// @brief Optional: Der Hardwarepin2 für diesen Sensor.
    uint8_t    obj_hw_pin2;
    /// @brief Ein Changed Flag, wird durch das abgeleitete Objekt gefüllt.
    bool       obj_changed;
};

#endif
// _GEN_SENSOR_H_