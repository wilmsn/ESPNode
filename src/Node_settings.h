#ifndef _NODE_SETTINGS_H_
#define _NODE_SETTINGS_H_

/******************************************************
The following settings ca be used for the individual node:
(m) = mandatory to enable; (o) = optional

++++++++Serial Debugging:+++++++++++
Enable Web Debugging
(o) #define DEBUG_SERIAL_WEB
Enable MQTT Debugging
(o) #define DEBUG_SERIAL_MQTT
Enable RF24 debugging
(o) #define DEBUG_SERIAL_RF24
Enable Modul debugging
(o) #define DEBUG_SERIAL_MODULE

++++++++RF24:+++++++++++
RF24 Gateway:
(m) #define RF24GW_NO   <Gateway ID>  //enables the gateway


******************************************************/
//****************************************************
// Produktive Nodes
//****************************************************
#if defined(NODE_FLUR)
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_SENSOR_18B20
#include "sensor_18B20.h"

#define HOSTNAME                 "FlurNode"
#define HOST_DISCRIPTION         "Der Node im Flur: Hintergrundbeleuchtung"
#define MAGICNO                  482

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Flurlicht", "licht", "licht", false, true, true, 0, 150, 255, 1, "Helligkeit", "intensity", "intensity");

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1","Temperatur","Temperatur", 4);

#define MQTT_CLIENT              "flurnode"
#define MQTT_TOPICP2             "flurnode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                103

#endif
//****************************************************
#if defined(NODE_TERASSE)
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_SENSOR_18B20
#include "sensor_18B20.h"

#define MAGICNO                  475

#define HOSTNAME                 "TerassenNode"
#define HOST_DISCRIPTION         "Der Node auf der Terasse"

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Terasse", "terasse", "terasse", false, false, false, 2);

#define MODULE2_DEFINITION       Switch_OnOff module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("sw2", "Balkon", "balkon", "balkon", false, false, false, 0);

#define MODULE3_DEFINITION       Sensor_18B20 module3;
#define MODULE3_BEGIN_STATEMENT  module3.begin("sens1","Temperatur","Temperatur", 4);

#define MQTT_CLIENT              "terassennode"
#define MQTT_TOPICP2             "terassennode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                104

#endif
//*****************************************************
#if defined(NODE_TEICH)
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_SENSOR_18B20
#include "sensor_18B20.h"

#define HOSTNAME                 "TeichNode"
#define HOST_DISCRIPTION         "Der Node zur Steuerung der Teichpumpe"
#define MAGICNO                  479

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Teichpumpe", "pumpe", "relais", false, false, true, 0, 2);

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("sens1","Temperatur","Temperatur", 4);

#define MQTT_CLIENT              "TeichNode"
#define MQTT_TOPICP2             "TeichNode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                101

#endif
//-----------------------------------------------------
#if defined(NODE_WOHNZIMMER)

#define USE_ACTOR_LEDMATRIX
#include "actor_ledmatrix.h"

#define USE_SENSOR_18B20
#include "sensor_18B20.h"

#define HOSTNAME                 "wohnzimmernode"
#define HOST_DISCRIPTION         "Node mit LED Matrix"
#define MAGICNO                  105
#define DO_LOG_CRITICAL          true

#define MODULE1_DEFINITION       Actor_LEDMatrix module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anzeige", "display", "display", false, true, false, 3, 1, "Helligkeit", "intensity", "intensity", "mx_line", "mx_graph");

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1", "Temperatur", "Temperatur", 4);

#define MQTT_CLIENT              "wohnzimmernode"
#define MQTT_TOPICP2             "wohnzimmernode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                102
#define DO_LOG_RF24              true

#endif

//-----------------------------------------------------

#if defined(NODE_KUECHENRADIO)
// Nur lauffähig auf ESP32 !!!
#ifdef ESP32
#define USE_AUDIOMODUL
#define USE_AUDIODISPLAY_GC9A01A
//#define USE_SDCARD
#define USE_AUDIO_RADIO
//#define USE_AUDIO_MEDIA
//#define USE_FTP
//#define USE_WIFIMULTI

#include "audiomodul.h"
#define MAGICNO                  76

//#define DEBUG_SERIAL_MODULE
//#define DEBUG_SERIAL_WEB

#define HOSTNAME                 "Kuechenradio"
#define HOST_DISCRIPTION         "Radio in der Kueche"

#define MODULE1_DEFINITION       AudioModul module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anlage", "anlage", "anlage");

#define DO_LOG_WEB               false
#define DO_LOG_MODULE            false
#define DO_LOG_SYSTEM            false

#else
#warning "Audio läuft nur auf dem ESP32"
#endif
#endif
//-------------------------------------------------------
#if defined(NODE_WOHNZIMMERRADIO)

#define USE_AUDIOMODUL
#define USE_AUDIO_RADIO

#include "audiomodul.h"
#define MAGICNO                  51

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_WEB

#define HOSTNAME                 "Wohnzimmerradio"
#define HOST_DISCRIPTION         "Ein Radionode im Wohnzimmer"

#define MODULE1_DEFINITION       AudioModul module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anlage", "anlage", "anlage");

#define DO_LOG_WEB               true
#define DO_LOG_MODULE            true
#define DO_LOG_SYSTEM            true

#endif
//-----------------------------------------------------

//*****************************************************
//    Testnodes
//-----------------------------------------------------
#ifdef NODESIMPLE
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"

#define HOSTNAME               "nodesimple"
#define HOST_DISCRIPTION       "Ein ESP32 Node ohne externe Elemente"
#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT
#define MODULE1_DEFINITION      Switch_OnOff module1;
#ifdef ESP32
#ifndef LED_BUILTIN
#define LED_BUILTIN   2
#endif
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, true, LED_BUILTIN);
#else
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, false, false, 2);
#endif
#define DO_LOG_SYSTEM           true
#define MAGICNO                 0

#endif
//-----------------------------------------------------
//****************************************************
// Hier ein Node mit Temperatursensor 18B20.
// Der Temperatursensor MUSS an GPIO 4 angeschlossen sein oder die Datei "sensor_18B20.cpp" muss abgeändert werden!
#ifdef NODE18B20
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_SENSOR_18B20
#include "sensor_18B20.h"

#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT

#define HOSTNAME                 "Node18B20"
#define HOST_DISCRIPTION         "Ein Testnode für den Temperatursensor 18B20"
#define MAGICNO                  0

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "int. LED", "led", "led", false, false, true, 2);

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1","Temperatur","Temperatur",4);

#endif
//-----------------------------------------------------
//****************************************************
// Hier ein Node mit einem Bosch Temperatursensor, einer zusätzlichen LED an GPIO 0 und einem RF24-Gateway
// Jede LED kann ein- oder ausgeschaltet werden.
// Je nach verwendetem Bosch Sensor werden bis zu 3 Meßwerte (Temperatur, Luftfeuchte und Lutdruck) angezeigt.
#ifdef NODEBOSCH

#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_SENSOR_BOSCH
#include "sensor_bosch.h"

#define MAGICNO                  0
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_WEB

#define HOSTNAME                 "BoschNode"
#define HOST_DISCRIPTION         "Ein Bosch Sensor Testnode mit RF24 Gatway"

#define MODULE1_DEFINITION       Sensor_Bosch module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("out1","Temperatur","Temperatur","out2","Luftdruck","Luftdruck","out3","Luftfeuchte","Luftfeuchte");

#define MODULE2_DEFINITION       Switch_OnOff module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("sw1", "interne LED", "led1", "led1", false, false, false, 2);

#define MODULE3_DEFINITION       Switch_OnOff module3;
#define MODULE3_BEGIN_STATEMENT  module3.begin("sw2", "zweite LED", "led2", "led2", false, false, false, 0);

#define MQTT_CLIENT              "BoschNode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                198

#define DO_LOG_CRITICAL          true
#define DO_LOG_RF24              true
#define DO_LOG_MQTT              true
#define DO_LOG_MODULE            true
#define DO_LOG_WEB               true
#define DO_LOG_SYSTEM            true

#endif
//-----------------------------------------------------
//****************************************************
// Hier ein Witty Node, dessen 4 LED (interne + RGB) als einzelne Schalter angesteuert werden.
// Dabei kann jede LED ein- oder ausgeschaltet werden. Die maximale Helligkeit der RGB Leds wird mittels
// Schieberegler eingestellt.
#ifdef WITTYNODE

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_WEB

#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_SENSOR_LDR
#include "sensor_ldr.h"

#define WITTY_RGB_RT           15
#define WITTY_RGB_GN           12
#define WITTY_RGB_BL           13
#define WITTY_LED_PIN          2

#define MAGICNO                123

#define HOSTNAME               "wittynode"
#define HOST_DISCRIPTION       "A Witty Node"

#define MQTT_CLIENT            "WittyNode"
#define MQTT_TOPICP2           "wittynode"

#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT
#define DEBUG_SERIAL_SYSTEM

#define DO_LOG_MQTT              true
#define DO_LOG_MODULE            true
#define DO_LOG_WEB               true
#define DO_LOG_SYSTEM            true

#define MODULE1_DEFINITION      Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, false, false, WITTY_LED_PIN);

#define MODULE2_DEFINITION      Switch_OnOff module2;
#define MODULE2_BEGIN_STATEMENT module2.begin("sw2", "RGB rot", "rot", "rot", false, true, false, WITTY_RGB_RT, 100, 100, 1, "Rot-Helligkeit", "rot_sl", "rot_sl");

#define MODULE3_DEFINITION      Switch_OnOff module3;
#define MODULE3_BEGIN_STATEMENT module3.begin("sw3", "RGB gruen", "gruen", "gruen", false, true, false, WITTY_RGB_GN, 100, 100, 2, "Grün Helligkeit", "gruen_sl", "gruen_sl");

#define MODULE4_DEFINITION      Switch_OnOff module4;
#define MODULE4_BEGIN_STATEMENT module4.begin("sw4", "RGB blau", "blau", "blau", false, true, false, WITTY_RGB_BL, 100, 100, 3, "Blau Helligkeit", "blau_sl", "blau_sl");

#define MODULE5_DEFINITION      Sensor_LDR module5;
#define MODULE5_BEGIN_STATEMENT module5.begin("out1", "LDR", 30);

#endif
//-----------------------------------------------------
#if defined(NODE_AUDIO)

#ifdef ESP32
#define USE_AUDIOMODUL
#define AUDIODISPLAY_GC9A01A
//#define USE_SDCARD
#define USE_AUDIO_RADIO
#define USE_AUDIO_MEDIA
#define USE_FTP
#define USE_WIFIMULTI

#include "audiomodul.h"
#define MAGICNO                  76

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_WEB

#define HOSTNAME                 "Audiotestnode"
#define HOST_DISCRIPTION         "Ein Audio Testnode"

#define MODULE1_DEFINITION       AudioModul module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anlage", "anlage", "anlage");

#define DO_LOG_WEB               true
#define DO_LOG_MODULE            true
#define DO_LOG_SYSTEM            true

#else
#warning "Audio läuft nur auf dem ESP32"
#endif
#endif

//-----------------------------------------------------
#if defined(NODE_TTGO_T_DISPLAY)

#ifdef ESP32
#define USE_AUDIOMODUL
#define USE_AUDIODISPLAY_ST7789
#define USE_AUDIODISPLAY
//#define USE_SDCARD
#define USE_AUDIO_RADIO
//#define USE_AUDIO_MEDIA
//#define USE_FTP
//#define USE_WIFIMULTI

#include "audiomodul.h"
#define MAGICNO                  76

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_WEB

#define HOSTNAME                 "Audiotestnode"
#define HOST_DISCRIPTION         "Ein Audio Testnode"

#define MODULE1_DEFINITION       AudioModul module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anlage", "anlage", "anlage");

#define DO_LOG_WEB               true
#define DO_LOG_MODULE            true
#define DO_LOG_SYSTEM            true

#else
#warning "Audio läuft nur auf dem ESP32"
#endif
#endif
//-----------------------------------------------------
#if defined(NODE_ROTARYTEST)


#define USE_ROTARYTEST
#define USE_ROTARYMODUL
#include "rotarytest.h"
#define MAGICNO                  0

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_WEB

#define HOSTNAME                 "Rotarytest"
#define HOST_DISCRIPTION         "Der Rotary steuert die Helligkeit der internen LED"

/*
void RotaryTest::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword)  {
*/
#define MODULE1_DEFINITION      RotaryTest module1;
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "Gerät", "geraet", "geraet");

#define DO_LOG_SYSTEM           true
#define MAGICNO                 0

#endif

//define constrains for precompiler 
//no changes below !!!!!!!!!!!!!!!!

#ifndef MAGICNO
#define MAGICNO               0
#endif

#if defined(RF24GW_NO)
#define RF24GW                true
#endif

#if defined(MQTT_CLIENT)
#define MQTT                  true
#ifndef MQTT_TOPICP2
#define MQTT_TOPICP2          MQTT_CLIENT
#endif
#endif

/// Debug Settings
#if defined(DEBUG_SERIAL_MODULE)
#define DEBUG_SERIAL
#endif
#if defined(DEBUG_SERIAL_RF24)
#define DEBUG_SERIAL
#endif
#if defined(DEBUG_SERIAL_WEB)
#define DEBUG_SERIAL
#endif
#if defined(DEBUG_SERIAL_MQTT)
#define DEBUG_SERIAL
#endif
#if defined(DEBUG_SERIAL_MODULE)
#define DEBUG_SERIAL
#endif

/// Modules
#if defined(LEDMATRIX_DEVICES_X) && defined(LEDMATRIX_DEVICES_Y)
#define LEDMATRIX
#endif
#if defined(MODULE1_DEFINITION)
#define MODULE1
#endif
#if defined(MODULE2_DEFINITION)
#define MODULE2
#endif
#if defined(MODULE3_DEFINITION)
#define MODULE3
#endif
#if defined(MODULE4_DEFINITION)
#define MODULE4
#endif
#if defined(MODULE5_DEFINITION)
#define MODULE5
#endif
#if defined(MODULE6_DEFINITION)
#define MODULE6
#endif

/// Audio
#ifdef USE_AUDIOMODUL
#if defined(CONFIG_IDF_TARGET_ESP32) 
#warning "Compiling Audiomodule with Settings for ESP32"
#define I2S_LRC                         26
#define I2S_BCLK                        27
#define I2S_DOUT                        25
#define ROTARY_ENCODER_A_PIN            34 //38 //37
#define ROTARY_ENCODER_B_PIN            35 //39 //36
#define ROTARY_ENCODER_SW_PIN           36 //40 //35
#define ROTARY_ENCODER_RESISTOR         INPUT_PULLUP
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
#warning "Compiling Audiomodule with Settings for ESP32-S3"
#define I2S_DOUT                        6
#define I2S_BCLK                        5
#define I2S_LRC                         4
// Rotary Encoder
#define ROTARY_ENCODER_A_PIN            1 //38 //37
#define ROTARY_ENCODER_B_PIN            2 //39 //36
#define ROTARY_ENCODER_SW_PIN           3 //40 //35
#define ROTARY_ENCODER_RESISTOR         INPUT_PULLUP
#endif
#endif

#ifdef USE_AUDIODISPLAY_GC9A01A
#define USE_DISPLAY_GC9A01A
#define USE_AUDIODISPLAY
#endif

#ifdef USE_DISPLAY_GC9A01A
#if defined(CONFIG_IDF_TARGET_ESP32) 
#warning "Compiling Display GC9A01A with Settings for ESP32"
//TFT Settings
#define TFT_MISO -1
#define TFT_MOSI 19
#define TFT_SCK  18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4
#define TFT_X  240
#define TFT_Y  135
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
//#warning ESP32S3
#warning "Compiling Display GC9A01A with Settings for ESP32-S3"
#define TFT_SCK                 12
#define TFT_MOSI                11
#define TFT_CS                  8
#define TFT_DC                  9
#endif
#endif


#ifdef USE_DISPLAY_XYZ
#if defined(CONFIG_IDF_TARGET_ESP32) 
#warning "Compiling Display xyz with Settings for ESP32"
//TFT Settings
#define TFT_MISO -1
#define TFT_MOSI 19
#define TFT_SCK  18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4
#define TFT_X  240
#define TFT_Y  135
// SCL (Display) => SCK  
// SDA (Display) => MOSI 
//#define TFT_SCK                 12
//#define TFT_MOSI                11
//#define TFT_CS                  21
//#define TFT_DC                  5
// Rotary Encoder
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
#warning "Compiling Display xyz with Settings for ESP32-S3"
#define TFT_SCK                 12
#define TFT_MOSI                11
#define TFT_CS                   8
#define TFT_DC                   9
#define TFT_RST                  7
#endif
#endif

#endif
//_NODE_SETTINGS_H_