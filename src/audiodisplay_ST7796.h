#ifndef _AUDIODISPLAY_ST7796S_H_
#define _AUDIODISPLAY_ST7796S_H_

#include "Adafruit_GFX.h"
#include "Adafruit_ST7796S.h"
/*
Layout der Anzeige:
=======================================================================
|                           Uhr                                       | 
|                                                                     |
|   Sendername                                                      V |
|                                                                   O |
|                                                                   L |
|   Aktueller Titel                                                 U |
|                                                                   M |
|                                                                   E |
|                                                                     |
|  IP Adresse                                              BPS        |
=======================================================================


*/
//some colors
#define COLOR_BLACK     0b1111111111111111
#define COLOR_WHITE     0b0000000000000000
#define COLOR_RED       0b0000011111111111
#define COLOR_GREEN     0b1111100000011111
#define COLOR_BLUE      0b1111111111100000
#define COLOR_YELLOW    0b0000000000011111
#define COLOR_CYAN      0b0000011111111111
#define COLOR_MAGENTA   0b1111100000011111
#define COLOR_GRAY      0b1100011100011000
#define COLOR_DARKGRAY  0b1110011110011100
#define COLOR_ORANGE    0b0000001011011111

#ifndef TFT_CS
#define TFT_CS               8
#endif
#ifndef TFT_RST
#define TFT_RST              10 //17
#endif
#ifndef TFT_DC
#define TFT_DC               9
#endif
#ifndef TFT_BL
#define TFT_BL               14
#endif
#ifndef TFT_SCK
#define TFT_SCK              12
#endif
#ifndef TFT_MOSI
#define TFT_MOSI             11
#endif
#ifndef TFT_MISO
#define TFT_MISO             13
#endif
#ifndef TFT_ROT
#define TFT_ROT              1
#endif
#ifndef BG_COLOR
#define BG_COLOR              COLOR_BLACK
#endif
#ifndef IP_COLOR
#define IP_COLOR              COLOR_WHITE
#endif
#ifndef BPS_COLOR
#define BPS_COLOR             COLOR_RED
#endif
#ifndef IP_FONTSIZE
#define IP_FONTSIZE           1
#endif
#ifndef BPS_FONTSIZE
#define BPS_FONTSIZE          1
#endif
#ifndef IP_POS_X
#define IP_POS_X              75
#endif
#ifndef IP_POS_Y
#define IP_POS_Y              310
#endif
#ifndef BPS_POS_X
#define BPS_POS_X             300
#endif
#ifndef BPS_POS_Y
#define BPS_POS_Y             310
#endif
#ifndef CLOCK_BIG_COLOR
#define CLOCK_BIG_COLOR       COLOR_WHITE
#endif
#ifndef CLOCK_BIG_FONTSIZE
#define CLOCK_BIG_FONTSIZE    4
#endif
#ifndef CLOCK_BIG_CURSOR_X
#define CLOCK_BIG_CURSOR_X    10
#endif
#ifndef CLOCK_BIG_CURSOR_Y
#define CLOCK_BIG_CURSOR_Y    200
#endif
#ifndef CLOCK_SMALL_COLOR
#define CLOCK_SMALL_COLOR     COLOR_WHITE
#endif
#ifndef CLOCK_SMALL_FONTSIZE
#define CLOCK_SMALL_FONTSIZE   1
#endif
#ifndef CLOCK_SMALL_CURSOR_X
#define CLOCK_SMALL_CURSOR_X   200
#endif
#ifndef CLOCK_SMALL_CURSOR_Y
#define CLOCK_SMALL_CURSOR_Y   30
#endif
#ifndef FONT1_MAX_CHAR
#define FONT1_MAX_CHAR         10
#endif
#ifndef FONT2_MIN_CHAR
#define FONT2_MIN_CHAR         7
#endif
#ifndef FONT2_MAX_CHAR
#define FONT2_MAX_CHAR         17
#endif
#ifndef RADIO_STATION_FONTSIZE        
#define RADIO_STATION_FONTSIZE     1
#endif
#ifndef RADIO_STATION_COLOR
#define RADIO_STATION_COLOR    COLOR_ORANGE
#endif
#ifndef RADIO_STATION_X        
#define RADIO_STATION_X        50
#endif
#ifndef RADIO_STATION_Y
#define RADIO_STATION_Y        100
#endif
#ifndef RADIO_TITLE_FONTSIZE
#define RADIO_TITLE_FONTSIZE       1
#endif
#ifndef RADIO_TITLE_COLOR
#define RADIO_TITLE_COLOR      COLOR_GREEN
#endif
#ifndef RADIO_TITLE_X
#define RADIO_TITLE_X          50
#endif
#ifndef RADIO_TITLE_Y
#define RADIO_TITLE_Y          200
#endif
#define BOOTWINDOW_X        50
#define BOOTWINDOW_Y        50
#define BOOTWINDOW_WIDTH    380
#define BOOTWINDOW_HEIGHT   220
#define BOOTWINDOW_COLOR    COLOR_DARKGRAY

class AudioDisplay : public Adafruit_ST7796S{
public:
  /// @brief Constructor für das Anzeigeobjekt
  /// @param _cs Der CS Pin
  /// @param _dc der DC Pin
  /// @param _rst Der RST Pin
  AudioDisplay(int8_t _cs, int8_t _dc, int8_t _rst);
  /// @brief Initialisiert die Anzeige
  void begin();
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

  void wipe_vol();  
  void show_vol(uint8_t cur_vol);  

/// @brief Zeigt eine Bootmeldung auf dem Display an
/// @details Diese Funktion wird beim Booten des Gerätes aufgerufen, um eine Nachricht auf
/// dem Display anzuzeigen. Der Text wird in der angegebenen Farbe dargestellt. Dabei gilt folgende Festlegung:
/// - txtcolor: 0 = grau, 1 = grün, 2 = rot
/// @param txtcolor Die Textfarbe für die Bootmeldung: 0 = weiss, 1 = grün, 2 = rot
/// @param msg Die anzuzeigende Nachricht
/// @param newline Ein Flag, das angibt, ob die Nachricht in einer neuen Zeile angezeigt werden soll (true) oder in der aktuellen Zeile fortgesetzt werden soll (false).
/// @param align_right Ein Flag, das angibt, ob die Nachricht rechtsbündig (true) oder linksbündig (false) ausgerichtet werden soll.
/// @note Diese Funktion ist für die Anzeige auf dem Display zuständig und wird in der Regel
///       beim Start des Geräts aufgerufen, um den Benutzer über den Bootvorgang zu informieren.
  void bootMessage(uint8_t txtcolor, const char* msg, bool newline, bool align_right);

private:
  void update_display();
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

uint8_t boot_line = 0;
uint8_t num_lines = 0;
bool    font_big = false;
uint8_t rotation = 0;
};

#endif
