#ifndef _ROTARYMODUL_H_
#define _ROTARYMODUL_H_
/***************************************************************************************
 ***************************************************************************************/

#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"

#define LONG_PRESSED_AFTER_MS     1000
#define SHORT_PRESSED_AFTER_MS    20

//class RotaryLibMulti : public RotaryLib {

class AiEsp32RotaryExtention : public AiEsp32RotaryEncoder {

public:
	AiEsp32RotaryExtention(uint8_t encoderAPin, uint8_t encoderBPin, uint8_t encoderButtonPin, int encoderVccPin, uint8_t encoderSteps, bool areEncoderPinsPulldown_forEsp32);
    /// @brief Fügt eine neue App inkl. Lev 0 hinzu. Min, Max und Val werden mit 0 initialisiert.
    /// @return Die aktuelle Anzahl der Apps (max_app)
    uint8_t app_add();

    /// @brief Fügt eine neue App inkl. Lev 0 hinzu. Min, Max und Val werden mit den übergebenen Werten initialisiert.
    /// @param min Minimaler Rotarywert
    /// @param max Maximaler Rotarywert
    /// @param val Aktueller Rotarywert
    /// @return Die aktuelle Anzahl der Apps (max_app)
    uint8_t app_add(uint16_t min, uint16_t max, uint16_t val);

    /// @brief Aktiviert die übergebene App und das Level
    /// @param app Die Application
    /// @param lev Das Level
    /// @return true: Einstellungen erfolgreich; false: Einstellungen nicht erfolgreich
    bool app_set(uint8_t app, uint8_t lev);

    /// @brief Setzt das Rotary auf die angegebenen Parameter und aktiviert diese.
    /// @param app Die Application
    /// @param lev Das Level
    /// @param min Minimaler Rotarywert
    /// @param max Maximaler Rotarywert
    /// @param val Aktueller Rotarywert
    /// @return true: Einstellungen erfolgreich; false: Einstellungen nicht erfolgreich
    bool app_set(uint8_t app, uint8_t lev, uint16_t min, uint16_t max, uint16_t val);

    /// @brief Setzt die App um 1 herauf, ist die Top_App erreicht wird die App auf 0 gesetzt
    /// @return Die gesetzte App
    uint8_t app_up();

    /// @brief Die aktuelle App abfragen
    /// @return Die aktuelle App
    uint8_t app();

    /// @brief Die maximale App abfragen
    /// @return Die maximale App
    uint8_t get_max_app();

    /// @brief Fügt ein neues Level zu einer App hinzu. Min, Max und Val werden mit 0 initialisiert.
    /// @param app Die Nummer einer App
    /// @return Das maximale Level in dieser App
    uint8_t lev_add(uint8_t app);

    /// @brief Fügt ein neues Level zu einer App hinzu. Min, Max und Val werden mit den übergebenen Werten initialisiert.
    /// @param app Die Nummer der zu erweiternden App
    /// @param min Minimaler Rotarywert
    /// @param max Maximaler Rotarywert
    /// @param val Aktueller Rotarywert
    /// @return Das maximale Level in dieser App
    uint8_t lev_add(uint8_t app, uint16_t min, uint16_t max, uint16_t val);

    /// @brief Setzt das Level um 1 herauf, ist das Top_Level erreicht wird das Level auf 0 gesetzt
    /// @return Das gesetzte Level
    uint8_t lev_up();

    /// @brief Das aktuelle Level abfragen
    /// @return Das aktuelle Level
    uint8_t lev();

    /// @brief Das maximale Level für die übergebene App abfragen
    /// @return Das maximale Level der übergebenen App
    uint8_t get_max_lev(uint8_t app);

    /// @brief Den maximalen Wert für die aktuelle Kombination aus App und Level setzen
    /// @param _max Der maximale Wert
    void max_set(uint16_t _max);

    /// @brief Den minimalen Wert für die aktuelle Kombination aus App und Level setzen
    /// @param _min Der minimale Wert
    void min_set(uint16_t _min);

    /// @brief Den aktuellen Wert für die aktuelle Kombination aus App und Level setzen
    /// @param _val Der aktuelle Wert
    void val_set(uint16_t _val);

    uint16_t val();

    uint16_t min();

    uint16_t max();

    bool valChanged();

    void begin();

    void loop(time_t now);

    bool buttonShortPressed = false;

    bool buttonLongPressed = false;

private:
    void set_val_changed();
    void reset_dir_cnt();
/// @brief Zeitmessung Rotary Schalter
    unsigned long millis_Button_pressed = 0;
/// @brief Letzter Zustand Rotary Schalter
    bool wasButtonDown = false;
    time_t timeout_cnt;

    struct app_t {
        uint8_t  app;
        uint8_t  lev;
        uint16_t min;
        uint16_t max;
        uint16_t val;
        app_t* p_app_next;
    };

    app_t * p_app_initial;

    app_t * p_app_current;

};



#endif
