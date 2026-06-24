#ifdef USE_AUDIODISPLAY_GC9A01A

#ifndef _AUDIODISPLAY_GC9A01A_H_
#define _AUDIODISPLAY_GC9A01A_H_

#define ARC_SIGMENT_DEGREES 3
#define ARC_WIDTH 5

#define TEXT_UNDER_BMP_X  30
#define TEXT_UNDER_BMP_Y  140
#define BMP_LEFT          70
#define BMP_DOWN          50
#define IP_POS_X          75
#define IP_POS_Y          215
#define IP_FONTSIZE       1
#define IP_COLOR          GC9A01A_WHITE
#define BPS_POS_X         90
#define BPS_POS_Y         230
#define BPS_FONTSIZE      1
#define BPS_COLOR         GC9A01A_RED
#define CLOCK_BIG_FONTSIZE 4
#define CLOCK_BIG_CURSOR_X 20
#define CLOCK_BIG_CURSOR_Y 100
#define CLOCK_SMALL_FONTSIZE 2
#define CLOCK_SMALL_CURSOR_X 80
#define CLOCK_SMALL_CURSOR_Y 30
#define FONT1_MAX_CHAR    10
#define FONT2_MIN_CHAR     7
#define FONT2_MAX_CHAR    17


extern uint16_t* bmpBuffer;

/**
 * @brief Anzeige für das Audiomodul spezialisiert auf das Display GC9A01A
 */
class AudioDisplay_GC9A01A {
public:

  /// @brief Initialisiert die Anzeige
//  void begin();
  /// @brief Die Loop Funktion wird regelmäßig aufgerufen
  /// @param now Der Unix Zeitstempel
  void loop(time_t now);

  /**
   * @brief Initialisierung einer Webseite
   * Wenn sich ein Browser verbindet und die Webseite des Nodes aufruft, wird diese Funtion durch das 
   * Hauptprogramm aufgerufen. Die Funktion stellt ein Teil-JSON mit allen Initialisierungsdaten in "html_json" 
   * bereit. Dieses sendet das Hauptprogramm mittels Message als Websocket an den Browser.
   */
  void html_info(String& _html_info);

  /// @brief clear() löscht alle Inhalte auf der Anzeige
  void clear();

  void update_display();

  void wipe_vol();  
  void show_vol(uint8_t cur_vol);  

/// @brief Zeigt eine Bootmeldung auf dem Display an
/// @details Diese Funktion wird beim Booten des Gerätes aufgerufen, um eine Nachricht auf
/// dem Display anzuzeigen. Der Text wird in der angegebenen Farbe dargestellt. Dabei gilt folgende Festlegung:
/// - txtcolor: 0 = grau, 1 = grün, 2 = rot
/// @param txtcolor Die Textfarbe für die Bootmeldung
/// @param myMsg Die anzuzeigende Nachricht

private:
  void clock_small();
  void clock_big();
  void clock_print();
  void fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour);
  String displaystr[3];
  uint8_t split4display(String& in_str);
  // int splitStr(const char* inStr, int startPos, int maxLen, char* resultStr);
  //void show_text(String& in_text, int posx, int posy, uint16_t color);
  //void show_text_s2(const char* mytext, int posx, int posy, uint16_t color);
  //void showRadioStation();
  int last_min;
//  String radioStation;
//  String radioTitle;
  int getPartStringEnd(String data, int startAt, int minLen, int maxLen);
  String replaceNonAscii(String inputString);

  uint8_t num_lines = 0;

};

#endif // _AUDIODISPLAY_GC9A01A_H_
#endif // USE_AUDIODISPLAY_GC9A01A