#ifndef _AUDIOMODUL_H_
#define _AUDIOMODUL_H_

#include "config.h"
#include "switch_onoff.h"
//#include "SD.h"
#include "Audio.h"
#ifdef USE_DISPLAY_GC9A01A
#include "audiodisplay_GC9A01A.h"
#endif
#ifdef USE_ROTARY
#include "AiEsp32RotaryExtention.h"
#endif

#define MODE_OFF            0
#define MODE_RADIO          1
#define MODE_MEDIA          2
#define MODE_SPEAKER        3
#define MODE_SETTINGS       4
#define MODE_MUSIC_UPDATE   5
#define MODE_RADIO_SEL      6


// Settings for Webradio-Stations definitions
#define MAXSTATIONS               10
#define STATION_NAME_LENGTH       30
#define STATION_URL_LENGTH        128
// Settings for Mediaplayer
#define SD_DIR_LENGTH             50
#define SD_FILE_LENGTH            50

#if defined(CONFIG_IDF_TARGET_ESP32) 
#ifdef USE_DISPLAY_GC9A01A
//#warning "Compiling Display GC9A01A with Settings for ESP32"
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
#ifndef TFT_RES
#define TFT_RES                 13
#endif
#endif //USE_Display_GC9A01A

#ifdef USE_AUDIO_MEDIA
#ifndef SD_CS
#define SD_CS                   5
#endif
#ifndef SD_SCK
#define SD_SCK                  18
#endif
#ifndef SD_MOSI
#define SD_MOSI                 23
#endif
#ifndef SD_MISO
#define SD_MISO                 19
#endif
#endif  //Use_AUDIO_MEDIA / SD-Card

#ifdef USE_ROTARY
#ifndef ROT_SW
#define ROT_SW                  33
#endif
#ifndef ROT_S1
#define ROT_S1                  34
#endif
#ifndef ROT_S2
#define ROT_S2                  35
#endif
#endif //USE_Rotary

#define I2S_BCLK                27
#define I2S_LRC                 26
#define I2S_DOUT                25
#endif  //CONFIG_IDF_TARGET_ESP32

#ifdef CONFIG_IDF_TARGET_ESP32S3
#ifdef USE_DISPLAY_GC9A01A
//#warning "Compiling Display GC9A01A with Settings for ESP32-S3"
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
#ifndef TFT_RES
#define TFT_RES                 -1
#endif
#endif //USE_Display_GC9A01A

#ifdef USE_AUDIO_MEDIA
#ifndef SD_CS
#define SD_CS                   10
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
#ifndef ROTARY_ENCODER_VCC_PIN
#define ROTARY_ENCODER_VCC_PIN     -1
#endif
#ifndef ROTARY_ENCODER_STEPS
#define ROTARY_ENCODER_STEPS       4
#endif
#ifndef ROTARY_ENCODER_R_PULLDOWN
#define ROTARY_ENCODER_R_PULLDOWN  false
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

void my_audio_info(Audio::msg_t m);

/// @brief Das Audiomodul ist eine Ableitung der Klasse "Switch_OnOff" 
class AudioModul : public Switch_OnOff {

public:
    /// @brief Die Initialisierung als logischer Schalter ohne HW-Pin
    /// @param html_place Der Einbauort in der Webseite
    /// @param label Ein Bezeichner für diesen Schalter
    /// @param mqtt_name Der Bezeichner in MQTT für diesen Schalter
    /// @param keyword Das Schlüsselword auf das dieser Schalter reagiert
    /// @param _show_diagramm True wenn ein 24 Stunden Zeitdiagramm über den Zustand des Schalters angezeigt werden soll, sonst false.
    void begin(const char* html_place, const char* label, const char* keyword, bool _show_diagramm);

    /// @brief Die normale Set Funktion aus dem Grundmodul erweitert um feste Schlüsselwörter 
    /// @brief zur Steuerung dieses Audiomodules..
    /// @brief station[0..9]_url; station[0..9]_name; play; ... <ToDo>
    /// @param _cmnd Das zu testende "keyword"
    /// @param _val Die übergebenen Befehlsinhalte. 
    /// @return "true" bei Übereinstimmung der Keywörter sonst false
    bool set(const String& _cmnd, const String& _val);

    /// @brief Die Loopfunktion für den eriodischen Aufruf.
    /// @param now Der Unix Zeitstempel. 
    void loop(time_t now);
    
    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_info(String& _html_info);

    /**
     * @brief Initialisierung einer Webseite
     * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
     * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
     * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
     */
    void html_init(String& _html_init);

    /**
     * @brief Updatedaten für die Webseite
     * Wenn sich der Inhalt der Webseite ändert, werden hier die geänderten Daten in Form eines Teil-JSON bereitgestellt.
     * Durch das zugrunde liegende Event wird die Variable html_update_set auf true gesetzt, damit das Hauptprogramm 
     * weiß, dass es neue Daten gibt. Das Hauptprogramm sendet diese Daten dann als Websocket an den Browser.
     */
    void html_update(String& _html_update);

    /**
     * @brief Sollte es in diesem Modul telemetrieähnliche Daten geben, werden diese hier als Teil-JSON eingetragen
     */
    void mqtt_info(String& _mqtt_info);

    /**
     * @brief Der MQTT Status
     * Dieser String muss durch das abgeleitete Objekt gefüllt werden. Dabei gilt für jeden Messwert:
     * "mqtt_nameX"+":"+"MesswertX",...
     * Hier steht immer ein abgeschlossenes Teil-JSON ohne Klammern.
     */
    void mqtt_stat(String& _mqtt_stat);

    /// @brief Der Name des aktuellen Streams / Sender
    String radio_stationname;
    /// @brief Der Streamtitle des aktuellen Streams / Sender
    String radio_streamtitle;
   /// @brief Die aktuelle BPS Rate 
    String bps;

    bool song_eof = false;

    bool update_set = false;
    bool display_update_set = false;
    bool display_update_vol = false;
    bool display_update_now = false;

#ifdef USE_DISPLAY_GC9A01A
    AudioDisplay*  display;
#endif
#ifdef USE_ROTARY
    AiEsp32RotaryExtention*  rotary;
#endif

    /// @brief Der Name des aktuellen Künstlers / Gruppe
    String media_artist_name;
    /// @brief Der Name des aktuellen Files / Name des Liedes
    String media_song_name;
    /// @brief Der Name des aktuellen Ordners / Name des Albums
    String media_album_name;
    /// @brief Der aktuelle Modus des Audiomoduls, z.B. MODE_RADIO, ....
    uint8_t mode;
    /// @brief Die aktuelle Lautstärke
    uint8_t vol;

private:

    uint8_t last_mode;
    uint8_t default_mode;

    bool change_from_rotary;

    bool mode_changed = false;
/*    
    uint16_t   bas;
    uint16_t   tre;
*/
    void set_mode(uint8_t new_mode);
    void off();
    void on();
    void start_timeout();
    bool timeout_set;
    time_t timeout_start;
 
    Audio*            audio;

#ifdef USE_ROTARY
    uint8_t this_app;
    uint8_t this_lev;
    uint16_t rot_last_val;
//    AiEsp32RotaryExtention*  rotary;
#endif

#ifdef USE_AUDIO_RADIO

// private Funktionen für das Radio

    /// @brief Schaltet das Radio an und spielt den aktuellen Sender.
    void radio_on();

    /// @brief Schaltet das Radio aus.
    void radio_off();

    /// @brief Sendet die Senderliste an die Weboberfläche.
    void radio_send_stn2web();

    /// @brief Lädt die Textdatei data/sender.txt in das Array audio_radio_station[]
    void radio_load_stations();

    void radio_save_stations();

// private Variablen für das Radio

    /// @brief Ein Array mit den Sendern
    station_t radio_station[MAXSTATIONS];

    /// @brief Der aktuell ausgewählte Sender, entspricht der Indexnummer im Array.
    uint8_t   radio_cur_station;

#endif //USE_AUDIO_RADIO

#ifdef USE_AUDIO_MEDIA

    /// @brief Das aktuelle Album
    uint16_t media_cur_album = 1;
    /// @brief Das aktuelle Lied
    uint16_t media_cur_song = 0;
    /// @brief Das aktuelle Album
    uint16_t media_sel_album = 0;
    /// @brief Das aktuelle Lied
    uint16_t media_sel_song = 0;

    time_t    song_starttime;
    bool      song_started = false;

    bool media_changemode = false;

    void media_get_album_for_web();

    void media_get_songs_for_web(uint16_t reqDirNo);
    /// @brief Schaltet den Mediaplayer an.
    void media_on();
    /// @brief Schaltet den Mediaplayer aus.
    void media_off();
    
    void media_play(uint16_t _albumNo, uint16_t _songNo);

    /**
     * Wird auf "true" gesetz wenn ein "Media update" durchgeführt werden soll.
     */
    bool media_do_update = false;

    /**
     * @brief: Hier wird je Album(Ordner) die Datei songs.txt mit den Albumtiteln gefüllt.
     */

    bool media_sd_init_songs = false;

    /**
     * @brief Hier wird für alle Alben die Datei album.txt mit den Verzeichnisnamen gefüllt.
     */

    bool media_sd_init_album = false;

 //   bool getAlbumByNumber(fs::FS &fs, uint16_t albumNo);
    bool getSongByNumber(fs::FS &fs, uint16_t albumNo, uint16_t songNo);

    /**
     * Wird auf "true" gesetz wenn ein "Media update" durchgeführt werden soll.
     */
    bool  media_update_running = false;
    char* media_update_lowstr;
    char* media_update_highstr;
    bool  media_update_found = false;
    bool  media_update_outfile = false;
    File  sd_root;
    File  sd_dir;
    File  sd_out;

    /**
     * @brief Die zentrale Sortierfunktion.
     * @param s0 Die unter Grenze, kann entweder leer sein oder den kleinsten String beinhalten
     * @param s1 Die obere Grenze, kann entweder leer sein oder den größten String beinhalten
     * @param s2 Der zu testende String
     * @return "true" wenn s2 zwischen s0 und s1 liegt, sonst "false"
     */
    bool media_sort(const char* s0, const char* s1, const char* s2);


    void initMedia();

    void convert_jpg_files();
#endif //USE_AUDIO_MEDIA

};

#endif  //_AUDIOMODUL_H_