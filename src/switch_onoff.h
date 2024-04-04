#ifndef _SWITCH_ONOFF_H_
#define _SWITCH_ONOFF_H_
/***************************************************************************************
 ***************************************************************************************/
#include "switch_generic.h"

/// @brief Ein abgeleitetes Objekt für einen Ein-Aus Schalter.\n
///Der Schalter reagiert nur wenn es in der **set** Funktion mit dem Schlüsselwort aufgerufen 
///wurde das bei der Initialisierung hinterlegt wurde.\n
///Folgende Strings als **value** übergeben schalten **aus**:\n
///**0** **aus** **Aus** **off** **Off**\n
///Folgende Strings als **value** übergeben schalten **ein**:\n
///**1** **ein** **Ein** **on** **On**\n
///Folgende Strings als **value** übergeben schalten **um**:\n
///**2** **umschalten** **Umschalten** **toggle** **Toggle**\n

class Switch_OnOff : public Switch_Generic {

public:
    /// @brief Die Initialisierung des Schalters als logischer Schalter ohne HW-Pin
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    /// @param start_value Die initiale Schaltposition des Schalters
    /// @param on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               bool start_value, bool on_value);

    /// @brief Die Initialisierung des Schalters für einen HW-Pin
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    /// @param hw_pin Der Hardware Pin
    /// @param start_value Die initiale Schaltposition des Schalters
    /// @param on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               uint8_t hw_pin, bool start_value, bool on_value);

    /// @brief Die Initialisierung des Schalters für zwei HW-Pins
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    /// @param hw_pin1 Der erste Hardware Pin
    /// @param hw_pin2 Der zweite Hardware Pin
    /// @param start_value Die initiale Schaltposition des Schalters
    /// @param on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               uint8_t hw_pin1, uint8_t hw_pin2, bool start_value, bool on_value);

    /// @brief Die Initialisierung des Schalters mit Regler ohne HW-Pin
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    /// @param start_value Die initiale Schaltposition des Schalters
    /// @param on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
    /// @param slider_val Der Wert des Schiebereglers zum Programmstart
    /// @param slider_no Die Nummer des Einbauortes des Schiebereglers
    /// @param slider_mqtt_name Der MQTT Bezeichner für den Schieberegler
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               bool start_value, bool on_value, uint8_t slider_val, uint8_t slider_no,
               const char* slider_mqtt_name);

    /// @brief Die Initialisierung des Schalters für einen HW-Pin und PWM Steuerung
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    /// @param hw_pin Der Hardware Pin
    /// @param start_value Die initiale Schaltposition des Schalters
    /// @param on_value Der Zustand des HW-Pis wenn der Schalter eingeschaltet ist.
    /// @param slider_val Der Wert des Schiebereglers zum Programmstart
    /// @param slider_no Die Nummer des Einbauortes des Schiebereglers
    /// @param slider_mqtt_name Der MQTT Bezeichner für den Schieberegler
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               uint8_t hw_pin, bool start_value, bool on_value, uint8_t slider_val, uint8_t slider_no,
               const char* slider_mqtt_name);

    /// @brief Schaltet den Schalter auf den übergebenen Zustand wenn das übergebene "keyword" mit dem hinterlegten "obj_keyword" übereinstimmt.
    /// @param keyword Das zu testende "keyword"
    /// @param value Folgende Strings als **value** übergeben schalten **aus**:\n 
    ///**0** **aus** **Aus** **off** **Off**\n 
    ///Folgende Strings als **value** übergeben schalten **ein**:\n 
    ///**1** **ein** **Ein** **on** **On**\n 
    ///Folgende Strings als **value** übergeben schalten **um**:\n 
    ///**2** **umschalten** **Umschalten** **toggle** **Toggle**\n 
    /// @return "true" bei Übereinstimmung der Keywörter sonst false
    bool set(const String& keyword, const String& value);

    /// @brief Erstellt ein Teil-JSON zur initialen Konfiguration der Webseite
    /// @param json Ein übergebener String der erweitert wird
    void html_create_json_part(String& json);

    /// @brief Gibt die aktuellen Einstellungen des Sliders zurück 
    /// @return Der aktuelle Wert des Sliders
    uint8_t get_slider_val();

    /// @brief Gibt die aktuellen Einstellungen des Schalters zurück
    /// @return true = Schalter ein; false = Schalter aus
    bool get_switch_val();

    /// @brief Sets the switch
    /// @param val 0=off 1=on 2=toogle
    void set_switch(uint8_t val);

    /// @brief Sets the slider
    /// @param val The value to set (0...255)
    void set_slider(uint8_t val);

    /// @brief Sets the sliders max value
    /// @param val The value to set (0...255)
    void set_slider_max_value(uint8_t val);

    /// @brief Schaltet die Hardware auf den in "state" übergebenen Zustand. "true" = Ein, "false" = Aus
    /// @param state Der neue Zustand des Schalters (0...255).
    void do_switch(bool state);

    /// @brief Der hardwareseitige Einschaltwert des Schalters.
    bool obj_on_value;
    /// @brief Der zu setzende Wert beim Boot Vorgang
    bool obj_start_value;
    /// @brief Der aktuelle Wert/Zustand des Schalters: "0" oder "1"
    bool obj_value;
    /// @brief "true" wenn der Slider genutzt wird, sonst "false"
    bool obj_slider_used = false;
    /// @brief Nimmt den MQTT Namen für den Slider auf
    String obj_slider_mqtt_name;
    /// @brief Nimmt den aktuellen Wert des Sliders auf
    uint8_t obj_slider_val;
    /// @brief Nimmt den maximalen Wert des Sliders auf
    uint8_t obj_slider_max_val;
    /// @brief Nummer des Slieders (für den Einbau in die HTML Oberfläche)
    uint8_t obj_slider_no;
//private:

};

#endif
