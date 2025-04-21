#ifndef _BASE_GENERIC_H_
#define _BASE_GENERIC_H_
#include <Arduino.h>
/**
 * @brief Ein generisches Objekt für einen Sensor/Actor.
 * Achtung: Nicht zum Einbau bestimmt, sondern nur als Vererbungsobjekt.
 * Ein leeres Objekt als Grundlage für alle Sensoren und Actoren.
 * Jedes spezialisierte Objekt wird als Ableitung dieses Objektes erstellt.
 * Hier werden Grundfähigkeiten für jeden Sensor oder Actor erstellt.
 */

class Base_Generic {
public:
    /**
     *  @brief Der Initiator.
     *  Hier werden alle Variablen auf ihre Anfangswerte gesetzt.
     */
    Base_Generic(void);

    /**
     * @brief Legt die Startkonfiguration des Sensors fest.
     * Die übergebenen Variabelen werden in die entsprechenden Objektvariabelen gespeichert.
     * @param _html_place wird in html_place gespeichert.
     * @param _label wird in label gespeichert
     */
    void begin(const char* _html_place, const char* _label);
    
    /**
     * @brief Legt die Startkonfiguration des Sensors fest.
     * Die übergebenen Variabelen werden in die entsprechenden Objektvariabelen gespeichert.
     * @param _html_place wird in html_place gespeichert.
     * @param _label wird in label gespeichert
     * @param _mqtt_name wird in mqtt_name gespeichert
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name);

    /**
     * @brief Legt die Startkonfiguration des Sensors fest.
     * Die übergebenen Variabelen werden in die entsprechenden Objektvariabelen gespeichert.
     * @param _html_place wird in html_place gespeichert.
     * @param _label wird in label gespeichert
     * @param _mqtt_name wird in mqtt_name gespeichert
     * @param _keyword wird in keyword gespeichert
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword);

    /**
     * @brief Eine Funktion die im Hauptprogramm im loop regelmäßig aufgerufen wird.
     * Hier als leere Funktion implementiert, kann im abgeleiteten Objekt gefüllt werden.
     * Es ist der einzige Ort in dem das Modul Rechenzeit bekommt, sollte sparsam genutzt werden.
     */
    void loop(time_t now);

    /**
     * @brief Der Komandointerpreter.
     * Hier als leere Funktion implementiert, kann im abgeleiteten Objekt gefüllt werden.
     * Über die Set Funktion werden alle Kommandos gescheusst. Das Modul entscheidet selber ob und wie es
     * auf das übergebene Keywort reagiert.
     * @return true wenn das Modul für den übergebenen Befehl zuständig ist, sonst false.
     */
    bool set(const String& _keyword, const String& _value);

    /**
     * @brief Prüft ob das übergebene keyword dem hinterlegten keyword entspricht
     * @param _keyword Das zu prüfende keyword
     * @return true bei Übereinstimmung sonst false
     */
    bool keyword_match(const String& _keyword);

    /**
     * @brief Sendet ein Zustandsupdate an den Webbrowser.
     * Sendet alle benötigten Daten um eine Zustandsänderung auf der Webseite für dieses Modul darzustellen. 
     * Kann von der abgeleiteten Klasse genutzt weden, muss aber nicht. 
     * Um die Funktion zu nutzen müssen die zu sendenden Daten als Teil-JSON (ohne geschweifte Klammern)
     * in der Variablen "html_json" hinterlegt sein. 
     */
    void html_update();
    
    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_init();
 
    //----------------Variablen---------------   
 
    /**
     * @brief Das gespeicherte Schlüsselword für diesen Sensor/Actor.
     */
    String     keyword;
    
    /**
     * @brief Der Einbauort für diesen Sensor/Actor, dient auch als Schlüsselwort wenn die Änderung durch die Webseite verursacht wird.
     */
    String     html_place;
    
    /**
     * @brief Eine Beschriftung für die Webseite. Wird sie gesetzt, wird sie auch als Schlüsselwort genutzt.
     */
    String     label;

    /**
     * @brief Ein Schalter der angibt ob der Nodestatus aus diesem Modul genommen wird.
     * Macht nur Sinn bei einem Schalter und muss dann in der abgeleiteten Klasse auf true gesetzt werden.
     */
    bool       is_state = false;

    /**
     * @brief Der aktuelle State des Nodes wird hier abgelegt (nur wenn dieses Modul den State setzt)
     */
    String     state;

    // HTML Support

    /**
     * @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Daten für die Seite "Systeminfo" bereitstellt.
     */
    bool       html_has_info = false;
    
    /**
     * @brief Systeminformationen zum Sensor für die Webseite als json abgespeichert.
     * Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Systeminfowert:
     * ""html_placeX"+":"+"labelX"+"MesswertX"+"EinheitX", ... "
     * Hier muss immer ein komplettes, gültiges Teil-JSON stehen!
     */
    String     html_info;

    /**
     * @brief Informationen zum Sensor für die Webseite als json abgespeichert;
     * Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
     * ""html_placeX"+":"+"labelX"+"MesswertX"+"EinheitX", ... "
     */
    String     html_json;
    
    /**
     * @brief Schalter: Wird wahr wenn html_json gefüllt ist. 
     * Die Rücksetzung des Schalters erfolgt durch das Hauptprogramm.
     */
    bool       html_json_filled = false;

    // MQTT Support

    /**
     * @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Telemetriedaten bereitstellt.
     */
    bool       mqtt_has_info = false;

    /**
     * @brief Sollte es in diesem Modul telemetrieähnliche Daten geben, werden diese hier als Teil-JSON eingetragen
     */
    String     mqtt_info;

    /**
     * @brief Schalter ob "obj_mqtt_state" verändert worden ist und neu (=true) übertragen werden soll.
     * Rücksetzung erfolgt im Hauptprogramm.
     */
    bool       mqtt_stat_changed = false;

    /**
     * @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Statusdaten bereitstellt.
     */
    bool       mqtt_has_stat = false;

    /**
     * @brief Der MQTT Status
     * Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
     * "mqtt_nameX"+":"+"MesswertX",...
     * Hier steht immer ein abgeschlossenes Teil-JSON ohne Klammern.
     */
    String     mqtt_stat;

    /**
     * @brief Die Bezeichnung für den ersten Wert
     */
    String     mqtt_name;

};

#endif