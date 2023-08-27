#include <Arduino.h>

class MyAudio {


    public:

    //function to do all the required setup
    void begin();

    //Loop function keeps the decoder running
    void loop();

    void on();

    void off();
    
    //start playing a stream from given station
    bool startUrl(char* url);

    //change the loudness
    void setVol(float newVol);

    //gives back the title
    char* showTitle();

    //do we have a new title? true if yes / false if no
    bool newTitle();

private:

    //stop playing the input stream release memory, delete instances
    void stopPlaying();

    bool titleChanged = false;
};
