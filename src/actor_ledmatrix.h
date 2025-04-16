#ifdef USE_ACTOR_LEDMATRIX
#ifndef _ACTOR_LEDMATRIX_H_
#define _ACTOR_LEDMATRIX_H_
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#include "LED_Matrix.h"

/// HW-Pin für das Clock der LED Matrix
#define LEDMATRIX_CLK        5  /* D1 */

/// HW-Pin für das Data-In der LED Matrix
#define LEDMATRIX_DIN        0  /* D3 */

/// HW-Pin für das Chip-Select der LED Matrix
#define LEDMATRIX_CS         2  /* D4 */

/// Nummer der Displayblöcke (8*8 LED) in X Richtung
#define LEDMATRIX_DEVICES_X  4

/// Nummer der Displayblöcke (8*8 LED) in Y Richtung
#define LEDMATRIX_DEVICES_Y  3

/// Länge des Chararrays zur Aufnahme einer Textzeile auf der LED Matrix
#define LINE_SIZE            8

/// @brief Ein abgeleitetes Objekt für eine eine LED Matrix.\n 
/// Benötigt: https://github.com/wilmsn/LED_Matrix als Gerätetreiber.\n 

class Actor_LEDMatrix : public Switch_OnOff {

public:
    /// @brief Die initialisierung dieses Objektes
    /// @param html_place Der Einbauplatz auf der HTML Seite für den Ein/Aus Schalter
    /// @param label Ein Beschriftungslabel für den Schalter
    /// @param mqtt_name Der MQTT Bezeichner für den Schalter
    /// @param keyword Das Schlüsselwort für den Schalter
    /// @param start_value Der Wert nach dem Einschalten (false = aus; true = ein)
    /// @param on_value Der logische Wert des IO Pins im eingeschalteten Zustand (false = aus; true = ein)
    /// @param is_state True wenn dieser SChalter den Status des Nodes darstellt sonst false. Es kann nur einen Status geben!
    /// @param slider_val Der Wert des Schiebereglers (Steuert die Helligkeit des Displays)
    /// @param slider_no Die Nummer des Schiebereglers (Einbauplatz)
    /// @param slider_label Eine Beschriftung für den Schieberegler
    /// @param slider_mqtt_name Der MQTT Bezeichner für den Schieberegler
    /// @param slider_keyword Ein Keyword für den Schieberegler
    /// @param mqtt_line Der MQTT Bezeichner für Zeilendaten
    /// @param mqtt_graph Der MQTT Bezeichner für Grafikdaten
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               bool start_value, bool on_value, bool is_state, uint8_t slider_val, uint8_t slider_no, 
               const char* slider_label, const char* slider_mqtt_name, const char* slider_keyword,
               const char* mqtt_line, const char* mqtt_graph);

    /// @brief Die "set" Methode des Actors. Hier wird geprüft ob das Kommando für diesen Aktor bestimmt ist, wenn ja
    /// wird es ausgeführt.
    /// @param keyword Der Bezeichner des Kommandos
    /// @param value Die Daten des Kommandos
    /// @return true wenn das Kommando für dieses "set" bestimmt war sonst false
    bool set(const String& keyword, const String& value);
    
    /// @brief Liest den Displayspeicher (Framebuffer) aus
    /// @param fb_cont Übernimmt die Daten aus dem Displayspeicher als eine Folge von "0" und "1"
    void getMatrixFB(String& fb_cont);

    /// @brief Erzeugt einen JSON zur Initialisierung innerhalb der Weboberfläche
    /// @param json Nimmt den JSON String zur Initialisierung auf
//    void html_create(String& json);

    void loop(time_t now);

private:
    /// @brief Gibt eine Zeile im Display aus
    /// Dabei haben die ersten 6 Zeichen im übergebenen Zeichenarray eine besondere Bedeutung:
    /// rohtext[0] legt die Zeile fest in welcher der Text ausgegeben wird
    /// rohtext[1] und rohtext[2] bilden den offset ab Zeilenanfang. Es sind nur numerische Zeichen erlaubt und der
    /// offset in Pixel wird nach der Formel "rohtext[1]-'0')*10 + (rohtext[2]-'0'" berechnet
    /// rohtext[3] als numerischer Wert setzt den verwendeten Font (siehe auch LED_Matrix.h)
    /// rohtext[4] legt den Überschreibmodus fest (numerischer Wert, derzeit nur 0 => Zeile löschen und neu beschreiben)
    /// rohtext[5] legt die Textausrichtung fest ("R" = rechtsbündig; "L" = linksbündig)
    /// rohtext[6] und weitere sind der angezeigte Text
    /// "LINE_SIZE" legt die Größe des Zeilenbuffers fest (Übertragene Zeichen + 6)
    /// @param rohtext Der anzuzeigende Text mit Formatierung
    void print_line(const char* rohtext);
    /// @brief Gibt eine Grafik auf dem Display aus
    /// Hier sind nur Zahlen erlaubt!
    /// Jeder Punkt wird wie folgt beschrieben:
    /// Die ersten beiden Zeichen ist der X-Offset
    /// Die nächsten beiden Zeichen ist der Y-Offset
    /// Das 5. Zeichen ist der Punkt (1 = setzen; 0 = löschen)
    /// @param rohtext Der übergebene Text
    void print_graph(const char* rohtext);
    /// @brief Nimmt das Schlüsselword für den Zeilentext auf
    String mqtt_line;
    /// @brief Nimmt das Schlüsselwort für den Grafiktext auf
    String mqtt_graph;
    /// @brief
    time_t graph_change_time;
};

#endif
#endif