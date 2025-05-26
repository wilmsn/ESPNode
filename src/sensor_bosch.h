#ifndef _SENSOR_BOSCH_H_
#define _SENSOR_BOSCH_H_

#include "base_generic.h"

/**
 * @brief Ein abgeleitetes Objekt für einen Bosch Sensor.
 * Benötigt: https://github.com/wilmsn/BMX_sensor als Gerätetreiber.
 * Das Objekt kann entweder für 2 Meßwerte (Temperatur und Luftdruck) beim BMP180/BMP280
 * oder für 3 Meßwerte (+ Luftfeuchte) beim BME280 initialisiert werden.
 */

class Sensor_Bosch : public Base_Generic {

public:
    /**
     * @brief Initialisierung für 2 Messwerte (Temperatur und Luftdruck = BMP180/280)
     * @param _html_place Einbauort in der HTML Seite für Messwert1
     * @param _label Bezeichner in der HTML Seite für Messwert1 
     * @param _mqtt_name Bezeichner für Messwert1 in der MQTT Übertragung
     * @param _html_place2 Einbauort in der HTML Seite für Messwert2
     * @param _label2 Bezeichner in der HTML Seite für Messwert2
     * @param _mqtt_name2 Bezeichner für Messwert2 in der MQTT Übertragung
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name,
               const char* _html_place2, const char* _label2, const char* _mqtt_name2);

    /**
     * @brief Initialisierung für 3 Messwerte (inkl. Luftfeuchte = BME280)
     * @param _html_place Einbauort in der HTML Seite für Messwert1
     * @param _label Bezeichner in der HTML Seite für Messwert1 
     * @param _mqtt_name Bezeichner für Messwert1 in der MQTT Übertragung
     * @param _html_place2 Einbauort in der HTML Seite für Messwert2
     * @param _label2 Bezeichner in der HTML Seite für Messwert2
     * @param _mqtt_name2 Bezeichner für Messwert2 in der MQTT Übertragung
     * @param _html_place3 Einbauort in der HTML Seite für Messwert3
     * @param _label3 Bezeichner in der HTML Seite für Messwert3
     * @param _mqtt_name3 Bezeichner für Messwert3 in der MQTT Übertragung
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name,
               const char* _html_place2, const char* _label2, const char* _mqtt_name2,
               const char* _html_place3, const char* _label3, const char* _mqtt_name3);

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
     * @brief Startet den Bosch Sensor im single Measure Mode. 
     * Mit der Erzeugung der Messwerte wird der Schalter "measure_started" gesetzt.
     */
    void start_measure(time_t now);

    /**
     * @brief Der Bezeichner innerhalb des JSON für die MQTT Übertragung für Messwert1
     */
    String     mqtt_name1;

    /**
     * @brief Eine Beschriftung des zweiten Meßwertes für die Webseite.
     */
    String     label2;

    /**
     * @brief Der Einbauort des zweiten Meßwertes für diesen Sensor.
     */
    String     html_place2;

    /**
     * @brief Der Bezeichner innerhalb des JSON für die MQTT Übertragung für Messwert2
     */
    String     mqtt_name2;
 
    /**
     * @brief Eine Beschriftung des dritten Meßwertes für die Webseite.
     */
    String     label3;

    /**
     * @brief Der Einbauort des dritten Meßwertes für diesen Sensor.
     */
    String     html_place3;

    /**
     * @brief Der Bezeichner innerhalb des JSON für die MQTT Übertragung für Messwert3
     */
    String     mqtt_name3;

    /**
     * @brief Ein Schalter ob die Messung gestartet wurde
     */
    bool       measure_started = false;

    /**
     * @brief Die Startzeit in Unix Sekunden - Initialwert 0 sorgt für sofortige Messung beim Start.
     */
    time_t     measure_starttime;

    /**
     * @brief Bezeichnung des Sensors (z.B. BMP180)
     */
    String bmx;

    /**
     * @brief Die gemessene Temperatur als String
     */
    String temp = String("---");

    /**
     * @brief Der gemessene Luftdruck als String
     */
    String pres = String("---");

    /**
     * @brief Die gemessene Luftfeuchtigkeit als String
     */
    String humi = String("---");
};

#endif