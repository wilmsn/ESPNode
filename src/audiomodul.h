#ifdef USE_AUDIOMODUL

#ifndef _AUDIOMODUL_H_
#define _AUDIOMODUL_H_

/***************************************************************************************
 ***************************************************************************************/
#define USE_ROTARYMODUL
#include "rotarymodul.h"
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_AUDIODISPLAY
#include "audiodisplay.h"

// Settings for Webradio-Stations definitions
#define MAXSTATIONS               10
#define STATION_NAME_LENGTH       30
#define STATION_URL_LENGTH        128

typedef struct {
    char    name[STATION_NAME_LENGTH];
    char    url[STATION_URL_LENGTH];
} station_t;

typedef enum Modus { Radio, Media, Speaker, Off } modus_t;

class AudioModul : public Switch_OnOff {

public:
    /// @brief Die Initialisierung als logischer Schalter ohne HW-Pin
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword);
 
    /// @brief Die normale Set Funktion aus de Grundmodul erweitert um feste Schlüsselwörter 
    /// @brief zur Einspeicherung von Radiostationen und zur Senderwahl.
    /// @brief station[0..9]_url; station[0..9]_name; play;
    /// @param keyword Das zu testende "keyword"
    /// @param value Folgende Strings als **value** übergeben schalten **aus**:\n 
    /// @return "true" bei Übereinstimmung der Keywörter sonst false
    bool set(const String& keyword, const String& value);
 
    void html_create_json_part(String& json);

    void audio_off();
 
    void audio_on();

    void audio_radio_set_station();

    void audio_radio_show_station();

    void audio_radio_save_stations();
    
    void audio_radio_load_stations();

    void audio_radio_station_json(String& json);

    void loop(time_t now);

private:
    void      set_modus(uint8_t _modus);
    uint8_t   get_modus();
    station_t station[MAXSTATIONS];
    modus_t   modus;
    uint8_t   audio_vol;
    uint8_t   audio_bas;
    uint8_t   audio_tre;
    uint8_t   audio_radio_cur_station;
};


#endif
#endif