#include "config.h"

#ifdef _WEBRADIO_H_
//#include "webradio.h"
//Includes from ESP8266audio
#include "AudioFileSourceICYStream.h" //input stream
#include "AudioFileSourceBuffer.h"    //input buffer
#include "AudioGeneratorMP3.h"        //decoder
#include "AudioOutputI2S.h"           //output stream


#define LRCLK 26
#define BCLK 27
#define DOUT 25

const int preallocateBufferSize = 20480; //5*1024;
const int preallocateCodecSize = 29192; // MP3 codec max mem needed
//const int preallocateBufferSize = 16*1024;
//const int preallocateCodecSize = 85332; // AAC+SBR codec max mem needed
float volume = 0.50;
void *preallocateBuffer = NULL;
void *preallocateCodec = NULL;
char url[128];

//instances for audio components
AudioGenerator *decoder = NULL;
AudioFileSourceICYStream *file = NULL;
AudioFileSourceBuffer *buff = NULL;
AudioOutputI2S *out = NULL;


char title[128];

//callback function will be called if meta data were found in input stream
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  if (strstr_P(type, PSTR("Title"))) { 
    strncpy(title, string, sizeof(title));
    title[sizeof(title)-1] = 0;
    Serial.printf("Title: %s", title);
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
  Serial.printf("STATUS: '%s'\n", s1);
  Serial.flush();
}

void Webradio::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword) {
  // First, preallocate all the memory needed for the buffering and codecs, never to be freed
  preallocateBuffer = malloc(preallocateBufferSize);
  preallocateCodec = malloc(preallocateCodecSize);
  if (!preallocateBuffer || !preallocateCodec) {
    Serial.begin(115200);
    Serial.printf_P(PSTR("FATAL ERROR:  Unable to preallocate %d bytes for app\n"), preallocateBufferSize+preallocateCodecSize);
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
  audioLogger = &Serial;
  file = NULL;
  buff = NULL;
  out = new AudioOutputI2S();
  if (out->SetPinout(BCLK, LRCLK, DOUT)) {
    Serial.println("Failed to set pinout");
  }
  decoder = NULL;
  sprintf(url,"%s","http://stream.lokalradio.nrw/4459m27");
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
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("webadio_off");
#endif
}

void Webradio::webradio_on() {
  file = new AudioFileSourceICYStream(url);
  Serial.printf_P(PSTR("created icystream\n"));
  file->RegisterMetadataCB(MDCallback, NULL);
  buff = new AudioFileSourceBuffer(file, preallocateBuffer, preallocateBufferSize);
  Serial.printf_P(PSTR("created buffer\n"));
  buff->RegisterStatusCB(StatusCallback, NULL);
  decoder = (AudioGenerator*) new AudioGeneratorMP3(preallocateCodec, preallocateCodecSize);
  Serial.printf_P(PSTR("created decoder\n"));
  decoder->RegisterStatusCB(StatusCallback, NULL);
  Serial.printf_P("Decoder start...\n");
  decoder->begin(buff, out);
  if (file) {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("Playing Radio Kiepenkerl");
#endif
  }
  set_vol();
}

void Webradio::set_vol() {
    slider_val_old = obj_slider_val;
    myvol = (float)(obj_slider_val / 2.55);
    Serial.print("New Volume: ");
    Serial.println(myvol);
    out->SetGain(myvol/100.0);
}

void Webradio::loop() {
  if (decoder && decoder->isRunning()) {
//    strcpy_P(status, PSTR("Playing")); // By default we're OK unless the decoder says otherwise
    if (!decoder->loop()) {
      Serial.println("Unusual stopping of decoder\n");
      webradio_off();
      webradio_on();
    }
  }

  if (slider_val_old != obj_slider_val ) {
#if defined(DEBUG_SERIAL_MODULE)
    Serial.println("Volume");
#endif
    set_vol();
  }
  if (obj_value != mystate) {
    if (obj_value) { 
#if defined(DEBUG_SERIAL_MODULE)
      Serial.println("Radio ON");
#endif
      webradio_on();
    } else {
#if defined(DEBUG_SERIAL_MODULE)
      Serial.println("Radio OFF");
#endif
      webradio_off();
    }
    mystate = obj_value;
  }
}


#endif
