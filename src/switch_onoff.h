#ifndef _SWITCH_ONOFF_H_
#define _SWITCH_ONOFF_H_
/***************************************************************************************
 ***************************************************************************************/
#include "base_generic.h"

/**
 * @brief Ein abgeleitetes Objekt für einen Ein-Aus Schalter.
 * Der Schalter reagiert nur wenn es in der **set** Funktion mit dem Schlüsselwort aufgerufen 
 * wurde das bei der Initialisierung hinterlegt wurde.
 * 
 * Folgende Strings als **value** übergeben schalten **aus**:
 * 
 * **0** **aus** **Aus** **off** **Off**
 * 
 * Folgende Strings als **value** übergeben schalten **ein**:
 * 
 * **1** **ein** **Ein** **on** **On**
 * 
 * Folgende Strings als **value** übergeben schalten **um**:
 * 
 * **2** **umschalten** **Umschalten** **toggle** **Toggle**
 */

class Switch_OnOff : public Base_Generic {

public:

    Switch_OnOff();

    /**
     * @brief Die Initialisierung des Schalters als logischer Schalter ohne HW-Pin
     * @param _html_place Der Einbauort in der Webseite
     * @param _label Ein Bezeichner für diesen Schalter
     * @param _mqtt_name Der Bezeichner in MQTT für diesen Schalter
     * @param _keyword Das Schlüsselword auf das dieser Schalter reagiert
     * @param _start_value Die initiale Schaltposition des Schalters
     * @param _on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
     * @param _is_state True wenn dieser SChalter den Status des Nodes darstellt sonst false. Es kann nur einen Status geben!
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
               bool _start_value, bool _on_value, bool _is_state);

    /**
     * @brief Die Initialisierung des Schalters für einen HW-Pin
     * @param _html_place Der Einbauort in der Webseite
     * @param _label Ein Bezeichner für diesen Schalter
     * @param _mqtt_name Der Bezeichner in MQTT für diesen Schalter
     * @param _keyword Das Schlüsselword auf das dieser Schalter reagiert
     * @param _start_value Die initiale Schaltposition des Schalters
     * @param _on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
     * @param _is_state True wenn dieser SChalter den Status des Nodes darstellt sonst false. Es kann nur einen Status geben!
     * @param _hw_pin Der Hardware Pin
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
               bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin);

    /**
     * @brief Die Initialisierung des Schalters für zwei HW-Pins
     * @param _html_place Der Einbauort in der Webseite
     * @param _label Ein Bezeichner für diesen Schalter
     * @param _mqtt_name Der Bezeichner in MQTT für diesen Schalter
     * @param _keyword Das Schlüsselword auf das dieser Schalter reagiert
     * @param _start_value Die initiale Schaltposition des Schalters
     * @param _on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
     * @param _is_state True wenn dieser SChalter den Status des Nodes darstellt sonst false. Es kann nur einen Status geben!
     * @param _hw_pin1 Der erste Hardware Pin
     * @param _hw_pin2 Der zweite Hardware Pin
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
               bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin1, uint8_t _hw_pin2);

    /**
     * @brief Die Initialisierung des Schalters mit Regler ohne HW-Pin
     * @param _html_place Der Einbauort in der Webseite
     * @param _label Ein Bezeichner für diesen Schalter
     * @param _mqtt_name Der Bezeichner in MQTT für diesen Schalter
     * @param _keyword Das Schlüsselword auf das dieser Schalter reagiert
     * @param _start_value Die initiale Schaltposition des Schalters
     * @param _on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
     * @param _is_state True wenn dieser SChalter den Status des Nodes darstellt sonst false. Es kann nur einen Status geben!
     * @param _slider_val Der Wert des Schiebereglers zum Programmstart
     * @param _slider_max_val Der maximale Wert des Schiebereglers (maximal zulässig: 255)
     * @param _slider_no Die Nummer des Einbauortes des Schiebereglers
     * @param _slider_label Die Beschriftung für den Schieberegler
     * @param _slider_mqtt_name Der MQTT Bezeichner für den Schieberegler
     * @param _slider_keyword Das Schlüsselword auf das dieser Schalter reagiert
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
               bool _start_value, bool _on_value, bool _is_state, uint8_t _slider_val, uint8_t _slider_max_val, uint8_t _slider_no,
               const char* _slider_label, const char* _slider_mqtt_name, const char* _slider_keyword);

    /**
     * @brief Die Initialisierung des Schalters für einen HW-Pin und PWM Steuerung
     * @param _html_place Der Einbauort in der Webseite
     * @param _label Die Beschritung für diesen Schalter
     * @param _mqtt_name Der Bezeichner in MQTT für diesen Schalter
     * @param _keyword Das Schlüsselword auf das dieser Schalter reagiert
     * @param _hw_pin Der Hardware Pin
     * @param _start_value Die initiale Schaltposition des Schalters
     * @param _on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
     * @param _is_state True wenn dieser SChalter den Status des Nodes darstellt sonst false. Es kann nur einen Status geben!
     * @param _slider_val Der Wert des Schiebereglers zum Programmstart
     * @param _slider_max_val Der maximale Wert des Schiebereglers (maximal zulässig: 255)
     * @param _slider_no Die Nummer des Einbauortes des Schiebereglers
     * @param _slider_label Die Beschriftung für den Schieberegler
     * @param _slider_mqtt_name Der MQTT Bezeichner für den Schieberegler
     * @param _slider_keyword Das Schlüsselword auf das dieser Schalter reagiert
     */
    void begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
               bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin, uint8_t _slider_val, uint8_t _slider_max_val, uint8_t _slider_no,
               const char* _slider_label, const char* _slider_mqtt_name, const char* _slider_keyword);

    /**
     * @brief Schaltet den Schalter auf den übergebenen Zustand wenn das übergebene "keyword" mit dem hinterlegten "keyword" übereinstimmt.
     * @param _cmnd Das Schlüsselword auf das dieser Schalter reagiert
     * @param _val Folgende Strings als **value** übergeben schalten **aus**:
     * 
     * **0** **aus** **Aus** **off** **Off**
     * 
     * Folgende Strings als **value** übergeben schalten **ein**:
     * 
     * **1** **ein** **Ein** **on** **On**
     * 
     * Folgende Strings als **value** übergeben schalten **um**:
     * 
     * **2** **umschalten** **Umschalten** **toggle** **Toggle**
     * @return "true" bei Übereinstimmung der Keywörter sonst false
     */
    bool set(const String& _cmnd, const String& _val);

     /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_init();

    /**
     * @brief Gibt die aktuellen Einstellungen des Sliders zurück 
     * @return Der aktuelle Wert des Sliders
     */
    uint8_t get_slider_val();

    /**
     * @brief Gibt die aktuellen Einstellungen des Schalters zurück
     * @return true = Schalter ein; false = Schalter aus
     */
    bool get_switch_val();

    /**
     * @brief Sets the switch
     * @param val 0=off 1=on 2=toogle
     */
    void set_switch(uint8_t _val);

    /**
     * @brief Sets the slider
     * @param val The value to set (0...255)
     */
    void set_slider(uint8_t _val);

    /**
     * @brief Sets the slider
     * @param val The value to set (0...255)
     */
    void set_slider_label(const char* _label);

    /**
     * @brief Sets the sliders max value
     * @param val The value to set (0...255)
     */
    void set_slider_max_value(uint8_t _val);

    /**
     * @brief Schaltet den Schalter auf einen neuen Zustand.
     * @param _new_state Der neue Zustand des Schalters ("true" = Ein, "false" = Aus).
     */
    void do_switch(bool _new_state);

    /**
     * @brief Der aktuelle Zustand des Schalters
     */
    bool switch_value;

    /**
     * @brief Der hardwareseitige Einschaltwert des Schalters.
     */
    bool on_value;

    /**
     * @brief Der zu setzende Wert beim Boot Vorgang
     */
    bool start_value;

    /**
     * @brief "true" wenn der Slider genutzt wird, sonst "false"
     */
    bool slider_used = false;

    /**
     * @brief Nimmt den MQTT Namen für den Switch auf
     */
    String switch_mqtt_name;

    /**
     * @brief Nimmt den MQTT Namen für den Slider auf
     */
    String slider_mqtt_name;

    /**
     * @brief Nimmt das Keyword für den Slider auf
     */
    String slider_keyword;

    /**
     * @brief Nimmt das Label für den Slider auf
     */
    String slider_label;

    /**
     * @brief Nimmt den aktuellen Wert des Sliders auf
     */
    uint8_t slider_value;

    /**
     * @brief Nimmt den maximalen Wert des Sliders auf
     */
    uint8_t slider_max_value;

    /**
     * @brief Nummer des Slieders (für den Einbau in die HTML Oberfläche)
     */
    uint8_t slider_no;

    /**
     * @brief Flag das festlegt ob HW-Pin1 genutzt wird (true = wird genutzt)
     */
    bool       hw_pin1_used = false;

    /**
     * @brief Flag das festlegt ob HW-Pin2 genutzt wird (true = wird genutzt)
     */
    bool       hw_pin2_used = false;

    /**
     * @brief Optional: Der Hardwarepin1 für diesen Sensor.
     */
    uint8_t    hw_pin1;

    /**
     * @brief Optional: Der Hardwarepin2 für diesen Sensor.
     */
    uint8_t    hw_pin2;

private:

};

#endif