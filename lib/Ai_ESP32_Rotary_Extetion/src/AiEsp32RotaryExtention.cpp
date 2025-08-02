#include "AiEsp32RotaryExtention.h"

/**************************************
 * Multi App and Lev Extention
**************************************/

AiEsp32RotaryExtention::AiEsp32RotaryExtention(uint8_t encoderAPin, uint8_t encoderBPin, uint8_t encoderButtonPin, int encoderVccPin, uint8_t encoderSteps, bool areEncoderPinsPulldown_forEsp32)
//  :  AiEsp32RotaryEncoder(encoderAPin, encoderBPin, encoderButtonPin, encoderVccPin, encoderSteps, areEncoderPinsPulldown_forEsp32) {
  :  AiEsp32RotaryEncoder(encoderAPin, encoderBPin, encoderButtonPin, encoderVccPin, encoderSteps, areEncoderPinsPulldown_forEsp32) {
  p_app_initial = (app_t*)malloc(sizeof(app_t));
  p_app_initial->p_app_next = NULL;
  p_app_initial->app = 0;
  p_app_initial->lev = 0;
  p_app_initial->min = 0;
  p_app_initial->max = 100;
  p_app_initial->val = 0;
  p_app_current = p_app_initial;
}

void AiEsp32RotaryExtention::loop(time_t now) {
  // 0. Check reset of level
/*  if (cur_level != 0) {
    if (now - timeout_cnt > 60) {
      setLevel(0);
      isChanged = 2;
    }
  }*/
  /// 2. Button is pressed
  if (isEncoderButtonDown()) {
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
      if (millis() - millis_Button_pressed <= SHORT_PRESSED_AFTER_MS) {
//      to short for a short click
        buttonShortPressed = false;
        buttonLongPressed  = false;
      } else {
        if (millis() - millis_Button_pressed <= LONG_PRESSED_AFTER_MS) {
//        short click
          buttonShortPressed = true;
        } else {
//        long click
          buttonLongPressed = true;
        }
      }
    }
  }
}

uint8_t AiEsp32RotaryExtention::get_max_app(){
  app_t* p_app_search = p_app_initial;
  uint8_t app_max = 0;
  while (p_app_search) {
    if (p_app_search->app > app_max) app_max = p_app_search->app;
    p_app_search = p_app_search->p_app_next;
  }
  return app_max;
}

uint8_t AiEsp32RotaryExtention::get_max_lev(uint8_t app){
  app_t* p_app_search = p_app_initial;
  uint8_t lev_max = 0;
  while (p_app_search) {
    if (p_app_search->app == app && p_app_search->lev > lev_max) lev_max = p_app_search->lev;
    p_app_search = p_app_search->p_app_next;
  }
  return lev_max;
}

uint8_t AiEsp32RotaryExtention::app_add(){
  return app_add(0,0,0);
}

uint8_t AiEsp32RotaryExtention::app_add(uint16_t _min, uint16_t _max, uint16_t _val){
  //Add a new record for App
  uint8_t max_app = get_max_app();
  app_t* p_app_search = p_app_initial;
  while (p_app_search->p_app_next) p_app_search = p_app_search->p_app_next;
  app_t* p_app_new = (app_t*)malloc(sizeof(app_t));
  p_app_new->p_app_next = NULL;
  p_app_new->app = max_app + 1;
  p_app_new->lev = 0;
  p_app_new->min = _min;
  p_app_new->max = _max;
  p_app_new->val = _val;
  p_app_search->p_app_next = p_app_new;
  return max_app + 1;
}

uint8_t AiEsp32RotaryExtention::lev_add(uint8_t _app){
  return lev_add(_app,0,0,0);
}

uint8_t AiEsp32RotaryExtention::lev_add(uint8_t _app, uint16_t _min, uint16_t _max, uint16_t _val){
  app_t* p_app_search = p_app_initial;
  uint8_t max_lev = get_max_lev(_app);
  //Add a new record for this Lev
  while (p_app_search->p_app_next) p_app_search = p_app_search->p_app_next;
  app_t* p_app_new = new app_t;
  p_app_new->p_app_next = NULL;
  p_app_new->app = _app;
  p_app_new->lev = max_lev + 1;
  p_app_new->min = _min;
  p_app_new->max = _max;
  p_app_new->val = _val;
  p_app_search->p_app_next = p_app_new;
  return max_lev + 1;
}

bool AiEsp32RotaryExtention::app_set(uint8_t _app, uint8_t _lev, uint16_t _min, uint16_t _max, uint16_t _val) {
  bool retval = false;
  app_t* p_app_search = p_app_initial;
  while (p_app_search) {
    if ( p_app_search->app == _app && p_app_search->lev == _lev ) {
      p_app_search->min = _min;
      p_app_search->max = _max;
      p_app_search->val = _val;   
      retval = true;
      p_app_current = p_app_search;
    }
    p_app_search = p_app_search->p_app_next;
  }
  max_set(_max);
  min_set(_min);
  val_set(_val);
  return retval;
}

bool AiEsp32RotaryExtention::app_set(uint8_t _app, uint8_t _lev){
  bool retval = false;
  app_t* p_app_search = p_app_initial;
  while (p_app_search) {
    if ( p_app_search->app == _app && p_app_search->lev == _lev ) {
      retval = true;
      p_app_current = p_app_search;
    }
    p_app_search = p_app_search->p_app_next;
  }
  max_set(p_app_current->max);
  min_set(p_app_current->min);
  val_set(p_app_current->val);
  return retval;
}

void AiEsp32RotaryExtention::max_set(uint16_t _max) {
  p_app_current->max = _max;
  setBoundaries(p_app_current->min, p_app_current->max);
}

void AiEsp32RotaryExtention::min_set(uint16_t _min) {
  p_app_current->min = _min;
  setBoundaries(p_app_current->min, p_app_current->max);
}

void AiEsp32RotaryExtention::val_set(uint16_t _val) {
  p_app_current->val = _val;
  setEncoderValue(p_app_current->val);
}

uint8_t AiEsp32RotaryExtention::lev_up() {
  bool success = false;
  uint8_t cur_app = p_app_current->app;
  uint8_t cur_lev = p_app_current->lev;
  if (cur_lev < get_max_lev(cur_app)) {
    if (app_set(cur_app, cur_lev+1)) {
      success = true;
    }
  } else {
    if (app_set(cur_app, 0)) {
      success = true;
    }
  }
  if (success) {
    return lev();
  } else {
    return 255;
  }
}

uint8_t AiEsp32RotaryExtention::app_up(){
  bool success = false;
  uint8_t cur_app = p_app_current->app;
  if (cur_app < get_max_app()) {
    if (app_set(cur_app+1, 0)) success = true;
  } else {
    if (app_set(0,0)) success = true;
  }
  if (success) {
    return cur_app;
  } else {
    return 255;
  }
}

uint8_t AiEsp32RotaryExtention::lev(){
  return p_app_current->lev;
}

uint8_t AiEsp32RotaryExtention::app(){
  return p_app_current->app;
}

uint16_t AiEsp32RotaryExtention::val(){
  uint16_t cur_val = readEncoder();
  p_app_current->val = cur_val;
  return cur_val;
}

uint16_t AiEsp32RotaryExtention::min(){
  return p_app_current->min;
}

uint16_t AiEsp32RotaryExtention::max(){
  return p_app_current->max;
}

bool AiEsp32RotaryExtention::valChanged(){
  return encoderChanged();
}

