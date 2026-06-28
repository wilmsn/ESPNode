#include "config.h"
#ifdef USE_AUDIODISPLAY_GC9A01A
#include "audiodisplay_GC9A01A.h"
#include "audiodisplay_bmps.h"
#include "audiomodul.h"
#include "common.h"

extern AudioModul* audiomodul_ptr;

void AudioDisplay_GC9A01A::update_display() {
  if (display) {
    audiomodul_ptr->display_update_set = false;
    Serial.print("Update Display: App_NO:");
    Serial.println(audiomodul_ptr->app_no);
    if (audiomodul_ptr->app_no == audiomodul_ptr->app_no_off) {
      display->fillScreen(GC9A01A_BLACK);
      clock_big();
    } // audiomodul_ptr->app_no == audiomodul_ptr->app_no_off
    else if (audiomodul_ptr->app_no == audiomodul_ptr->app_no_radio) {
      display->fillScreen(GC9A01A_BLACK);
      clock_small();
      show_vol(audiomodul_ptr->vol);
      // set ip address
      display->setTextColor(IP_COLOR);  
      display->setTextSize(IP_FONTSIZE);
      display->setCursor(IP_POS_X,IP_POS_Y);
      display->print(WiFi.localIP().toString());
      // end set ip address
      // set bps
      display->setTextColor(BPS_COLOR);  
      display->setTextSize(BPS_FONTSIZE);
      display->setCursor(BPS_POS_X, BPS_POS_Y);
      display->println(audiomodul_ptr->bps);
      // end set bps
      // set station
      num_lines = split4display(audiomodul_ptr->radio_stationname);
      display->setTextColor(GC9A01A_ORANGE);
      if (num_lines == 1) {
        display->setTextSize(3);
        display->setCursor(25, 75);
        display->print(displaystr[0]);
      } else {
        display->setTextSize(2);
        uint8_t thisline = 0;
        while (thisline < num_lines && thisline < 2) {
          if (thisline <= 1) display->setCursor(25, 75 + (thisline * 20));
          display->print(displaystr[thisline]);
          thisline++;
        }
      }
      // end set station
      // set streamtitle
      num_lines = split4display(audiomodul_ptr->radio_streamtitle);
      display->setTextColor(GC9A01A_GREEN);
      if (num_lines == 1) {
        display->setTextSize(3);
        display->setCursor(25, 130);
        display->print(displaystr[0]);
      } else {
        display->setTextSize(2);
        uint8_t thisline = 0;
        while (thisline < num_lines && thisline < 3) {
          display->setCursor(25, 130 + (thisline * 20));
          display->print(displaystr[thisline]);
          thisline++;
        }
      }
    } //audiomodul_ptr->app_no == audiomodul_ptr->app_no_radio
    else if (audiomodul_ptr->app_no == audiomodul_ptr->app_no_radio_select) {
      display->fillScreen(GC9A01A_BLACK);
      display->setTextSize(2);
      display->setTextColor(GC9A01A_WHITE);
      display->setCursor(60,20);
      display->print("Senderwahl");
      display->setTextColor(GC9A01A_ORANGE);
/*      display->setCursor(40,70);
      display->setTextSize(1);
      if (audiomodul_ptr->rot_last_val > 0) {
        print(audiomodul_ptr->radio_station[audiomodul_ptr->rot_last_val-1].name);
      }*/
      display->setCursor(25,120);
      String stationname = audiomodul_ptr->radio_station[audiomodul_ptr->radio_sel_station].name;
      num_lines = split4display(stationname);
      if (num_lines == 1) {
        display->setTextSize(3);
        display->print(displaystr[0]);
      } else {
        display->setTextSize(2);
        uint8_t thisline = 0;
        while (thisline < num_lines && thisline < 2) {
          if (thisline <= 1) display->setCursor(25, 120 + (thisline * 20));
          display->print(displaystr[thisline]);
          thisline++;
        }
      }
/*      display->setCursor(40,180);
      setTextSize(1);
      if (audiomodul_ptr->rot_last_val < MAXSTATIONS-1) {
        print(audiomodul_ptr->radio_station[audiomodul_ptr->rot_last_val+1].name);
      }*/
    } // audiomodul_ptr->app_no == audiomodul_ptr->app_no_radio_select
    else if (audiomodul_ptr->app_no == audiomodul_ptr->app_no_media) {
      display->fillScreen(GC9A01A_BLACK);
      clock_small();
      num_lines = split4display(audiomodul_ptr->media_artist_name);
      display->setTextColor(GC9A01A_ORANGE);
      if (num_lines == 1) {
        display->setTextSize(3);
        display->setCursor(25, 75);
        display->print(displaystr[0]);
      } else {
        display->setTextSize(2);
        uint8_t thisline = 0;
        while (thisline < num_lines && thisline < 2) {
          if (thisline <= 1) display->setCursor(25, 75 + (thisline * 20));
          display->print(displaystr[thisline]);
          thisline++;
        }
      }
    // end set station
    // set streamtitle
      num_lines = split4display(audiomodul_ptr->media_song_name);
      display->setTextColor(GC9A01A_GREEN);
      if (num_lines == 1) {
        display->setTextSize(3);
        display->setCursor(25, 130);
        display->print(displaystr[0]);
      } else {
        display->setTextSize(2);
        uint8_t thisline = 0;
        while (thisline < num_lines && thisline < 3) {
          display->setCursor(25, 130 + (thisline * 20));
          display->print(displaystr[thisline]);
          thisline++;
        }
      }
    } // audiomodul_ptr->app_no == audiomodul_ptr->app_no_media
    else if (audiomodul_ptr->app_no == audiomodul_ptr->app_no_max+1) {
      display->fillScreen(GC9A01A_BLACK);
      if (audiomodul_ptr->app_no_new == audiomodul_ptr->app_no_off) {
        display->drawRGBBitmap(80,80,off_bmp,OFF_BMP_HEIGHT,OFF_BMP_WIDTH);
      }
      if (audiomodul_ptr->app_no_new == audiomodul_ptr->app_no_radio) {
        display->drawRGBBitmap(80,80,radio_bmp,RADIO_BMP_HEIGHT,RADIO_BMP_WIDTH);
      }
    }
  } else {
    Serial.println("Display not initialized");
  }
}

void AudioDisplay_GC9A01A::loop(time_t now) {
  localtime_r(&now, &timeinfo);
  if (audiomodul_ptr->display_update_vol) {
    show_vol(audiomodul_ptr->vol);
    audiomodul_ptr->display_update_vol = false;
  }
  if (audiomodul_ptr->display_update_now) {
    update_display();
    audiomodul_ptr->display_update_now = false;
  }
  if (audiomodul_ptr->display_update_set &&
     (timeinfo.tm_sec == 10 || 
      timeinfo.tm_sec == 20 ||
      timeinfo.tm_sec == 30 ||
      timeinfo.tm_sec == 40 ||
      timeinfo.tm_sec == 50)) {
    update_display();
  }
  if (timeinfo.tm_min != last_min ) {
    last_min = timeinfo.tm_min;
    update_display();
  }
}

void AudioDisplay_GC9A01A::show_vol(uint8_t cur_vol) {
  // Umrechnung von 0-100 auf 0-180 Grad
  uint8_t disp_vol = map(cur_vol,0,100,0,180);
  // Alte Volume Anzeige löschen
  fillArc(119,119,-90,180,120,120,ARC_WIDTH,GC9A01A_BLACK);
  // Neue Volume Anzeige zeichnen
  fillArc(119,119,-90,disp_vol,120,120,ARC_WIDTH,GC9A01A_YELLOW);
}

void AudioDisplay_GC9A01A::clock_big() {
  display->fillScreen(CLOCK_BIG_BACKGROUND_COLOR);
  display->setTextColor(CLOCK_BIG_COLOR); 
  display->setTextSize(CLOCK_BIG_FONTSIZE);
  display->setCursor(CLOCK_BIG_CURSOR_X,CLOCK_BIG_CURSOR_Y);
  clock_print();
  display->setTextColor(IP_COLOR);  
  display->setTextSize(IP_FONTSIZE);
  display->setCursor(IP_POS_X,IP_POS_Y);
  display->print(WiFi.localIP().toString());
}

void AudioDisplay_GC9A01A::clock_small() {
  display->fillRect(CLOCK_SMALL_CURSOR_X, CLOCK_SMALL_CURSOR_Y, CLOCK_SMALL_WIDTH, CLOCK_SMALL_HEIGHT, CLOCK_SMALL_BACKGROUND_COLOR);
  display->setTextColor(CLOCK_SMALL_COLOR); 
  display->setTextSize(CLOCK_SMALL_FONTSIZE);
  display->setCursor(CLOCK_SMALL_CURSOR_X,CLOCK_SMALL_CURSOR_Y);
  clock_print();
}

void AudioDisplay_GC9A01A::clock_print() {
  if ( timeinfo.tm_hour < 10) display->printf(" ");
  display->printf("%d:",timeinfo.tm_hour);
  if ( timeinfo.tm_min < 10) display->printf("0");
  display->printf("%d",timeinfo.tm_min);
}

void AudioDisplay_GC9A01A::fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour) {

  byte seg = ARC_SIGMENT_DEGREES; // Segments are 3 degrees wide = 120 segments for 360 degrees
  byte inc = ARC_SIGMENT_DEGREES; // Draw segments every 3 degrees, increase to 6 for segmented ring

  // Calculate first pair of coordinates for segment start
  float sx = cos((start_angle - 90) * DEG_TO_RAD);
  float sy = sin((start_angle - 90) * DEG_TO_RAD);
  uint16_t x0 = sx * (rx - w) + x;
  uint16_t y0 = sy * (ry - w) + y;
  uint16_t x1 = sx * rx + x;
  uint16_t y1 = sy * ry + y;

  // Draw colour blocks every inc degrees
  for (int i = start_angle; i < start_angle + degree; i += inc) {

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * DEG_TO_RAD);
    float sy2 = sin((i + seg - 90) * DEG_TO_RAD);
    int x2 = sx2 * (rx - w) + x;
    int y2 = sy2 * (ry - w) + y;
    int x3 = sx2 * rx + x;
    int y3 = sy2 * ry + y;

    display->fillTriangle(x0, y0, x1, y1, x2, y2, colour);
    display->fillTriangle(x1, y1, x2, y2, x3, y3, colour);

    // Copy segment end to sgement start for next segment
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
}

#endif
