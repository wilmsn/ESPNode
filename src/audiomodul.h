#ifdef USE_AUDIOMODUL

#ifndef _AUDIOMODUL_H_
#define _AUDIOMODUL_H_

/***************************************************************************************





Klickstream (Modus und Level):

1) Kurzer Klick
Im Startzustand wird das Radio (wenn USE_AUDIO_RADIO gesetzt) gestartet.
Modus = Off     Level = 0 Pos = (0 bis 5)   ==> Anlage aus
                Level = 0 Pos > 5           ==> Anlage ein -> Radio
Modus = Radio   Level = 0 Pos = beliebig    ==> Radio: Sender wird gespielt (Lautstärkeregelung)
                Level = 1 Pos = beliebig    ==> Radio: Senderwahl; Nach 60 Sek Sprung zu Level 0
Modus = Media   Level = 0 Pos = beliebig    ==> Media: Musikstück wird gespielt (Lautstärkeregelung)
                Level = 1 Pos = beliebig    ==> Media: Albenwahl; Nach 60 Sek Sprung zu Level 0
                Level = 2 Pos = beliebig    ==> Media: Musikstückwahl; Nach 60 Sek Sprung zu Level 0
Modus = Speaker Level = 0 Pos = beliebig    ==> Speaker: Musikstück wird gespielt (Lautstärkeregelung)

2) Langer Klick
Ein langer Klick ist unabhängig von Modus und Level und startet das Auswahlmenü
Modus = Choose  Level = 0 Pos = beliebig    ==> Modus wird ausgewählt; Nach 60 Sek Sprung zum alten Modus
                Level = 1 Pos = beliebig    ==> Modus wird gewechselt





 ***************************************************************************************/
#define USE_ROTARYMODUL
#include "rotarymodul.h"
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_AUDIODISPLAY
#include "audiodisplay.h"
#ifdef USE_MEDIAPLAYER
#include "FS.h"
#include "SD.h"
#endif

// Settings for general use
#define TIMEOUT                   60
// Settings for Webradio-Stations definitions
#define MAXSTATIONS               10
#define STATION_NAME_LENGTH       30
#define STATION_URL_LENGTH        128
// Settings for Mediaplayer
#define MAXALBUM                  100
#define ALBUMLENGTH               40
#define SONGLENGTH                40

typedef struct {
    char    name[STATION_NAME_LENGTH];
    char    url[STATION_URL_LENGTH];
} station_t;

typedef enum Modus { Off = 0, 
#ifdef USE_AUDIO_RADIO
    Radio, 
#endif 
#ifdef USE_AUDIO_MEDIA
    Media,
#endif
#ifdef USE_AUDIO_SPEAKER
    Speaker,
#endif
     Settings, Select, modus_count } modus_t;

// Die Keywordliste auf die in der "set" Funktion reagiert wird:
#define AUDIO_RADIO_SET_STATION "station"
#define AUDIO_RADIO             "radio"
#define AUDIO_MEDIA             "media"
#define AUDIO_SPEAKER           "speaker"


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

    void loop(time_t now);

private:

#ifdef USE_AUDIO_RADIO
    void audio_radio_off();
 
    void audio_radio_on();

    void audio_radio_select();

    void audio_radio_set_station();

    void audio_radio_show();

    void audio_radio_save_stations();
    
    void audio_radio_load_stations();

    void audio_radio_send_stn2web();
#endif
 //   void audio_radio_station_json(String& json);
#ifdef USE_AUDIO_MEDIA

    void audio_media_off();
 
    void audio_media_on();

    void audio_media_sel_file(File& dir, File& file, uint8_t count);
//    void audio_media_file_from_dir(File mydir, File *myfile, uint8_t fileNo);
    File audio_media_file_from_dir(File mydir, uint8_t fileNo);

    void audio_media_select_dir();

    void audio_media_select_file();

    void audio_media_show();

    void audio_media_play(uint8_t _dirNo, uint8_t _fileNo);

    uint16_t audio_media_num_dir = 0;
#endif
#ifdef USE_AUDIO_SPEAKER

    void audio_speak_off();
 
    void audio_speak_on();

    void audio_speak_show();
#endif
    void      audio_set_modus(modus_t _modus);
    void      audio_show_modus(modus_t _modus);
    void      audio_all_apps_off();
    modus_t   modus;
    modus_t   last_modus;
    bool      time_update = false;
    uint8_t   audio_vol;
    uint8_t   audio_bas;
    uint8_t   audio_tre;
    bool      timeout_set = false;
    time_t    timeout_start;
// Radio
#ifdef USE_AUDIO_RADIO
    station_t station[MAXSTATIONS];
    uint8_t   audio_radio_cur_station;
#endif
// Mediaplayer
#ifdef USE_AUDIO_MEDIA
    uint8_t   audio_media_cur_dir  = 0;
    uint8_t   audio_media_cur_file = 0;
    uint8_t   audio_media_num_file = 0;
#endif
// Bluetoothspeaker
#ifdef USE_AUDIO_SPEAKER

#endif
// Generic use
    int       old_min = 0;
};


#endif
#endif