#ifndef _ACTOR_NEOPIXEL_H_
#define _ACTOR_NEOPIXEL_H_
// ESP32S3 RGB LED auf Pin 48
#include "switch_onoff.h"
#include <Adafruit_NeoPixel.h>

class Actor_NeoPixel : public Switch_OnOff {

public:
    /**
     * @brief Initialisierung des NeoPixel Actors
     * @param _gpio Der GPIO Pin für die Neopixels
     * @param _num_led Anzahl der Neopixel LED in der Reihe
     * @param _html_place_rt Der Einbauort des Rot Schalters in der Webseite
     * @param _html_place_ge Der Einbauort des Gelb Schalters in der Webseite
     * @param _html_place_bl Der Einbauort des Blau Schalters in der Webseite
     * 
     */
     void begin(const char* _html_place, const char* _html_label, const char* _keyword,
               int _gpio, int _num_led); 

    /**
     * @brief Der Komandointerpreter.
     * Hier als leere Funktion implementiert, kann im abgeleiteten Objekt gefüllt werden.
     * Über die Set Funktion werden alle Kommandos gescheusst. Das Modul entscheidet selber ob und wie es
     * auf das übergebene Keywort reagiert.
     * @return true wenn das Modul für den übergebenen Befehl zuständig ist, sonst false.
     */
    bool set(const String& _cmnd, const String& _val);

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
     void html_init(String& _html_init);

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_info(String& _html_init);

    /**
     * @brief Update einer Webseite
     * Wenn sich der Inhalt einer Webseite ändert wird hier der geänderte Inhalt bereitgestellt.
     */
    void html_update(String& _html_update);


private:
  Adafruit_NeoPixel* strip;
  uint32_t color;
  int num_pixels;
  int gpio;

};

#endif