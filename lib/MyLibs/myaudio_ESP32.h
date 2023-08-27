#include <Arduino.h>

class MyAudio {


    public:

    //function to do all the required setup
    void begin();

    //Loop function keeps the decoder running
    void loop();

    //start playing a stream from given station
    bool startUrl(char* url);

    //change the loudness
    void setVol(float newVol);

    //gives back the title
    char* showTitle();

    //do we have a new title? true if yes / false if no
    bool newTitle();

    private:

    bool titleChanged = false;
};