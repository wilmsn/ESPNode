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
#ifdef USE_SDCARD
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

private:

    modus_t   modus;
    modus_t   last_modus;
    bool      time_update = false;
    uint8_t   audio_vol;
    uint8_t   audio_bas;
    uint8_t   audio_tre;
    bool      timeout_set = false;
    time_t    timeout_start;
    time_t    song_started;
// Radio
#ifdef USE_AUDIO_RADIO
    station_t station[MAXSTATIONS];
    uint8_t   audio_radio_cur_station;
#endif
// Mediaplayer
#ifdef USE_AUDIO_MEDIA
    uint16_t   audio_media_cur_dir  = 0;
    uint16_t   audio_media_cur_file = 0;
    uint32_t   audio_media_num_file = 0;

struct  music_t {
        uint16_t        dirNo;
        uint16_t        fileNo;
        char            dirName[50];
        char            fileName[50];
        music_t*        p_next;
};
music_t*                p_initial = NULL;


#endif
// Bluetoothspeaker
#ifdef USE_AUDIO_SPEAKER

#endif
// Generic use
    int       old_min = 0;
#ifdef USE_AUDIO_RADIO
    /// @brief Schaltet das Radio aus.
    void audio_radio_off();

    /// @brief Schaltet das Radio an.
    void audio_radio_on();

    /// @brief Die gesamte Initialisierung des Radios auf dem Display.
    void audio_radio_disp_init();

    /// @brief Die gesamte Initialisierung dieser Anwendung auf der Weboberfläche.
    void audio_radio_web_init();

    void audio_radio_select();

    void audio_radio_set_station();

//    void audio_radio_show();

    void audio_radio_save_stations();
    
    void audio_radio_load_stations();
#endif
 //   void audio_radio_station_json(String& json);
#ifdef USE_AUDIO_MEDIA
    /// @brief Schaltet den Mediaplayer aus.
    void audio_media_off();
 
    /// @brief Schaltet den Mediaplayer ein.
    void audio_media_on();

    /// @brief Die gesamte Initialisierung des Mediaplayers auf dem Display.
    void audio_media_disp_init();

    /// @brief Die gesamte Initialisierung des Mediaplayers auf der Weboberfläche.
    void audio_media_web_init();

    bool audio_media_sel_dir(File& root, File& dir, uint8_t count);
    
    bool audio_media_sel_file(File& dir, File& file, uint8_t count);

    void audio_media_get_album(uint16_t reqDirNo);

//    void audio_media_sel_file(File& dir, File& file, uint8_t count);

    File audio_media_file_from_dir(File mydir, uint8_t fileNo);

    void audio_media_select_album();
//    void audio_media_select_dir();

    void audio_media_select_song();
//    void audio_media_select_file();

 //   void audio_media_show();

    void audio_media_play(uint8_t _dirNo, uint8_t _fileNo);

    void newEntry(music_t* p_new);
    void addMusic(uint16_t dirNo, uint16_t fileNo, const char* dirName, const char* fileName);
    void allAlbum2Web();
    uint32_t countSongs();
    bool validSong(uint16_t dirNo, uint16_t fileNo);
    void nextSong();

    uint16_t audio_media_num_dir = 0;

    uint32_t audio_media_read_all();
#endif
#ifdef USE_AUDIO_SPEAKER

    void audio_speak_off();
 
    void audio_speak_on();

    void audio_speak_web_init();
#endif
    void      audio_set_modus(modus_t _modus);
    void      audio_show_modus(modus_t _modus);


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
 
    void html_create(String& tmpstr);

    void loop(time_t now);

};


#endif
#endif