#include "config.h"
#ifdef USE_ROTARYMODUL
#include "rotarymodul.h"

#if defined(CONFIG_IDF_TARGET_ESP32) 
#warning ESP32
#warning "Compiling Audiomodule with Settings for ESP32"
// Definitions for ESP32 Board
// Rotary Encoder Settings
#define ROTARY_ENCODER_A_PIN            32
#define ROTARY_ENCODER_B_PIN            33
#define ROTARY_ENCODER_BUTTON_PIN       4
#define ROTARY_ENCODER_VCC_PIN          -1
//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS            4
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S2)
#warning ESP32S2
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
//#warning ESP32S3
#warning "Compiling Rotarymodule with Settings for ESP32-S3"
//#warning "Settings for ESP32-S3"
#define ROTARY_ENCODER_A_PIN      1 //38 //37
#define ROTARY_ENCODER_B_PIN      2 //39 //36
#define ROTARY_ENCODER_BUTTON_PIN 3 //40 //35
#define ROTARY_ENCODER_VCC_PIN    -1
#define ROTARY_ENCODER_STEPS      4
#endif

#include "AiEsp32RotaryEncoder.h"

#define DEBUG_SERIAL_MODULE


/// The settings for each level for the rotary encoder
/// 0 => Radio playing level
/// 1 => station select level
/// 2 => ????


/// @brief Instance for rotary encoder
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

void IRAM_ATTR readRotaryISR() {
  rotary.readEncoder_ISR();
}

void RotaryModul::begin() {
  rotary.begin(); //start rotary handling
  rotary.setup(readRotaryISR); //register interrupt service routine
}

void RotaryModul::initLevel(uint8_t _level, uint8_t _minVal, uint8_t _curVal, uint8_t _maxVal) {
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

uint8_t RotaryModul::curLevel() {
  return cur_level;
}

uint8_t RotaryModul::curValue() {
  return level[cur_level].cur;
}

void RotaryModul::setLevel(uint8_t _level) {
  cur_level = _level;
  rotary.setBoundaries(level[cur_level].min, level[cur_level].max, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotary.setEncoderValue(level[cur_level].cur); //preset the value to current gain
}

void RotaryModul::setValue(uint8_t _value) {
  level[cur_level].cur = _value;
  rotary.setEncoderValue(level[cur_level].cur); //preset the value to current gain
}

void RotaryModul::loop(time_t now) {
  // 0. Check reset of level
  if (cur_level != 0) {
    if (now - timeout_cnt > 60) {
      setLevel(0);
      isChanged = 2;
    }
  }
  // 1. Position changed
  if (rotary.encoderChanged()) {
    level[cur_level].cur = rotary.readEncoder();
    isChanged = 1;
  }
  /// 2. Button is pressed
  if (rotary.isEncoderButtonDown()) {
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
          if (cur_level != 0) timeout_cnt = now;
        } else {
          setLevel(0);
        }
        isChanged = 2;
      }
    }
  }
}


#endif
