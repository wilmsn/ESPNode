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
    String audio_radio_stationname;
    /// @brief Der Streamtitle des aktuellen Streams / Sender
    String audio_radio_streamtitle;
   /// @brief Die aktuelle BPS Rate 
    String audio_kbs;

    bool song_eof = false;

#ifdef USE_DISPLAY_GC9A01A
    AudioDisplay*  display;
#endif
#ifdef USE_ROTARY
    AiEsp32RotaryExtention*  rotary;
#endif

private:
    /// @brief Die Variable "mode" ist zu jeder Zeit mit dem gerade aktiven "mode" gefüllt
    mymode_t   mode;
    /// @brief Der letzte verwendete "mode"
    mymode_t   last_mode = Radio;
    /// @brief Der letzte verwendete "mode" aus (Radio, Media und Speaker) wird hier gespeichert und nach dem Einschalten aktiviert.
    mymode_t   default_mode;

    bool change_from_rotary;

    bool mode_changed = false;
    /// @brief Die aktuelle Lautstärke
    uint16_t   audio_vol;
/*    
    uint16_t   audio_bas;
    uint16_t   audio_tre;
*/
    void audio_set_mode(mymode_t new_mode);
    void audio_off();
    String print_mode(mode_t mymode);
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

    /// @brief Schaltet das Radio an.
    void audio_radio_on();

    /// @brief Schaltet das Radio aus.
    void audio_radio_off();

    /// @brief Spielt den aktuellen Sender.
    void audio_radio_play();

    /// @brief Sendet die Senderliste an die Weboberfläche.
    void audio_radio_send_stn2web();

    /// @brief Lädt die Textdatei data/sender.txt in das Array audio_radio_station[]
    void audio_radio_load_stations();

    void audio_radio_save_stations();

// private Variablen für das Radio

    /// @brief Ein Array mit den Sendern
    station_t audio_radio_station[MAXSTATIONS];

    /// @brief Der aktuell ausgewählte Sender, entspricht der Indexnummer im Array.
    uint8_t   audio_radio_cur_station;

#endif //USE_AUDIO_RADIO

#ifdef USE_AUDIO_MEDIA

    /// @brief Der Name des aktuellen Ordners / Name des Albums
    String audio_media_album_name;
    /// @brief Der Name des aktuellen Künstlers / Gruppe
    String audio_media_artist_name;
    /// @brief Der Name des aktuellen Files / Name des Liedes
    String audio_media_song_name;
    /// @brief Das aktuelle Album
    uint16_t audio_media_cur_album = 1;
    /// @brief Das aktuelle Lied
    uint16_t audio_media_cur_song = 0;
    /// @brief Das aktuelle Album
    uint16_t audio_media_sel_album = 0;
    /// @brief Das aktuelle Lied
    uint16_t audio_media_sel_song = 0;

    time_t    song_starttime;
    bool      song_started = false;

    bool audio_media_changemode = false;

    void audio_media_get_album_for_web();

    void audio_media_get_songs_for_web(uint16_t reqDirNo);
    /// @brief Schaltet den Mediaplayer an.
    void audio_media_on();
    /// @brief Schaltet den Mediaplayer aus.
    void audio_media_off();
    
    void audio_media_play(uint16_t _albumNo, uint16_t _songNo);

    /**
     * Wird auf "true" gesetz wenn ein "Media update" durchgeführt werden soll.
     */
    bool audio_media_do_update = false;

    /**
     * @brief: Hier wird je Album(Ordner) die Datei songs.txt mit den Albumtiteln gefüllt.
     */

    bool audio_media_sd_init_songs = false;

    /**
     * @brief Hier wird für alle Alben die Datei album.txt mit den Verzeichnisnamen gefüllt.
     */

    bool audio_media_sd_init_album = false;

 //   bool getAlbumByNumber(fs::FS &fs, uint16_t albumNo);
    bool getSongByNumber(fs::FS &fs, uint16_t albumNo, uint16_t songNo);

    /**
     * Wird auf "true" gesetz wenn ein "Media update" durchgeführt werden soll.
     */
    bool audio_media_update_running = false;
    char* audio_media_update_lowstr;
    char* audio_media_update_highstr;
    bool audio_media_update_found = false;
    bool audio_media_update_outfile = false;
    File sd_root;
    File sd_dir;
    File sd_out;

    /**
     * @brief Die zentrale Sortierfunktion.
     * @param s0 Die unter Grenze, kann entweder leer sein oder den kleinsten String beinhalten
     * @param s1 Die obere Grenze, kann entweder leer sein oder den größten String beinhalten
     * @param s2 Der zu testende String
     * @return "true" wenn s2 zwischen s0 und s1 liegt, sonst "false"
     */
    bool audio_media_sort(const char* s0, const char* s1, const char* s2);


    void initMedia();

    void convert_jpg_files();
#endif //USE_AUDIO_MEDIA

};

#endif  //_AUDIOMODUL_H_