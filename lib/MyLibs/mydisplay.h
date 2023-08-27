#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Fonts/FreeSans9pt7b.h>
#include <SPI.h>

#define LINESIZE 20

class MyDisplay {

  public:
    MyDisplay();
    void begin();
    void loop();
    void clear();
    void writeLine(uint8_t line, const char *text, uint16_t color, bool textWrap);
    void displayVol(uint8_t myVol);

  private:
//   Adafruit_ST7735 tft;

};
