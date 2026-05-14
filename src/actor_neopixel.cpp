#include "config.h"
#include "actor_neopixel.h"

// TODO: Erweiterung auf mehr Pixel, Farbwahl, Effekte, ...
// Bis jetzt wird nur ein Pixel gesteuert, der entweder an oder aus ist. Die Farbe ist fest auf Weiß eingestellt. 
// Weitere Funktionen können über die Set Funktion und die Webseite ergänzt werden.


void Actor_NeoPixel::begin(const char* _html_place, const char* _html_label, const char* _keyword, int _gpio, int _num_led) {
  Switch_OnOff::begin(_html_place, _html_label, _keyword, false, true, true, false);
  this->num_pixels = _num_led;
  this->gpio = _gpio;
  this->strip = new Adafruit_NeoPixel(_num_led, _gpio, NEO_GRB + NEO_KHZ800);
  this->strip->begin();
  this->strip->clear();
//  this->strip->setPixelColor(0, this->strip->Color(255, 0, 0)); // Rot
  this->strip->show();

  // Initialisierung der Webseite
  this->html_init_set = true;
  this->html_info_set = true;
}

bool Actor_NeoPixel::set(const String& _cmnd, const String& _val) {
  bool retval = false;
  String myvalue = _val;
  if ( Switch_OnOff::set(_cmnd, _val) ) {
    if (switch_is_on) {
      this->color = 0xFFFFFF; // Weiß
      this->strip->setPixelColor(0, this->color); // Weiß
      this->strip->show();
    } else {
      this->strip->clear();
      this->strip->show();
    }
    retval = true;
  }
  return retval;
}

void Actor_NeoPixel::html_init(String& _html_init) {
  Switch_OnOff::html_init(_html_init);
  append_comma(_html_init);
  html_update(_html_init);
}

void Actor_NeoPixel::html_info(String& _html_info) {
  _html_info += String("\"tab_head_") + html_place + String("\":\"Neopixel\"") +
                String(",\"tab_line1_") + html_place + String("\":\"") + html_label + String(" (") + keyword + 
                String("):#GPIO:") + String(this->gpio) + String("\"");
}

void Actor_NeoPixel::html_update(String& _html_update) {
  Switch_OnOff::html_update(_html_update);
}

