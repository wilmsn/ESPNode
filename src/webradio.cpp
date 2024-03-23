#include "config.h"

#ifdef _WEBRADIO_H_
//#include "webradio.h"
#include "Audio.h"
#include "AiEsp32RotaryEncoder.h"

//#define I2S_DOUT     25
//#define I2S_BCLK     27
//#define I2S_LRC      26
#define DO_ESP32S3
Audio audio;
extern tm timeinfo;

// Definitions for ESP32 Board
#ifdef DO_ESP32
#warning "ESP32"
// I2S Settings
#define LRCLK 26
#define BCLK 27
#define DOUT 25
// Rotary Encoder Settings
#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 33
#define ROTARY_ENCODER_BUTTON_PIN 4
#define ROTARY_ENCODER_VCC_PIN -1
//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4
//TFT Settings
#define GC9A01A_TFT_DC 5
#define GC9A01A_TFT_CS 21
#define ARC_SIGMENT_DEGREES 3
#define ARC_WIDTH 5
#endif
#ifdef DO_ESP32S3
#warning "ESP32-S3"
#define I2S_DOUT     6
#define I2S_BCLK     5
#define I2S_LRC      4
#define ROTARY_ENCODER_A_PIN      37
#define ROTARY_ENCODER_B_PIN      36
#define ROTARY_ENCODER_BUTTON_PIN 35
#define ROTARY_ENCODER_VCC_PIN    -1
#define ROTARY_ENCODER_STEPS      4
// Display
// SCL (Display) => SCK  12
// SDA (Display) => MOSI 11
#define GC9A01A_TFT_CS        10
#define GC9A01A_TFT_DC        9
#endif


// Level definitions
#define MAXLEVEL    3
// Level 1 ist die Lautst#rke
#define LEVEL1_MIN  0
#define LEVEL1_MAX  100
// Level 2 ist die Senderauswahl
#define LEVEL2_MIN  0
#define LEVEL2_MAX  9
// Level 3 ist ???
#define LEVEL3_MIN  0
#define LEVEL3_MIN  0
// Station definitions
#define MAXSTATION            10
#define STATION_NAME_LENGTH   20
#define STATION_URL_LENGTH    30

typedef struct {
    uint8_t level;
    uint8_t min;
    uint8_t max;
    uint8_t cur;
} level_t;

typedef struct {
    uint8_t num;
    char    name[STATION_NAME_LENGTH];
    char    url[STATION_URL_LENGTH];
} station_t;


level_t level[MAXLEVEL];
station_t station[MAXSTATION];
int last_min = 0;
char url[128];

//instance for rotary encoder
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);
//instance for TFT Display
Adafruit_GC9A01A tftx(GC9A01A_TFT_CS, GC9A01A_TFT_DC);
RadioDisplay mydisplay;

char title[128];

void IRAM_ATTR readRotaryISR() {
  rotary.readEncoder_ISR();
}

void Webradio::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword) {
  // First, preallocate all the memory needed for the buffering and codecs, never to be freed
  bool start_value = true;
  bool on_value = true;
  if (audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT)) {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.print("Set PinOut: BCLK:");
    Serial.print(I2S_BCLK);
    Serial.print(" LRC:");
    Serial.print(I2S_LRC);
    Serial.print(" DOUT:");
    Serial.println(I2S_DOUT);
#endif
  }
  audio.setBufsize(30000,300000);
  audio.setVolumeSteps(100);
//  audioLogger = &Serial;
  mydisplay.begin(&tftx);
  mydisplay.show_ip(WiFi.localIP().toString().c_str());
  rotary.begin(); //satrencoder handling
// ToDo
// Level Array aus den Preferences einlesen  
// Danach folgende 3 Zeilen löschen
  rotary.setup(readRotaryISR); //register interrupt service routine
  rotary.setBoundaries(0, 100, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotary.setEncoderValue(cur_vol); //preset the value to current gain
//
// ToDo 
// Im Webinterface muss der slider auf die aktuelle Lautstärke gesetzt werden.
//  myvol = (float)(obj_slider_val / 2.55);
// ToDo
//  if (audio.setPinout(BCLK, LRCLK, DOUT)) {
//#if defined(DEBUG_SERIAL_MODULE)
//    Serial.println("Failed to set pinout");
//#endif
//  }
// ToDo
// Stationsliste aus den Preferences laden  
  sprintf(url,"%s","http://stream.lokalradio.nrw/4459m27");
// End ToDo  
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, start_value, on_value, 100, 1, "Lautstärke");
  webradio_on();
  mystate = true;
}

void Webradio::webradio_off() {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("webadio_off");
#endif
    audio.setVolume(0);
    audio.stopSong();
}

void Webradio::webradio_on() {
  if (audio.connecttohost("http://stream.lokalradio.nrw/4459m27")) {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("Playing Radio Kiepenkerl");
#endif
  }
  set_vol();  
}

void Webradio::set_vol() {
  char volstr[20];
  int  bg;
  sprintf(volstr,"New Volume %u",cur_vol);
  write2log(LOG_SENSOR,1,volstr);
  audio.setVolume(cur_vol);
  mydisplay.show_vol(cur_vol);
}

void Webradio::loop() {
  audio.loop();
  if (!audio.isRunning()) {
    webradio_off();
    webradio_on();
  }
  if (slider_val_old != obj_slider_val ) {
    
    slider_val_old = obj_slider_val;
    cur_vol = obj_slider_val / 2.55;
//    rotary.setEncoderValue(cur_vol); 
//    set_vol();
  }
  if (rotary.encoderChanged()) {
    cur_vol = rotary.readEncoder();
    Serial.print("Rotary changed: ");
    Serial.println(cur_vol);
    set_vol();
  }
  if (obj_value != mystate) {
    if (obj_value) { 
      write2log(LOG_SENSOR,1,"Radio on");
      webradio_on();
    } else {
      write2log(LOG_SENSOR,1,"Radio off");
      webradio_off();
    }
    mystate = obj_value;
  }
  if (last_min != timeinfo.tm_min) {
    mydisplay.show_time();
    last_min = timeinfo.tm_min;
  }
}

void audio_info(const char *info){
    write2log(LOG_SENSOR,2,"Info:", info);
}

void audio_showstreamtitle(const char *info){
  write2log(LOG_SENSOR,2,"Titel:", info);
  mydisplay.show_title(info);
}

void audio_bitrate(const char *info) {
    char bpsInfo[5];
    bpsInfo[0] = info[0];
    bpsInfo[1] = info[1];
    bpsInfo[2] = info[2];
    bpsInfo[3] = 'K';
    bpsInfo[4] = 0;
    write2log(LOG_SENSOR,2,"Bitrate:", info);
}

void audio_showstation(const char *info){
//  String sinfo = String(info);
//  sinfo.replace("|", "\n");
  write2log(LOG_SENSOR,2,"Station:", info);
  mydisplay.show_station(info);  
}


/**************************
 * 
 * Display
 * 
***************************/



void RadioDisplay::show_ip(const char* myip) {
  tft->fillScreen(GC9A01A_BLACK);
  tft->setTextColor(GC9A01A_WHITE);  
  tft->setTextSize(1);
  tft->setCursor(75, 225);
  tft->println(myip);

}

void RadioDisplay::show_vol(uint8_t vol) {
  if (vol > 90) vol=90;
  fillArc(119,119,-90,180,120,120,ARC_WIDTH,GC9A01A_BLACK);
  fillArc(119,119,-90,vol*2,120,120,ARC_WIDTH,GC9A01A_YELLOW);
}

void RadioDisplay::show_time() {
  tft->fillRect(80, 30, 90, 23, GC9A01A_BLACK);
  tft->setTextColor(GC9A01A_WHITE); 
  tft->setTextSize(3);
  tft->setCursor(80,30);
  if ( timeinfo.tm_min < 10) {
    tft->printf("%d:0%d",timeinfo.tm_hour, timeinfo.tm_min);
  } else {
    tft->printf("%d:%d",timeinfo.tm_hour, timeinfo.tm_min);
  }
}

void RadioDisplay::show_station(const char* mystation) {
  tft->fillRect(25, 65, 190, 55, GC9A01A_BLACK);
  show_text(mystation, 25, 65, GC9A01A_ORANGE);
}

void RadioDisplay::show_title(const char* mytitle) {
  tft->fillRect(0, 130, 240, 60, GC9A01A_BLACK);
  show_text(mytitle, 25, 130, GC9A01A_GREEN);
}

void RadioDisplay::show_text(const char* mytext, int posx, int posy, uint16_t color) {
  int mypos = 0;
  int mytxtlength = strlen(mytext);
  int chars_per_line;
  int pixel_to_next_line;
  tft->setTextColor(color);  
  if (mytxtlength > 20) {
    chars_per_line = 16;
    tft->setTextSize(2);
    pixel_to_next_line = 20;
  } else {
    chars_per_line = 10;
    tft->setTextSize(3);
    pixel_to_next_line = 30;
  }
  char mystr[chars_per_line+1];
  mypos = splitStr(mytext,mypos,chars_per_line,mystr);
  if (mypos < strlen(mytext)) {
    tft->setCursor(posx, posy);
    tft->println(mystr);
    mypos = splitStr(mytext,mypos,chars_per_line,mystr);
  }
  tft->setCursor(posx, posy + pixel_to_next_line);
  tft->println(mystr);
}

void RadioDisplay::begin(Adafruit_GC9A01A* mytft) {
  tft = mytft;
  tft->begin();
  tft->fillScreen(GC9A01A_BLACK);
}

int RadioDisplay::splitStr(const char* inStr, int startPos, int maxLen, char* resultStr) {
  int char2cut = 0;
  int retval = 0;
  resultStr[0] = 0;
  if (strlen(inStr) > maxLen + startPos) {
    for(int i=startPos; i<maxLen+startPos; i++) {
       if ( inStr[i] == ' ') char2cut = i;
    }
    if (char2cut > 0) {
      retval = char2cut + 1;
    } else {
      retval = startPos + maxLen -1;
      char2cut = startPos + maxLen -1;
    }
  } else {
    char2cut = strlen(inStr);
    retval = char2cut;
  }
  Serial.print("Char2cut:");
  Serial.println(char2cut);
  for(int i=startPos; i<char2cut; i++) {
    resultStr[i-startPos] = inStr[i];
    Serial.print(inStr[i]);
    int xx = (inStr[i]);
    Serial.println(xx);
  }
  resultStr[char2cut-startPos] = 0;
  Serial.print(" Str in:");
  Serial.print(inStr);
  Serial.print(" Retval:");
  Serial.print(retval);
  Serial.print(" StartPos:");
  Serial.print(startPos);
  Serial.print(" MaxLen:");
  Serial.print(maxLen);
  Serial.print(" ");
  Serial.println(resultStr);
  return retval;
}

void RadioDisplay::fillArc(int x, int y, int start_angle, int degree, int rx, int ry, int w, unsigned int colour)
{

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

    tft->fillTriangle(x0, y0, x1, y1, x2, y2, colour);
    tft->fillTriangle(x1, y1, x2, y2, x3, y3, colour);

    // Copy segment end to sgement start for next segment
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
}







#endif
