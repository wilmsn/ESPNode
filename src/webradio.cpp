#include "config.h"

#ifdef _WEBRADIO_H_
//#include "webradio.h"
//Includes from ESP8266audio
#include "AudioFileSourceICYStream.h" //input stream
#include "AudioFileSourceBuffer.h"    //input buffer
#include "AudioGeneratorMP3.h"        //decoder
#include "AudioOutputI2S.h"           //output stream
#include <myencoder.h>

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


const int preallocateBufferSize = 20480; //5*1024;
const int preallocateCodecSize = 29192; // MP3 codec max mem needed
//const int preallocateBufferSize = 16*1024;
//const int preallocateCodecSize = 85332; // AAC+SBR codec max mem needed
float volume = 0.50;
void *preallocateBuffer = NULL;
void *preallocateCodec = NULL;

level_t level[MAXLEVEL];
station_t station[MAXSTATION];
char url[128];

//instances for audio components
AudioGenerator *decoder = NULL;
AudioFileSourceICYStream *file = NULL;
AudioFileSourceBuffer *buff = NULL;
AudioOutputI2S *out = NULL;

//instance for rotary encoder
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

char title[128];

void IRAM_ATTR readRotaryISR() {
  rotary.readEncoder_ISR();
}

//callback function will be called if meta data were found in input stream
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  if (strstr_P(type, PSTR("Title"))) { 
    strncpy(title, string, sizeof(title));
    title[sizeof(title)-1] = 0;
    write2log(LOG_SENSOR,1, title);
    //show the message on the display
//  newTitle = true;
  } else {
    // Who knows what to do?  Not me!
  };
}

void StatusCallback(void *cbData, int code, const char *string)
{
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1) - 1] = 0;
  write2log(LOG_SENSOR,1,s1);
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
#if defined(DEBUG_SERIAL_MODULE)
    Serial.print("Set PinOut: BCLK:");
    Serial.print(I2S_BCLK);
    Serial.print(" LRC:");
    Serial.print(I2S_LRC);
    Serial.print(" DOUT:");
    Serial.println(I2S_DOUT);
#endif
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
  mystate = true;
  webradio_on();
}

void Webradio::webradio_off() {
  if (decoder) {
    decoder->stop();
    delete decoder;
    decoder = NULL;
  }
  if (buff) {
    buff->close();
    delete buff;
    buff = NULL;
  }
  if (file) {
    file->close();
    delete file;
    file = NULL;
  }
  write2log(LOG_SENSOR,1,"webradio off");
}

void Webradio::webradio_on() {
  file = new AudioFileSourceICYStream(url);
  write2log(LOG_SENSOR,1,"created icystream");
  file->RegisterMetadataCB(MDCallback, NULL);
  buff = new AudioFileSourceBuffer(file, preallocateBuffer, preallocateBufferSize);
  write2log(LOG_SENSOR,1,"created buffer");
  buff->RegisterStatusCB(StatusCallback, NULL);
  decoder = (AudioGenerator*) new AudioGeneratorMP3(preallocateCodec, preallocateCodecSize);
  write2log(LOG_SENSOR,1,"created decoder");
  decoder->RegisterStatusCB(StatusCallback, NULL);
  write2log(LOG_SENSOR,1,"Decoder start...\n");
  decoder->begin(buff, out);
  if (file) {
      write2log(LOG_SENSOR,1,"Playing Radio Kiepenkerl");
  }
  set_vol();
}

void Webradio::set_vol() {
    char vol_str[20];
    snprintf(vol_str,19, "New Volume: %3.0f",cur_vol);
    write2log(LOG_SENSOR,1,vol_str);
    sprintf(vol_str,"S:%u",cur_vol);
    set(show_keyword(),String(vol_str));
    out->SetGain((float)cur_vol/100.0);
}

void Webradio::loop() {
  if (decoder && decoder->isRunning()) {
//    strcpy_P(status, PSTR("Playing")); // By default we're OK unless the decoder says otherwise
    if (!decoder->loop()) {
      write2log(LOG_SENSOR,1,"Unusual stopping of decoder");
      webradio_off();
      webradio_on();
    }
  }
  if (slider_val_old != obj_slider_val) {
    
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
      webradio_on();
    } else {
      webradio_off();
    }
    mystate = obj_value;
  }
}



#endif
