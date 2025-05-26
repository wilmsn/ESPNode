#ifdef USE_AUDIOMODUL

#ifndef _AUDIOMODUL_H_
#define _AUDIOMODUL_H_

#define USE_SWITCH_ONOFF
#include "switch_onoff.h"

// Settings for Webradio-Stations definitions
#define MAXSTATIONS               10
#define STATION_NAME_LENGTH       30
#define STATION_URL_LENGTH        128



#if defined(CONFIG_IDF_TARGET_ESP32) 
#ifdef USE_AUDIODISPLAY_GC9A01A
#warning "Compiling Display GC9A01A with Settings for ESP32"
#ifndef TFT_SCK
#define TFT_SCK                 18
#endif
#ifndef TFT_MOSI
#define TFT_MOSI                23
#endif
#ifndef TFT_CS
#define TFT_CS                  17
#endif
#ifndef TFT_DC
#define TFT_DC                  16
#endif
#endif //USE_Display_GC9A01A

#ifdef USE_SDCARD
#ifndef SD_CS
#define SD_CS                   8
#endif
#ifndef SD_SCK
#define SD_SCK                  12
#endif
#ifndef SD_MOSI
#define SD_MOSI                 11
#endif
#ifndef SD_MISO
#define SD_MISO                 13
#endif
#endif  //Use_SDCard

#ifdef USE_ROTARY
#ifndef ROT_KEY
#define ROT_KEY                 35
#endif
#ifndef ROT_S!
#define ROT_S1                  36
#endif
#ifndef ROT_S2
#define ROT_S2                  37
#endif
#endif //USE_Rotary

#define I2S_BCLK                5
#define I2S_LRC                 4
#define I2S_DOUT                6
#endif  //CONFIG_IDF_TARGET_ESP32

#ifdef CONFIG_IDF_TARGET_ESP32S3
#ifdef USE_AUDIODISPLAY_GC9A01A
#warning "Compiling Display GC9A01A with Settings for ESP32-S3"
#ifndef TFT_SCK
// SCL
#define TFT_SCK                 12
#endif
#ifndef TFT_MOSI
// SDA
#define TFT_MOSI                11
#endif
#ifndef TFT_CS
#define TFT_CS                  8
#endif
#ifndef TFT_DC
#define TFT_DC                  9
#endif
#ifndef TFT_ROT
#define TFT_ROT                 3
#endif
#endif //USE_Display_GC9A01A

#ifdef USE_SDCARD
#ifndef SD_CS
#define SD_CS                   8
#endif
#ifndef SD_SCK
#define SD_SCK                  12
#endif
#ifndef SD_MOSI
#define SD_MOSI                 11
#endif
#ifndef SD_MISO
#define SD_MISO                 13
#endif
#endif  //Use_SDCard

#ifdef USE_ROTARY
#ifndef ROT_SW
#define ROT_SW                  3
#endif
#ifndef ROT_S1
#define ROT_S1                  1
#endif
#ifndef ROT_S2
#define ROT_S2                  2
#endif
#endif //USE_Rotary

#define I2S_BCLK                5
#define I2S_LRC                 4
#define I2S_DOUT                6
#endif  //CONFIG_IDF_TARGET_ESP32S3


/// @brief Eine Struktur für die Radiosender
typedef struct {
    /// @brief Der Sendername so wie er im Display angezeigt wird
    char    name[STATION_NAME_LENGTH];
    /// @brief Die URL zum Stream des Senders
    char    url[STATION_URL_LENGTH];
} station_t;

/// @brief Ein ENUM für die verschiedenen Modi
typedef enum { 
    Off = 0, 
    Radio, 
    Media, 
    Speaker, 
    Settings, 
    MusicUpdate, 
    LastApp 
} mymode_t;







/// @brief Das Audiomodul ist eine Ableitung der Klasse "Switch_OnOff" 
class AudioModul : public Switch_OnOff {

public:
    /// @brief Die Initialisierung als logischer Schalter ohne HW-Pin
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword);
    bool set(const String& _cmnd, const String& _val);
    void html_init();    
    void loop(time_t now);
private:
    /// @brief Die Variable "mode" ist zu jeder Zeit mit dem gerade aktiven "mode" gefüllt
    mymode_t   mode;
    /// @brief Der letzte verwendete "mode"
    mymode_t   last_mode;
    /// @brief Der letzte verwendete "mode" aus (Radio, Media und Speaker) wird hier gespeichert und nach dem Einschalten aktiviert.
    mymode_t   default_mode;

    bool change_from_rotary;

    bool mode_changed = false;
    /// @brief Die aktuelle Lautstärke
    uint16_t   audio_vol;
#ifdef USE_ROTARY
    uint16_t rot_last_val;
#endif
/*    
    uint16_t   audio_bas;
    uint16_t   audio_tre;
*/
    void html_upd_data();
    void audio_set_mode(mymode_t new_mode);
    void audio_off();
    String print_mode(mode_t mymode);
    void start_timeout();
    bool timeout_set;
    time_t timeout_start;
  

#ifdef USE_AUDIO_RADIO

    /// @brief Ein Array mit den Sendern
    station_t audio_radio_station[MAXSTATIONS];
    /// @brief Der aktuell ausgewählte Sender
    uint8_t   audio_radio_cur_station;

    /// @brief Schaltet das Radio an.
    void audio_radio_on();

    /// @brief Schaltet das Radio aus.
    void audio_radio_off();

    /// @brief Lädt die Textdatei data/sender.txt in das Array audio_radio_station[]
    void audio_radio_load_stations();

    void audio_radio_save_stations();

#endif


};




#endif  //_AUDIOMODUL_H_
#endif  //USE_AUDIOMODUL