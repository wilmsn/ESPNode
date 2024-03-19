#include "config.h"

#ifdef _WEBRADIO_H_
//#include "webradio.h"
#include "Audio.h"

#define I2S_DOUT     25
#define I2S_BCLK     27
#define I2S_LRC      26

Audio audio;

void Webradio::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword) {

// Definitions for ESP32 Board
#ifdef ESP32
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
char url[128];

//instance for rotary encoder
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

char title[128];

void IRAM_ATTR readRotaryISR() {
  rotary.readEncoder_ISR();
}


void Webradio::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword) {
  // First, preallocate all the memory needed for the buffering and codecs, never to be freed
  preallocateBuffer = malloc(preallocateBufferSize);
  preallocateCodec = malloc(preallocateCodecSize);
  if (!preallocateBuffer || !preallocateCodec) {
    write2log(LOG_CRITICAL,1,"FATAL ERROR:  Unable to preallocate Memory for app");
    while (1) delay(1000); // Infinite halt
  }
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
  audio.setBufsize(30000,-1);
//  audioLogger = &Serial;
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
  file = NULL;
  buff = NULL;
  out = new AudioOutputI2S();
  if (out->SetPinout(BCLK, LRCLK, DOUT)) {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("Failed to set pinout");
#endif
  }
  decoder = NULL;
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
    slider_val_old = obj_slider_val;
    myvol = (uint8_t)(obj_slider_val / 11);
    sprintf(volstr,"New Volume %u",myvol);
    write2log(LOG_SENSOR,1,volstr);
    audio.setVolume(myvol);
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
}

void audio_info(const char *info){
    write2log(LOG_SENSOR,2,"Info:", info);
}

void audio_showstreamtitle(const char *info){
    write2log(LOG_SENSOR,2,"Titel:", info);
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
    String sinfo = String(info);
    sinfo.replace("|", "\n");
    write2log(LOG_SENSOR,2,"Station:", info);
}







#endif
