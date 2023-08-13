#ifndef _RADIO_H_
#define _RADIO_H_
/***************************************************************************************
 ***************************************************************************************/
#include "switch_onoff.h"

/// @brief tbd


class Radio : public Switch_OnOff {

public:
    void begin(const char* html_place, const char* label);
    void loop(void);
private:

};

#endif