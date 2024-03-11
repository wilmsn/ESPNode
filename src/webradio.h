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
    
private:
    void webradio_off();
    void webradio_on();
    void set_vol();

    uint8_t myvol;
    uint8_t slider_val_old;
    bool    mystate;
};

#endif
