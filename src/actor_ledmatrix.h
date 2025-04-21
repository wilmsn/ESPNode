#ifdef USE_ACTOR_LEDMATRIX
#ifndef _ACTOR_LEDMATRIX_H_
#define _ACTOR_LEDMATRIX_H_
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#include "LED_Matrix.h"

/**
 * HW-Pin für das Clock der LED Matrix
 */
#define LEDMATRIX_CLK        5  /* D1 */

/**
 * HW-Pin für das Data-In der LED Matrix
 */
#define LEDMATRIX_DIN        0  /* D3 */

/**
 * HW-Pin für das Chip-Select der LED Matrix
 */
#define LEDMATRIX_CS         2  /* D4 */

/**
 * Nummer der Displayblöcke (8*8 LED) in X Richtung
 */
#define LEDMATRIX_DEVICES_X  4

/**
 * Nummer der Displayblöcke (8*8 LED) in Y Richtung
 */
#define LEDMATRIX_DEVICES_Y  3

/**
 * Länge des Chararrays zur Aufnahme einer Textzeile auf der LED Matrix
 */
#define LINE_SIZE            8

/**
 * @brief Ein abgeleitetes Objekt für eine eine LED Matrix Anzeige.
 * Benötigt: https://github.com/wilmsn/LED_Matrix als Gerätetreiber.
 */
class Actor_LEDMatrix : public Switch_OnOff {

public:
    /**
     * @brief Die initialisierung dieses Objektes
     * @param _html_place Der Einbauplatz auf der HTML Seite für den Ein/Aus Schalter
     * @param _label Ein Beschriftungslabel für den Schalter
     * @param _mqtt_name Der MQTT Bezeichner für den Schalter
     * @param _keyword Das Schlüsselwort für den Schalter
     * @param _start_value Der Wert nach dem Einschalten (false = aus; true = ein)
     * @param _on_value Der logische Wert des IO Pins im eingeschalteten Zustand (false = aus; true = ein)
     * @param _is_state True wenn dieser SChalter den Status des Nodes darstellt sonst false. Es kann nur einen Status geben!
     * @param _slider_val Der Wert des Schiebereglers (Steuert die Helligkeit des Displays)
     * @param _slider_no Die Nummer des Schiebereglers (Einbauplatz)
     * @param _slider_label Eine Beschriftung für den Schieberegler
     * @param _slider_mqtt_name Der MQTT Bezeichner für den Schieberegler
     * @param _slider_keyword Ein Keyword für den Schieberegler
     * @param _mqtt_line Der MQTT Bezeichner für Zeilendaten
     * @param _mqtt_graph Der MQTT Bezeichner für Grafikdaten
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
               bool _start_value, bool _on_value, bool _is_state, uint8_t _slider_val, uint8_t _slider_no, 
               const char* _slider_label, const char* _slider_mqtt_name, const char* _slider_keyword,
               const char* _mqtt_line, const char* _mqtt_graph);

    /**
     * @brief Die "set" Methode des Actors.
     * Hier wird geprüft ob das Kommando für diesen Aktor bestimmt ist, wenn ja wird es ausgeführt.
     * @param _cmnd Der Bezeichner des Kommandos
     * @param _val Die Daten des Kommandos
     * @return true wenn das Kommando für dieses "set" bestimmt war sonst false
     */
    bool set(const String& _cmnd, const String& _val);
    
    /**
     * @brief Liest den Displayspeicher (Framebuffer) aus
     * @param fb_cont Übernimmt die Daten aus dem Displayspeicher als eine Folge von "0" und "1"
     */
    void getMatrixFB(String& fb_cont);

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_init();

    /**
     * @brief Die loop Funktion wird gegelmäßig vom Hauptprogramm aufgerufen
     * @param now Die aktuelle Zeit in Unix Sekunden
     */
    void loop(time_t now);

private:
    /**
     * @brief Erweitert den Inhalt der Variablen html_json um die aktuellen dynamischen Daten.
     */    
    void html_upd_data();

    /**
     * @brief Gibt eine Zeile im Display aus
     * Dabei haben die ersten 6 Zeichen im übergebenen Zeichenarray eine besondere Bedeutung:
     * rohtext[0] legt die Zeile fest in welcher der Text ausgegeben wird
     * rohtext[1] und rohtext[2] bilden den offset ab Zeilenanfang. Es sind nur numerische Zeichen erlaubt und der
     * offset in Pixel wird nach der Formel "rohtext[1]-'0')*10 + (rohtext[2]-'0'" berechnet
     * rohtext[3] als numerischer Wert setzt den verwendeten Font (siehe auch LED_Matrix.h)
     * rohtext[4] legt den Überschreibmodus fest (numerischer Wert, derzeit nur 0 => Zeile löschen und neu beschreiben)
     * rohtext[5] legt die Textausrichtung fest ("R" = rechtsbündig; "L" = linksbündig)
     * rohtext[6] und weitere sind der angezeigte Text
     * "LINE_SIZE" legt die Größe des Zeilenbuffers fest (Übertragene Zeichen + 6)
     * @param rohtext Der anzuzeigende Text mit Formatierung
     */
    void print_line(const char* rohtext);

    /**
     * @brief Gibt eine Grafik auf dem Display aus
     * Hier sind nur Zahlen erlaubt!
     * Jeder Punkt wird wie folgt beschrieben:
     * Die ersten beiden Zeichen ist der X-Offset
     * Die nächsten beiden Zeichen ist der Y-Offset
     * Das 5. Zeichen ist der Punkt (1 = setzen; 0 = löschen)
     * @param rohtext Der übergebene Text
     */
    void print_graph(const char* rohtext);

    /**
     * @brief Nimmt das Schlüsselword für den Zeilentext auf
     */
    String mqtt_line;

    /**
     * @brief Nimmt das Schlüsselwort für den Grafiktext auf
     */
    String mqtt_graph;

    /**
     * @brief Hält das letzte Update der Matrixanzeige fest
     * Da die Matrixanzeige erst nach mehreren Updates komplett ist,
     * wird diese erst 2 Sekunden nach dem ersten Update refreshed.
     */
    time_t graph_change_time;
};

#endif
#endif