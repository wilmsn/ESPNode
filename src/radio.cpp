#include "radio.h"
#include "Audio.h"
#define I2S_DOUT     27
#define I2S_BCLK      26
#define I2S_LRC        25

Audio audio;

void Radio::begin(const char* html_place, const char* label) {
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(5);
  audio.connecttohost("http://stream.lokalradio.nrw/4459m27");
}

void Radio::loop(void) {
  audio.loop();
}

