/*******************************************************
 *  Connection between ESP32 and MAX 98357A
 *    ESP32              MAX98357A
 *    ------             -------
 *    5V                 Vin
 *    GND                GND 
 *    GPIO 26            LRC
 *    GPIO 27            BCLK
 *    GPIO 25            DIN
 *     -                 Gain
 *     -                 SD
 ********************************************************/
//#define USE_ME
#ifdef USE_ME
#include "myaudio_ESP32.h"

//Includes from ESP32-audioI2S
#include "Audio.h"

#define LRC  26
#define BCLK 27
#define DIN  25
#define TITLELENGTH 50

Audio audio;

char title[TITLELENGTH];

//function to do all the required setup
void MyAudio::begin() {
    audio.setPinout(BCLK, LRC, DIN);
    audio.setVolume(15); // 0...21
    audio.connecttohost("http://streams.radio21.de/nrw/mp3-192/web");
}

//this function checks if decoder runs
void MyAudio::loop() {
  audio.loop();
}

void MyAudio::setVol(float newVol) {
   audio.setVolume(newVol); 
}

bool MyAudio::startUrl(String url) {
   return audio.connecttohost(url.c_str()); 
}

char* MyAudio::showTitle() {
    snprintf(title,10,"tbd.");
    return title;
}

 bool MyAudio::newTitle() {
    
    return false;
 }
#endif