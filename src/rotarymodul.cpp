#include "config.h"
#ifdef USE_ROTARYMODUL
#include "rotarymodul.h"
#include "common.h"

void RotaryModul::begin(uint8_t _pin_a, uint8_t _pin_b, uint8_t _pin_sw, uint8_t _pin_mode) {
  pin_a = _pin_a;
  pin_b = _pin_b;
  pin_sw = _pin_sw;
  pin_mode = _pin_mode;
	pinMode(pin_a, pin_mode);
	pinMode(pin_b, pin_mode);
	pinMode(pin_sw, pin_mode);
  raw_val = 0;
  raw_val_old = 0;
  rot_val = 0;
  read();
  read();
  rotChanged = false;
  rot_val = 0;
}

void RotaryModul::read() {

  int pin_a_val  = digitalRead(pin_a);
  int pin_b_val  = digitalRead(pin_b);
  int pin_sw_val = digitalRead(pin_sw);

  if (pin_a_val) {
    raw_a = 0b00000010;
  } else {
    raw_a = 0b00000000;
  }
  if (pin_b_val) {
    raw_b = 0b00000001;
  } else {
    raw_b = 0b00000000;
  }
  raw_val = raw_a | raw_b;
  if ( raw_val != raw_val_old) {
    // Hochdrehen:  00 >> 01 >> 11 >> 10 >> 00
    if (
      (raw_val_old == 0 && raw_val == 1) || 
      (raw_val_old == 1 && raw_val == 3) || 
      (raw_val_old == 3 && raw_val == 2) || 
      (raw_val_old == 2 && raw_val == 0) 
    ) {
      if (rot_val < rot_max_val) rot_val++;
      rotChanged = true;
    } 
    // Runterdrehen:  00 >> 10 >> 11 >> 01 >> 00
    else if (
      (raw_val_old == 0 && raw_val == 2) || 
      (raw_val_old == 2 && raw_val == 3) || 
      (raw_val_old == 3 && raw_val == 1) || 
      (raw_val_old == 1 && raw_val == 0)  
    ) {  
      if (rot_val > rot_min_val) rot_val--;
      rotChanged = true;
    } 
    raw_val_old = raw_val;
    char tmpstr[20];
    sprintf(tmpstr,"A: %u B: %u SW: %i", raw_a, raw_b, pin_sw_val);
    write2log(LOG_MODULE,1,tmpstr);
  }
  if (pin_sw_val) {
    raw_sw = 1;
  } else {
    raw_sw = 0;
  }
}

void RotaryModul::initLevel(uint8_t _level, uint16_t _minVal, uint16_t _curVal, uint16_t _maxVal) {
  if ( _level <= ROTARY_MAXLEVEL) {
    level[_level].min = _minVal;
    level[_level].cur = _curVal;
    level[_level].max = _maxVal;
    if ( cur_level == _level ) {
      setLevel(cur_level);
    }
  }
}

uint8_t RotaryModul::changed() {
  uint8_t retval = isChanged;
  isChanged = 0;
  return retval;
}

void RotaryModul::setIsChanged(uint8_t _changed) {
  isChanged = _changed;
}

uint8_t RotaryModul::curLevel() {
  return cur_level;
}

uint16_t RotaryModul::curValue() {
  return level[cur_level].cur;
}

uint16_t RotaryModul::curValue(uint8_t _level) {
  return level[_level].cur;
}

void RotaryModul::setMaxValue(uint16_t _maxValue) {
  level[cur_level].max = _maxValue;
  rot_max_val = _maxValue;
}

void RotaryModul::setMaxLevel(uint8_t _maxLevel) {
  max_level = _maxLevel;
}

void RotaryModul::setLevel(uint8_t _level) {
  if (_level <= max_level) {
    cur_level = _level;
  } else {
    cur_level = 0;
  }
  rot_min_val = level[cur_level].min;
  rot_max_val = level[cur_level].max;
  rot_val = level[cur_level].cur;
}

void RotaryModul::setValue(uint16_t _value) {
  level[cur_level].cur = _value;
  rot_val = _value;
}

void RotaryModul::loop(time_t now) {
  read();
  // 1. Position changed
  if (rotChanged) {
    level[cur_level].cur_old = level[cur_level].cur;
    level[cur_level].cur = rot_val;
    isChanged = 1;
    rotChanged = false;
  }
  /// 2. Button is pressed
  if (raw_sw == 0) {
    if (!wasButtonDown) {
      //start measuring
      millis_Button_pressed = millis();
    }
    //else we wait since button is still down
    wasButtonDown = true;
  } else {
    //button is up
    if (wasButtonDown) {
      wasButtonDown = false;
      if (millis() - millis_Button_pressed >= LONG_PRESSED_AFTER_MS) {
//      long click
        isChanged = 3;
      } else if (millis() - millis_Button_pressed >= SHORT_PRESSED_AFTER_MS) {
//      short click => Change Level
        if ( cur_level < ROTARY_MAXLEVEL) {
          setLevel(cur_level+1);
        } else {
          setLevel(0);
        }
        isChanged = 2;
      }
    }
  }
}


#endif
