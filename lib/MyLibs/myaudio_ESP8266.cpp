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
#define USE_ME
#ifdef USE_ME
#include "myaudio_ESP8266.h"

//Includes from ESP8266audio
#include <AudioFileSourceICYStream.h>
#include <AudioFileSourceBuffer.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>


#define LRC  26
#define BCLK 27
#define DIN  25
#define TITLELENGTH 50

//buffer size for stream buffering
const int preallocateBufferSize = 82*1024;  //size of preallocated buffer
const int preallocateCodecSize = 29192;     // MP3 codec max mem needed
//pointer to preallocated memory
void *preallocateBuffer = NULL;
void *preallocateCodec = NULL;
char ext_mytitle[TITLELENGTH];
bool ext_newtitle = false;

//instances for audio components
AudioGenerator *decoder = NULL;         //MP3 decoder
AudioFileSourceICYStream *file = NULL;  //Input modul for ICY stream from web
AudioFileSourceBuffer *buff = NULL;     //Buffer for input stream
AudioOutputI2S *out;                    //Output module to produce I2S


//callback function will be called if meta data were found in input stream
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  if (strstr_P(type, PSTR("Title"))) { 
    strncpy(ext_mytitle, string, sizeof(ext_mytitle));
    ext_mytitle[sizeof(ext_mytitle)-1] = 0;
    ext_newtitle = true;
  };
}

//function to do all the required setup
void MyAudio::begin() {
  //reserve buffer für for decoder and stream
  preallocateBuffer = malloc(preallocateBufferSize);          // allocate memory for stream-file-buffer
  preallocateCodec = malloc(preallocateCodecSize);            // allocate memory for decoder- buffer
  delay(1000);
  //check if we got the memory
  if (!preallocateBuffer || !preallocateCodec) {
    while(1){
      yield(); // Infinite halt
    }
  } 
  //create I2S output do use with decoder
  out = new AudioOutputI2S();
  //set the pins to b e used for I2S: Bit-clock, left/right clock, datastream
  out->SetPinout(BCLK, LRC, DIN);
}

//this function checks if decoder runs
void MyAudio::loop() {
  //check if stream has ended normally not on ICY streams
  if (decoder->isRunning()) {
    if (!decoder->loop()) {
      decoder->stop();
    }
  } else {
    delay(10000);
    ESP.restart();
  }  
}

//stop playing the input stream release memory, delete instances
void MyAudio::stopPlaying() {
  if (decoder)  {
    decoder->stop(); //stop playing
    delete decoder; //free decoder an its memory
    decoder = NULL; 
  }
  if (buff)  {
    buff->close(); //do the same for buffer
    delete buff;
    buff = NULL;
  }
  if (file)  {    //AND FINALLY FOR THE STREAM
    file->close();
    delete file;
    file = NULL;
  }
}

//start playing a stream from given station
bool MyAudio::startUrl(char* url) {
  bool retval = true;

  stopPlaying();  //first close existing streams
  if (file) delete file;
  delay(500);
  file = new AudioFileSourceICYStream();
  Serial.print("A3: ");

  retval = file->open(url);
  if (retval) {
    file->RegisterMetadataCB(MDCallback, NULL); 
    //create a new buffer which uses the preallocated memory
    buff = new AudioFileSourceBuffer(file, preallocateBuffer, preallocateBufferSize);
  Serial.print("A4: ");
    //create and start a new decoder
    decoder = (AudioGenerator*) new AudioGeneratorMP3(preallocateCodec, preallocateCodecSize);
  Serial.print("A5: ");
    delay(1000);
    decoder->begin(buff, out);
  } else {
    delete file; //if no success delete the file
    file = NULL;
  }
  return retval;
}

//change the loudness to current gain
void MyAudio::setVol(float newVol){
  float v = newVol / 100.0;
  out->SetGain(v);  //the parameter is the loudness as percent
  Serial.printf("New volume = %4.2f\n",v);
}

void MyAudio::on() {

}

void MyAudio::off() {
  stopPlaying();
}

char* MyAudio::showTitle(){
  ext_newtitle = false;
  return ext_mytitle;
}

bool MyAudio::newTitle(){
  return ext_newtitle;
}
#endif