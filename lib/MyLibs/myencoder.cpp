/*******************************************************
 *  Connection between ESP32 and Rotary Encoder
 *    ESP32              Rotary
 *    ------             -------
 *    3V3                VCC
 *    GND                GND 
 *    GPIO 04            KEY
 *    GPIO 32            S1
 *    GPIO 33            S2
 ********************************************************/
#include "myencoder.h"
#include "AiEsp32RotaryEncoder.h"

#define SERIAL_DEBUG

#ifdef ESP32
#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 33
#define ROTARY_ENCODER_BUTTON_PIN 4
#else
/*
// ESP8266 Testaufbau
#define ROTARY_ENCODER_A_PIN	      5  //D1
#define ROTARY_ENCODER_B_PIN	      12 //D6
#define ROTARY_ENCODER_BUTTON_PIN	  16 //D0
*/
#endif

#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4
//instance for rotary encoder
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

unsigned long shortPressAfterMiliseconds = 50;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned long longPressAfterMiliseconds = 1000;  //how long čong press shoud be.

//interrupt handling for rotary encoder
void IRAM_ATTR readRotaryISR() {
  rotary.readEncoder_ISR();
//  Serial.println("Rotary ISR");
}

void MyEncoder::setLimits(uint8_t minLim, uint8_t maxLim) {
  rotary.setBoundaries(minLim, maxLim, false);
#ifdef SERIAL_DEBUG
  Serial.print("Encoder Limits: ");
  Serial.print(minLim);
  Serial.print(" ");
  Serial.println(maxLim);
#endif  
}

void MyEncoder::setPos(uint8_t newPos) {
  rotary.setEncoderValue(newPos);
  curPos = newPos;
#ifdef SERIAL_DEBUG
  Serial.print("Encoder new Pos: ");
  Serial.println(curPos);
#endif  
}

bool MyEncoder::newPos() {
  bool retval = posChanged;
  posChanged = false;
  return retval;
}

uint8_t MyEncoder::readPos() {
  posChanged = false;
#ifdef SERIAL_DEBUG
  Serial.print("Encoder read Pos: ");
  Serial.println(curPos);
#endif
  curPos = rotary.readEncoder();
  return curPos;
}

bool MyEncoder::newLev() {
  return levChanged;
}

uint8_t MyEncoder::readLev() {
#ifdef SERIAL_DEBUG
//  Serial.print("Encoder readLevel: ");
//  Serial.println(curLev);
#endif  
  levChanged = false;
  return curLev;
}

bool MyEncoder::isLongClicked() {
  bool retval = false;
  if ( longClicked ) {
    longClicked = false;
    retval = true;
  }
  return retval;
}

bool MyEncoder::isShortClicked() {
  bool retval = false;
  if ( shortClicked ) {
    shortClicked = false;
    retval = true;
  }
  return retval;
}

void MyEncoder::begin(uint8_t startPos) {
  curPos = startPos;
  begin();
}

void MyEncoder::begin() {
  rotary.begin(); //satrencoder handling
  rotary.setup(readRotaryISR); //register interrupt service routine
  rotary.setBoundaries(0, 100, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotary.setEncoderValue(curPos); //preset the value to current gain
}

void MyEncoder::loop() {
  if (rotary.encoderChanged()) {
    curPos = rotary.readEncoder(); //get new value for gain
    posChanged = true;
    Serial.println("Rotary Loop pos changed");
  }
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;

  bool isEncoderButtonDown = rotary.isEncoderButtonDown();
  //isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed

  if (isEncoderButtonDown) {
    if (!wasButtonDown) {
      //start measuring
      lastTimeButtonDown = millis();
    }
    //else we wait since button is still down
    wasButtonDown = true;
    return;
  }

  //button is up
  if (wasButtonDown) {
    //click happened, lets see if it was short click, long click or just too short
    if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
      longClicked = true;
    } else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
      shortClicked = true;
      curLev++;
      if (curLev > maxLev) curLev = 0;
    }
  }
  wasButtonDown = false;
}


