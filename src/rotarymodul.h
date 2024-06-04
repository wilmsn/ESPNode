#ifdef USE_ROTARYMODUL

#ifndef _ROTARYMODUL_H_
#define _ROTARYMODUL_H_
/***************************************************************************************
 ***************************************************************************************/

typedef struct {
    uint8_t min;
    uint8_t max;
    uint8_t cur;
} level_t;

#define LONG_PRESSED_AFTER_MS     1000
#define SHORT_PRESSED_AFTER_MS    20
#define ROTARY_MAXLEVEL           2


class RotaryModul {

public:
    /// @brief Die Initialisierung des Rotary Modules
    /// @brief Alle Werte werden innerhalb des Modules gesetzt.
    void begin();
    void initLevel(uint8_t _level, uint8_t _minVal, uint8_t _curVal, uint8_t _maxVal);
    uint8_t changed();
//    void reset_changed();
    uint8_t curLevel();
    uint8_t curValue();
    void setLevel(uint8_t level);
    void setValue(uint8_t value);
    void loop();


private:
    level_t level[ROTARY_MAXLEVEL+1];
/// @brief Die aktuelle Position des Rotaryencoders
//    uint8_t cur_position = 0;
/// @brief Die aktuelle Station aus der Senderliste
    uint8_t cur_level = 0;
/// @brief Zeitmessung Rotary Schalter
    unsigned long millis_Button_pressed = 0;
/// @brief Letzter Zustand Rotary Schalter
    bool wasButtonDown = false;
    uint8_t isChanged = 0;

};

#endif
#endif