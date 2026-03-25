#ifndef _SENSOR_18B20_H_
#define _SENSOR_18B20_H_

#include <OneWire.h>
#include <DallasTemperature.h>
#include "base_generic.h"

/**
 * @brief Ein abgeleitetes Objekt für einen 18B20 Sensor.
 * Benötigt: OneWire und DallasTemperature als Gerätetreiber.
 */

class Sensor_18B20 : public Base_Generic {

public:
    /**
     * @brief Initialisierung des Sensorobjektes. 
     * Achtung: Der Port wird über die Precompilerdirektive ONEWIREBUS gesetzt.
     * @param _html_place Der Einbauort der HTML Seite
     * @param _html_label Ein Bezeichner für den Messwert
     * @param _keyword Der MQTT Bezeichner für den Messwert
     */
    void begin(const char* _html_place, const char* _html_label, const char* _keyword);

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

    /**
     * @brief Der MQTT Status
     * Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
     * "mqtt_nameX"+":"+"MesswertX",...
     * Hier steht immer ein abgeschlossenes Teil-JSON ohne Klammern.
     */
    void mqtt_stat(String& _mqtt_stat);

    /**
     * @brief Sollte es in diesem Modul telemetrieähnliche Daten geben, werden diese hier als Teil-JSON eingetragen
     */
    void mqtt_info(String& _mqtt_info);

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_init(String& _html_init);

    /**
     * @brief Updatedaten für die Webseite
     * Wenn sich der Inhalt der Webseite ändert, werden hier die geänderten Daten in Form eines Teil-JSON bereitgestellt.
     * Durch das zugrunde liegende Event wird die Variable html_update_set auf true gesetzt, damit das Hauptprogramm 
     * weiß, dass es neue Daten gibt. Das Hauptprogramm sendet diese Daten dann als Websocket an den Browser.
     */
    void html_update(String& _html_update);

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_info(String& _html_init);

private:

    OneWire* oneWire;
    DallasTemperature* sensor;

    /**
     * @brief Startet den Bosch Sensor im single Measure Mode. 
     * Nach der Erzeugung der Messwerte wird der Schalter "measure_started" gesetzt.
     */
    void start_measure(time_t now);

    /**
     * @brief Ein Flag ob die Messung gestartet wurde
     */
    bool       measure_started = false;

    /**
     * @brief Die Startzeit in Unix Sekunden - Initialwert 0 sorgt für sofortige Messung beim Start
     */
    time_t     measure_starttime = 0;

    /**
     * @brief Eine Variable für die Deviceadresse
     */
    DeviceAddress myThermometer;

    /**
     * @brief Ein String zur Aufnahme der gemessenen Temperatur
     */
    String tempC = String("---");

};

#endif