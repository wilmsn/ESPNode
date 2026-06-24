#ifdef USE_AUDIODISPLAY_GC9A01A
#ifndef _AUDIODISPLAY_GC9A01A_H_
#define _AUDIODISPLAY_GC9A01A_H_

#include "audiodisplay.h"

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
#define CLOCK_BIG_FONTSIZE   7
#define CLOCK_BIG_CURSOR_X   20
#define CLOCK_BIG_CURSOR_Y   100
#define CLOCK_BIG_COLOR      GC9A01A_WHITE
#define CLOCK_BIG_BACKGROUND_COLOR GC9A01A_BLACK
#define CLOCK_SMALL_FONTSIZE 2
#define CLOCK_SMALL_CURSOR_X 80
#define CLOCK_SMALL_CURSOR_Y 25
#define CLOCK_SMALL_WIDTH    90
#define CLOCK_SMALL_HEIGHT   23
#define CLOCK_SMALL_COLOR    GC9A01A_WHITE
#define CLOCK_SMALL_BACKGROUND_COLOR GC9A01A_BLACK


/**
 * @brief Anzeige für das Audiomodul spezialisiert auf das Display GC9A01A
 */
class AudioDisplay_GC9A01A : public AudioDisplay {
public:

  /// @brief Die Loop Funktion wird regelmäßig aufgerufen
  /// @param now Der Unix Zeitstempel
  void loop(time_t now);

  void update_display();

  void show_vol(uint8_t cur_vol);  

private:
  void clock_small();
  void clock_big();
  void clock_print();
  void fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour);
  int last_min;

};

#endif // _AUDIODISPLAY_GC9A01A_H_
#endif // USE_AUDIODISPLAY_GC9A01A