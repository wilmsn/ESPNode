#ifdef USE_AUDIODISPLAY_GC9A01A

#ifndef _AUDIODISPLAY_GC9A01A_H_
#define _AUDIODISPLAY_GC9A01A_H_

#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include "audiodisplay_bmps.h"

#define ARC_SIGMENT_DEGREES 3
#define ARC_WIDTH 5

#define TEXT_UNDER_BMP_X  30
#define TEXT_UNDER_BMP_Y  140
#define BMP_LEFT          70
#define BMP_DOWN          50
#define IP_POS_X          75
#define IP_POS_Y          225
#define IP_FONTSIZE       1

/**
 * @brief Anzeige für das Audiomodul spezialisiert auf das Display GC9A01A
 */
class AudioDisplay : public Adafruit_GC9A01A {
public:
  /// @brief Constructor für das Anzeigeobjekt
  /// @param _cs Der CS Pin
  /// @param _dc der DC Pin
  /// @param _rot Die Rotation
  AudioDisplay(int8_t _cs, int8_t _dc, uint8_t _rot);
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
  /// @brief Schaltet die Anzeige in den "Mediaplayer" Modus
  void screen_media();
  /// @brief Schaltet die Anzeige in den "Settings" Modus
  void screen_settings();
  /// @brief Anzeige der bps im Radiobetrieb
  /// @param mybps Ein c-String mit den bps Werten
  void radio_bps(const char* mybps);
  /// @brief Anzeige des eingestellten Senders
  /// @param Ein c-String mit dem Sender 
  void radio_station(const char* mystation);
  /// @brief Anzeige des übermittelten Streamtitles
  /// @param Ein c-String mit dem Streamtitle
  void radio_streamtitle(const char* myplayinfo);
  void radio_select_station(const char* s0, const char* s1, const char* s2);
  void screen_media_update();

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
enum class screenmode_t {Screen_Off = 0, Screen_Radio, Screen_Media, Screen_Speaker, Screen_Settings, Screen_MediaUpdate};
/*
enum  screenmode_t { 
  Screen_Off = 0, 
  Screen_Radio, 
  Screen_Media, 
  Screen_Speaker, 
  Screen_Settings, 
  Screen_MusicUpdate, 
  Screen_Last 
};
*/
enum screenmode_t cur_screen;

//  void screen(screenmode_t _screen);

private:
  void clock_small();
  void clock_big();
  void clock_print();
  void ip();
  void fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour);
  int splitStr(const char* inStr, int startPos, int maxLen, char* resultStr);
  void show_text(const char* mytext, int posx, int posy, uint16_t color);
  void show_text_s2(const char* mytext, int posx, int posy, uint16_t color);
  int last_min;
};

#endif
#endif