#ifndef _BASE_GENERIC_H_
#define _BASE_GENERIC_H_
#include <Arduino.h>
//#define DEBUG_SERIAL_MODULE
/***************************************************************************************
 * Ein leeres Objekt als Grundlage für alle Sensoren und Actoren.
 * Jedes spezialisierte Objekt wird als Ableitung dieses Objektes erstellt.
 ***************************************************************************************/

/// @brief Ein generisches Objekt für einen Sensor/Actor. Nicht zum Einbau bestimmt, sondern nur als Vererbungsobjekt.\n
/// Hier werden Grundfähigkeiten für jeden Sensor oder Actor erstellt.\n \n

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

    /// @brief Eine Funktion die im Hauptprogramm im loop regelmäßig aufgerufen wird. Hier können in den abgeleiteten Modulen regelmäßige Aufrufe hinterlegt werden.
    void loop(time_t now);

    bool set(const String& keyword, const String& value);

    /// @brief Prüft ob das übergebene keyword dem hinterlegten keyword entspricht
    /// @param keyword Das zu prüfende keyword
    /// @return true bei Übereinstimmung sonst false
    bool keyword_match(const String& keyword);

    /// @brief Dummy, in dieser Funktion wird bei der abgeleiteten Klasse der Webinhalt initialisiert.
    void html_create(String& tmpstr);

    /// @brief Sendet den Inhalt der Variablen "obj_htm_stat" als Websocketmessage.
    void html_refresh();
    
    /// @brief Dummy, in dieser Funktion wird bei der abgeleiteten Klasse der Inhalt für die Systeminfoseite geliefert.
    void html_info(String& tmpstr);

    // MQTT Support

    /// @brief Der mqtt_name wird im Topic3 für die Statusdaten genutzt.
    /// @return Inhalt der Variablen "obj_mqtt_name"
    String& mqtt_name();

    /// @brief Aktuelle Statuswerte werden hier für die MQTT Übertragung zurückgegeben.
    /// @brief Die Befüllung der zugrundeliegenden Variablen "obj_mqtt_state" erfolgt in dem abgeleiteten Script.
    /// @brief Der Eintrag erfolgt entweder als Einzelwert oder als JSON formatierter String.
    /// @brief Eine Übertragung per MQTT erfolgt nach jeder Änderung getriggert durch mqtt_stat_changed()
    /// @return Ein String
    String& mqtt_stat();

    /// @brief Sobald sich ein Wert innerhalb von mqtt_state() verändert muss die abgeleitete Klasse "obj_mqtt_state_changed" entsprechend setzen.
    /// @return true wenn sich mqtt_state geändert hat sonst false
    bool mqtt_stat_changed();

    /// @brief Hier wird alles übertragen was keine aktuelen Statuswerte sind.
    /// @return Ein Teil-JSON als String ohne Klammern
    String& mqtt_info();

    /// @brief Ein Schalter ob mqtt_info vorhanden ist.
    /// @return MQTT Info: true vorhanden; false nicht vorhanden
    bool mqtt_has_info();

 //----------------Variablen---------------   
 
    /// @brief Das Schlüsselword für diesen Sensor/Actor.
    String     obj_keyword;
    
    /// @brief Der Einbauort für diesen Sensor/Actor, dient auch als Schlüsselwort wenn die Änderung durch die Webseite verursacht wird.
    String     obj_html_place;
    
    /// @brief Eine Beschriftung für die Webseite. Wird sie gesetzt, wird sie auch als Schlüsselwort genutzt. 
    String     obj_label;
    
    /// @brief Systeminformationen zum Sensor für die Webseite als json abgespeichert;
    /// @brief Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Systeminfowert:
    /// @brief ""obj_html_placeX"+":"+"obj_labelX"+"MesswertX"+"EinheitX", ... "
    /// @brief Hier muss immer ein komplettes, gültiges Teil-JSON stehen dazu wird als default ein Dummy eingetragen.
    String     obj_html_info = "\"x\":0";

    /// @brief Informationen zum Sensor für die Webseite als json abgespeichert;
    /// @brief Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
    /// @brief ""obj_html_placeX"+":"+"obj_labelX"+"MesswertX"+"EinheitX", ... "
    /// @brief Hier muss immer ein komplettes, gültiges Teil-JSON stehen dazu wird als default ein Dummy eingetragen.
    String     obj_html_stat = "\"x\":0";

    // MQTT Support

    /// @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Telemetriedaten bereitstellt.
    bool       obj_mqtt_has_info;

    /// @brief Sollte es in diesem Modul telemetrieähnliche Daten geben, werden diese hier als Teil-JSON eingetragen
    String     obj_mqtt_info;

    /// @brief Schalter ob "obj_mqtt_state" verändert worden ist und neu (=true) übertragen werden soll.
    bool       obj_mqtt_stat_changed;

    /// @brief Der MQTT Status
    /// @brief Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
    /// @brief "mqtt_nameX"+":"+"MesswertX",...
    /// @brief Hier steht immer ein abgeschlossenes Teil-JSON ohne Klammern.
    String     obj_mqtt_stat;

    /// @brief Die Bezeichnung für den ersten Wert
    String     obj_mqtt_name;

};

#endif