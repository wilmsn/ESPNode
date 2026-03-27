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
     * @param _html_label wird in label gespeichert
     * @param _keyword wird in keyword gespeichert
     */
    void begin(const char* _html_place, const char* _html_label, const char* _keyword);

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
    bool set(const String& _cmnd, const String& _val);

    /**
     * @brief Prüft ob das übergebene keyword dem hinterlegten keyword entspricht
     * @param _cmnd Das zu prüfende Kommando wird gegen das keyword geprüft
     * @return true bei Übereinstimmung sonst false
     */
    bool keyword_match(const String& _cmnd);

    /**
     * @brief Das gespeicherte Schlüsselword für diesen Sensor/Actor.
     */
    String  keyword;

    /**
     * @brief Ein Schalter der angibt ob der Nodestatus aus diesem Modul genommen wird.
     * Macht nur Sinn bei einem Schalter und muss dann in der abgeleiteten Klasse auf true gesetzt werden.
     */
    bool is_state = false;
    
    // HTML Support

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_init(String& _html_init);

    /**
     * @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Daten für die HTML Initialisierung
     * bereitstellt.
     */
    bool html_init_set = false;

    /**
     * @brief Updatedaten für die Webseite
     * Wenn sich der Inhalt der Webseite ändert, werden hier die geänderten Daten in Form eines Teil-JSON bereitgestellt.
     * Durch das zugrunde liegende Event wird die Variable html_update_set auf true gesetzt, damit das Hauptprogramm 
     * weiß, dass es neue Daten gibt. Das Hauptprogramm sendet diese Daten dann als Websocket an den Browser.
     */
    void html_update(String& _html_update);

    /**
     * @brief HTML Update steht bereit.
     * Die Variable wird auf true gesetzt wenn ein inhalticher Update vorliegt.
     * Die Rücksetzung erfolgt im Hauptprogramm, nachdem die Daten übertragen wurden.
     */
    bool html_update_set = false;

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_info(String& _html_info);

    /**
     * @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Daten für die Seite "Systeminfo" 
     * bereitstellt.
     */
    bool html_info_set = false;
    
    /**
     * @brief Der Einbauort für diesen Sensor/Actor, dient auch als Schlüsselwort wenn die Änderung durch die Webseite verursacht wird.
     */
    String  html_place;
    
    /**
     * @brief Eine Beschriftung für die Webseite. Wird sie gesetzt, wird sie auch als Schlüsselwort genutzt.
     */
    String  html_label;

    /**
     * @brief Der aktuelle State des Nodes wird hier abgelegt (nur wenn dieses Modul den State setzt)
     */
    String state;

    // MQTT Support

    /**
     * @brief Sollte es in diesem Modul telemetrieähnliche Daten geben, werden diese hier als Teil-JSON eingetragen
     */
    void mqtt_info(String& _mqtt_info);

    /**
     * @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Telemetriedaten bereitstellt.
     */
    bool mqtt_info_set = false;

    /**
     * @brief Der MQTT Status
     * Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
     * "mqtt_nameX"+":"+"MesswertX",...
     * Hier steht immer ein abgeschlossenes Teil-JSON ohne Klammern.
     */
    void mqtt_stat(String& _mqtt_stat);
        
    /**
     * @brief Schalter ob "obj_mqtt_state" verändert worden ist und neu (=true) übertragen werden soll.
     * Rücksetzung erfolgt im Hauptprogramm.
     */
    bool mqtt_stat_changed = false;

    /**
     * @brief In der abgeleiteten Klasse wird hier auf "true" gesetzt wenn dieses Modul Statusdaten bereitstellt.
     */
    bool mqtt_stat_set = false;

    void append_comma(String& _string);

};

#endif