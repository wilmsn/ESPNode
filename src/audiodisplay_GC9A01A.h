#ifdef USE_DISPLAY_GC9A01A

#ifndef _AUDIODISPLAY_GC9A01A_H_
#define _AUDIODISPLAY_GC9A01A_H_

#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#define ARC_SIGMENT_DEGREES 3
#define ARC_WIDTH 5

#define TEXT_UNDER_BMP_X  30
#define TEXT_UNDER_BMP_Y  140
#define BMP_LEFT          70
#define BMP_DOWN          50
#define IP_POS_X          75
#define IP_POS_Y          225
#define IP_FONTSIZE       1

extern uint16_t* bmpBuffer;

/**
 * @brief Anzeige für das Audiomodul spezialisiert auf das Display GC9A01A
 */
class AudioDisplay : public Adafruit_GC9A01A {
public:
  /// @brief Constructor für das Anzeigeobjekt
  /// @param _cs Der CS Pin
  /// @param _dc der DC Pin
  /// @param _rot Die Rotation
  AudioDisplay(int8_t _cs, int8_t _dc, int8_t _rst, uint8_t _rot);
  /// @brief Initialisiert die Anzeige
  void begin();
  /// @brief Die Loop Funktion wird regelmäßig aufgerufen
  /// @param now Der Unix Zeitstempel
  void loop(time_t now);
  /// @brief clear() löscht alle Inhalte auf der Anzeige
  void clear();
  String html_info;
  /// @brief Setzt die Lautstärkeanzeige auf einen neuen Wert
  /// @param vol Die anzuzeigende Lautstärke
  void vol(uint8_t vol);
  /// @brief Schaltet die Anzeige in den "Aus" Modus
  void screen_off();
  /// @brief Schaltet die Anzeige in den "Radio" Modus
  void screen_radio();
  /// @brief Schaltet die Anzeige in den "Radio Senderwahl" Modus
  void screen_radio_select();
  /// @brief Schaltet die Anzeige in den "Mediaplayer" Modus
  void screen_media();
  /// @brief Schaltet die Anzeige in den "Media Update" Modus
  void screen_media_update();
  /// @brief Schaltet die Anzeige in den "Settings" Modus
  void screen_settings();
  /// @brief Anzeige der bps im Radiobetrieb
  /// @param mybps Ein c-String mit den bps Werten
  void radio_bps(const char* mybps);
  /// @brief Anzeige des eingestellten Senders
  /// @param Ein String mit dem Sender 
  void radio_station(const char* mystation);
  /// @brief Anzeige des übermittelten Streamtitles
  /// @param Ein String mit dem Streamtitle
  void radio_streamtitle(String& myplayinfo);
  void radio_select_station(const char* s0, const char* s1, const char* s2);
/// @brief Zeigt eine Bootmeldung auf dem Display an
/// @details Diese Funktion wird beim Booten des Gerätes aufgerufen, um eine Nachricht auf
/// dem Display anzuzeigen. Der Text wird in der angegebenen Farbe dargestellt. Dabei gilt folgende Festlegung:
/// - txtcolor: 0 = grau, 1 = grün, 2 = rot
/// @param txtcolor Die Textfarbe für die Bootmeldung
/// @param myMsg Die anzuzeigende Nachricht
/// @note Diese Funktion ist für die Anzeige auf dem Display zuständig und wird in der Regel
///       beim Start des Geräts aufgerufen, um den Benutzer über den Bootvorgang zu informieren.
  void bootMessage(uint8_t txtcolor, const char* msg, bool newline = true);
  void media_select_album(String& album, const uint16_t * pic);
  void media_select_song(String& album, String& song, const uint16_t * pic);
  void media_album(String& albumName);
  void media_artist(String& artistName);
  void media_song(String& songName);
  /// @brief Anzeige der bps im Mediabetrieb
  /// @param mybps Ein c-String mit den bps Werten
  void media_bps(String& mybps);
  void media_update();
  void show_media_bps();
  void show_media_album();
  void show_media_artist();
  void show_media_song();

/*
  // App Auswahl
  void show_modus(const char* modusStr);
  // App Mediaplayer
  void show_jpg(String& jpgFile);
  void select(const char* s0, const char* s1, const char* s2);
  void select(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4);
  void select(const char* s0, uint16_t * pic);
*/
/// @brief Ein ENUM für die verschiedenen Modi
enum class screenmode_t {
  Screen_Off = 0, 
  Screen_Radio, 
  Screen_RadioSel, 
  Screen_Media, 
  Screen_Speaker, 
  Screen_Settings, 
  Screen_MediaUpdate
};

enum screenmode_t cur_screen;


private:
  void clock_small();
  void clock_big();
  void clock_print();
  void ip();
  void fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour);
  // int splitStr(const char* inStr, int startPos, int maxLen, char* resultStr);
  //void show_text(String& in_text, int posx, int posy, uint16_t color);
  //void show_text_s2(const char* mytext, int posx, int posy, uint16_t color);
  //void showRadioStation();
  int last_min;
//  String radioStation;
//  String radioTitle;
  int getPartStringEnd(String data, int startAt, int minLen, int maxLen);
  void show_radio_streamtitle();
  void show_radio_station();
  void show_radio_bps();
  void show_vol();
  String replaceNonAscii(String inputString);

/// @brief Variable zur Steuerung der Anzeige Steaminhalte im Display

/// @brief Speichert den aktuelle Streamtitle damit dieser nach Displayänderung schnell dargestellt werden kann.
String cur_streamtitle;
String cur_station;
String cur_bps;
String cur_album;
String cur_artist;
String cur_song;
uint8_t cur_vol;
uint8_t boot_line = 0;
bool    boot_last_nl = true;
uint8_t rotation = 0;
};

#endif
#endif