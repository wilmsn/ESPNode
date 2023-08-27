#include <Arduino.h>

class MyEncoder {

public:

void begin();
void begin(uint8_t startPos);
void loop();
void setPos(uint8_t newPos);
void setLimits(uint8_t minLim, uint8_t maxLim);
bool newPos();
uint8_t readPos();

bool newLev();
uint8_t readLev();

bool isLongClicked(); 
bool isShortClicked(); 

bool isButtonDown();
//bool isButtonClicked();

private:
uint8_t curPos = 0;
bool posChanged = false;
uint8_t curLev = 0;
uint8_t maxLev = 1;
bool levChanged = false;
bool shortClicked = false;
bool longClicked = false;
unsigned long lastTimePressed = 0;

};
