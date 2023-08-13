#include "radio.h"
#include "Audio.h"
#include "AiEsp32RotaryEncoder.h"

#define I2S_DOUT       27
#define I2S_BCLK       26
#define I2S_LRC        25

//used pins for rotary encoder
#define ROTARY_ENCODER_A_PIN 13
#define ROTARY_ENCODER_B_PIN 14
#define ROTARY_ENCODER_BUTTON_PIN 12
#define ROTARY_ENCODER_VCC_PIN 12 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4
//instance for rotary encoder
AiEsp32RotaryEncoder gain = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

Audio audio;

String title;

//interrupt handling for rotary encoder
void IRAM_ATTR readGainISR() {
  gain.readEncoder_ISR();
}

void audio_showstreamtitle(const char *info){
    title = info;
}

void audio_showstation(const char *info){
    Serial.print("station     ");
    Serial.println(info);
}

void Radio::begin(const char* html_place, const char* label) {
  gain.begin(); //satrencoder handling
  gain.setup(readGainISR); //register interrupt service routine
  gain.setBoundaries(0, 50, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  gain.setEncoderValue(3); //preset the value to current gain

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(gain.readEncoder());
  audio.connecttohost("http://stream.lokalradio.nrw/4459m27");
}

void Radio::loop(void) {
  audio.loop();
  if (gain.encoderChanged()) {
    uint8_t vol = gain.readEncoder();
    Serial.print("Volume: ");
    Serial.println(vol);
    Serial.println(title);
    audio.setVolume(vol);
  }
}

