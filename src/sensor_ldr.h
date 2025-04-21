#ifdef USE_SENSOR_LDR
#ifndef _SENSOR_LDR_H_
#define _SENSOR_LDR_H_
/***************************************************************************************
 ***************************************************************************************/
#include "base_generic.h"

/**
 * Die folgende Zeile sorgt dafür das der Eingang A0 nicht genutzt wird um die interne Spannung zu messen!
 */
#define ANALOGINPUT

/**
 * @brief Ein abgeleitetes Objekt um einen LDR auszulesen.
 * Keine Gerätetreiber benötigt.
 * Achtung: Da der analoge Eingang hier genutzt wird muss die Nutzung der Spannungüberwachung im Hauptprogramm ausgeschaltet werden.
 * Dies geschieht durch **#define ANALOGINPUT**
 */
class Sensor_LDR : public Base_Generic {

public:
    /**
     * @brief Die normale Initialisierung aus Sensor_Generic erweitert um eine Messung des LDR damit dieser sofort nach dem Start verfügbar ist.
     * @param _html_place Der Einbauort des Messwertes 
     * @param _label Der Bezeichner des Messwertes
     */
    void begin(const char* _html_place, const char* _label);

    /**
     * @brief Die loop Funktion wird gegelmäßig vom Hauptprogramm aufgerufen
     * @param now Die aktuelle Zeit in Unix Sekunden
     */
    void loop(time_t now);

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_init();

private:

    /**
     * @brief Der numerische Messwert des LDR
     */
    int            value;

    /**
     * @brief Die Startzeit in Unix Sekunden - Initialwert 0 sorgt für sofortige Messung beim Start
     */
    time_t         measure_starttime = 0;

};

#endif
#endif