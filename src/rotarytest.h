#ifdef USE_ROTARYTEST

#ifndef _ROTARYTEST_H_
#define _ROTARYTEST_H_

/***************************************************************************************
 ***************************************************************************************/
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"

class RotaryTest : public Switch_OnOff {

public:
    /// @brief Die Initialisierung als logischer Schalter ohne HW-Pin
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword);
    
    void loop(time_t mytime);

};


#endif
#endif