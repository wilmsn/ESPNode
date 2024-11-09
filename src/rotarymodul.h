#ifdef USE_ROTARYMODUL

#ifndef _ROTARYMODUL_H_
#define _ROTARYMODUL_H_
/***************************************************************************************
 ***************************************************************************************/

typedef struct {
    uint16_t min;
    uint16_t max;
    uint16_t cur;
    uint16_t cur_old;
} level_t;

#define LONG_PRESSED_AFTER_MS     1000
#define SHORT_PRESSED_AFTER_MS    20
#define ROTARY_MAXLEVEL           5


class RotaryModul {

public:
    /// @brief Die Initialisierung des Rotary Modules
    /// @brief Alle Werte werden innerhalb des Modules gesetzt.
    void begin(uint8_t _pin_a, uint8_t _pin_b, uint8_t _pin_sw, uint8_t _pin_mode);
    void initLevel(uint8_t _level, uint16_t _minVal, uint16_t _curVal, uint16_t _maxVal);
    uint8_t changed();
    uint8_t curLevel();
    uint16_t curValue();
    uint16_t curValue(uint8_t _level);
    void setMaxLevel(uint8_t _Maxlevel);
    void setLevel(uint8_t _level);
    void setMaxValue(uint16_t _maxValue);
    void setValue(uint16_t _value);
    void setIsChanged(uint8_t _changed);
    void loop(time_t now);
    void html_info(String& tmpstr);


private:
/// @brief Das Level Array. Hier werden alle Einstellungen in diesem Level verwaltet.
    level_t level[ROTARY_MAXLEVEL+1];
/// @brief Das aktuelle Level des Rotaryencoders
    uint8_t cur_level = 0;
/// @brief Der maximale Level des Rotaryencoders
    uint8_t max_level = 0;
/// @brief Zeitmessung Rotary Schalter
    unsigned long millis_Button_pressed = 0;
/// @brief Letzter Zustand Rotary Schalter
    bool wasButtonDown = false;
    uint8_t isChanged = 0;
    bool rotChanged = false;
//    time_t timeout_cnt;
    uint8_t  pin_a;
    uint8_t  pin_b;
    uint8_t  pin_sw;
    uint8_t  pin_mode;
    uint8_t  raw_a;
    uint8_t  raw_b;
    uint8_t  raw_sw;
    uint8_t  raw_val;
    uint8_t  raw_val_old;
    uint16_t rot_val;
    uint16_t rot_max_val;
    uint16_t rot_min_val;

    void read();
};

#endif
#endif