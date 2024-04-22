#ifndef _WEBRADIO_H_
#define _WEBRADIO_H_
/***************************************************************************************
 ***************************************************************************************/
#include "switch_onoff.h"


/// @brief Ein abgeleitetes Objekt für einen Ein-Aus Schalter.\n 
///Der Schalter reagiert nur wenn es in der **set** Funktion mit dem Schlüsselwort aufgerufen 
///wurde das bei der Initialisierung hinterlegt wurde.\n 
///Folgende Strings als **value** übergeben schalten **aus**:\n 
///**0** **aus** **Aus** **off** **Off**\n 
///Folgende Strings als **value** übergeben schalten **ein**:\n 
///**1** **ein** **Ein** **on** **On**\n 
///Folgende Strings als **value** übergeben schalten **um**:\n 
///**2** **umschalten** **Umschalten** **toggle** **Toggle**\n 


class Webradio : public Switch_OnOff {

public:
    /// @brief Die Initialisierung als logischer Schalter ohne HW-Pin
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword);
    void loop();
    /// @brief Erzeugt einen JSON zur Initialisierung innerhalb der Weboberfläche
    /// @param json Nimmt den JSON String zur Initialisierung auf
    //void html_create_json_part(String& json);
    /// @brief Die normale Set Funktion aus de Grundmodul erweitert um feste Schlüsselwörter 
    /// @brief zur Einspeicherung von Radiostationen und zur Senderwahl.
    /// @brief station[0..9]_url; station[0..9]_name; play;
    /// @param keyword Das zu testende "keyword"
    /// @param value Folgende Strings als **value** übergeben schalten **aus**:\n 
    /// @return "true" bei Übereinstimmung der Keywörter sonst false
    bool set(const String& keyword, const String& value);

    
private:
    void webradio_off();
    void webradio_on();
    void set_vol();
    void set_station();
    void show_station();
    void save_station();
    uint8_t cur_vol = 10;   // Die aktuelle Lautstärke von 0 bis 100 Prozent
    uint8_t slider_val_old;
    bool    mystate;
};

//#include "SPI.h"  //Wird nur bei Software SPI benötigt!
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define ARC_SIGMENT_DEGREES 3
#define ARC_WIDTH 5


class RadioDisplay {
public:
  void begin(Adafruit_GC9A01A* mytft);
  void clear();
  void show_ip(const char* myip);
  void show_bps(const char* mybps);
  void show_vol(uint8_t vol);
  void show_station(const char* mystation);
  void show_title(const char* mytitle);
  void show_time(bool big);
  void select_station(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4);

private:
  void fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour);
  int splitStr(const char* inStr, int startPos, int maxLen, char* resultStr);
  void show_text(const char* mytext, int posx, int posy, uint16_t color);
  
  Adafruit_GC9A01A* tft;
};

#endif
