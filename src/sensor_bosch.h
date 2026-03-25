#ifndef _SENSOR_BOSCH_H_
#define _SENSOR_BOSCH_H_

#include "base_generic.h"
#include "BMX_sensor.h"

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
     * @param _html_label Bezeichner in der HTML Seite für Messwert1 
     * @param _keyword Keyword für Messwert1 in der MQTT Übertragung
     * @param _html_place2 Einbauort in der HTML Seite für Messwert2
     * @param _html_label2 Bezeichner in der HTML Seite für Messwert2
     * @param _keyword2 Keyword für Messwert2 in der MQTT Übertragung
     */
    void begin(const char* _html_place, const char* _html_label, const char* _keyword1,
               const char* _html_place2, const char* _html_label2, const char* _keyword2);

    /**
     * @brief Initialisierung für 3 Messwerte (inkl. Luftfeuchte = BME280)
     * @param _html_place Einbauort in der HTML Seite für Messwert1
     * @param _html_label Bezeichner in der HTML Seite für Messwert1 
     * @param _keyword Keyword für Messwert1 in der MQTT Übertragung
     * @param _html_place2 Einbauort in der HTML Seite für Messwert2
     * @param _html_label2 Bezeichner in der HTML Seite für Messwert2
     * @param _keyword2 Keyword für Messwert2 in der MQTT Übertragung
     * @param _html_place3 Einbauort in der HTML Seite für Messwert3
     * @param _html_label3 Bezeichner in der HTML Seite für Messwert3
     * @param _keyword3 Keyword für Messwert3 in der MQTT Übertragung
     */
    void begin(const char* _html_place, const char* _html_label, const char* _keyword,
               const char* _html_place2, const char* _html_label2, const char* _keyword2,
               const char* _html_place3, const char* _html_label3, const char* _keyword3);

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
    void html_init(String& _html_init);

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_info(String& _html_init);

    /**
     * @brief Updatedaten für die Webseite
     * Wenn sich der Inhalt der Webseite ändert, werden hier die geänderten Daten in Form eines Teil-JSON bereitgestellt.
     * Durch das zugrunde liegende Event wird die Variable html_update_set auf true gesetzt, damit das Hauptprogramm 
     * weiß, dass es neue Daten gibt. Das Hauptprogramm sendet diese Daten dann als Websocket an den Browser.
     */
    void html_update(String& _html_update);

    /**
     * @brief Sollte es in diesem Modul telemetrieähnliche Daten geben, werden diese hier als Teil-JSON eingetragen
     */
    void mqtt_info(String& _mqtt_info);

    /**
     * @brief Der MQTT Status
     * Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
     * "mqtt_nameX"+":"+"MesswertX",...
     * Hier steht immer ein abgeschlossenes Teil-JSON ohne Klammern.
     */
    void mqtt_stat(String& _mqtt_stat);
     
private:

    /**
     * @brief Ein Zeiger auf das BMX_SENSOR Objekt. Dieses Objekt stellt die HW-Schnittstelle zum Sensor dar.
     */
    BMX_SENSOR* bmx_sensor;

    /**
     * @brief Startet den Bosch Sensor im single Measure Mode. 
     * Mit der Erzeugung der Messwerte wird der Schalter "measure_started" gesetzt.
     */
    void start_measure(time_t now);

    /**
     * @brief Eine Beschriftung des zweiten Meßwertes für die Webseite.
     */
    String     html_label2;

    /**
     * @brief Der Einbauort des zweiten Meßwertes für diesen Sensor.
     */
    String     html_place2;

    /**
     * @brief Der Bezeichner innerhalb des JSON für die MQTT Übertragung für Messwert2
     */
    String     keyword2;
 
    /**
     * @brief Eine Beschriftung des dritten Meßwertes für die Webseite.
     */
    String     html_label3;

    /**
     * @brief Der Einbauort des dritten Meßwertes für diesen Sensor.
     */
    String     html_place3;

    /**
     * @brief Der Bezeichner innerhalb des JSON für die MQTT Übertragung für Messwert3
     */
    String     keyword3;

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