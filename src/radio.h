#ifndef _RADIO_H_
#define _RADIO_H_
/***************************************************************************************
 ***************************************************************************************/
#include "switch_onoff.h"

#define STATIONS_NUM          10
#define STATIONS_URL_SIZE     150
#define STATIONS_NAME_SIZE    32
#define STATIONMODE_TIMEOUT   15000

enum mymode_t {
  volumeMode = 0,
  stationMode,
  setupMode1,
  setupMode2
};

struct station_t {
  char url[STATIONS_URL_SIZE];
  char name[STATIONS_NAME_SIZE];
};


class Radio : public Switch_OnOff {

  public:
    void begin(const char* html_place, const char* label, const char* mqtt_name,  const char* keyword,
                         bool start_value, bool on_value, uint8_t slider_no,
                         const char* slider_mqtt_name);
    void loop(void);
    String info;
  private:
    void stationLayout();
    void radioLayout();
    station_t station[STATIONS_NUM];
    uint8_t curStation = 0;
    uint8_t tmpStation = 0;
    uint8_t curVol = 5;
    bool isOn = false;

};





#endif