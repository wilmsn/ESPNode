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

    /// @brief Sendet alle benötigten Daten um eine Zustandsänderung auf der Webseite für diesen Schalter darzustellen.
    /// @brief Sendet den Inhalt der Variablen "obj_htm_stat" als Websocketmessage wenn obj_html_has_stat = true.
    void html_update();
    
 //----------------Variablen---------------   
 
    /// @brief Das Schlüsselword für diesen Sensor/Actor.
    String     keyword;
    
    /// @brief Der Einbauort für diesen Sensor/Actor, dient auch als Schlüsselwort wenn die Änderung durch die Webseite verursacht wird.
    String     html_place;
    
    /// @brief Eine Beschriftung für die Webseite. Wird sie gesetzt, wird sie auch als Schlüsselwort genutzt. 
    String     label;

    /// @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Daten für die Webseiteninitialisierung bereitstellt.
    bool       html_has_stat = false;
 
    /// @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Daten für die Seite "Systeminfo" bereitstellt.
    bool       html_has_info = false;
    
    /// @brief Systeminformationen zum Sensor für die Webseite als json abgespeichert;
    /// @brief Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Systeminfowert:
    /// @brief ""obj_html_placeX"+":"+"obj_labelX"+"MesswertX"+"EinheitX", ... "
    /// @brief Hier muss immer ein komplettes, gültiges Teil-JSON stehen dazu wird als default ein Dummy eingetragen.
    String     html_info;

    /// @brief Wenn auf der Webseite spezielle Initialisierungsinformationen benötigt werden, 
    /// @brief müssen diese hier vom abgeleiteten Objekt im JSON Format abgelegt werden.
    String     html_init;

    /// @brief Informationen zum Sensor für die Webseite als json abgespeichert;
    /// @brief Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
    /// @brief ""obj_html_placeX"+":"+"obj_labelX"+"MesswertX"+"EinheitX", ... "
    String     html_stat;

    /// @brief Ein Schalter der angibt ob der Nodestatus aus diesem Modul genommen wird.
    /// @brief Macht nur Sinn bei einem Schalter und muss dann in der abgeleiteten Klasse auf true gesetzt werden.
    bool       is_state = false;

    /// @brief Der aktuelle State des Nodes wird hier abgelegt (nur wenn dieses Modul den State setzt)
    String     state;

    // MQTT Support

    /// @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Statusdaten bereitstellt.
    bool       mqtt_has_stat = false;

    /// @brief Der aktuelle Wert/Zustand des Schalters: "0" oder "1"
    bool       mqtt_state;

    /// @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Telemetriedaten bereitstellt.
    bool       mqtt_has_info = false;

    /// @brief Sollte es in diesem Modul telemetrieähnliche Daten geben, werden diese hier als Teil-JSON eingetragen
    String     mqtt_info;

    /// @brief Schalter ob "obj_mqtt_state" verändert worden ist und neu (=true) übertragen werden soll.
    bool       mqtt_stat_changed = false;

    /// @brief Der MQTT Status
    /// @brief Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
    /// @brief "mqtt_nameX"+":"+"MesswertX",...
    /// @brief Hier steht immer ein abgeschlossenes Teil-JSON ohne Klammern.
    String     mqtt_stat;

    /// @brief Die Bezeichnung für den ersten Wert
    String     mqtt_name;

};

#endif