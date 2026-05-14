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
#define MAGICNO                  344

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Flurlicht", "licht", "licht", false, true, true, 0, 150, 255, 1, "Helligkeit", "intensity", "intensity", true);

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1","Temperatur","Temperatur");

#define MQTT_CLIENT              "flurnode"
#define MQTT_TOPICP2             "flurnode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                102

#endif
//****************************************************
#if defined(NODE_TERASSE)
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_SENSOR_18B20
#include "sensor_18B20.h"

//#define DEBUG_SERIAL_MODULE
//#define DEBUG_SERIAL_WEB

#define MAGICNO                  200

#define HOSTNAME                 "TerassenNode"
#define HOST_DISCRIPTION         "Der Node auf der Terasse"

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Terasse", "terasse", "terasse", false, false, false, 2, true);

#define MODULE2_DEFINITION       Switch_OnOff module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("sw2", "Balkon", "balkon", "balkon", false, false, false, 0, true);

#define MODULE3_DEFINITION       Sensor_18B20 module3;
#define MODULE3_BEGIN_STATEMENT  module3.begin("out1","Temperatur","Temperatur");

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
#define MAGICNO                  416

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Teichpumpe", "pumpe", false, false, true, 0, 2, true);

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1","Temperatur","temperatur");

#define MQTT_CLIENT              "teichnode"
#define MQTT_TOPICP2             "teichnode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                101

#endif
//-----------------------------------------------------
#if defined(NODE_WOHNZIMMERLICHT)
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"

#define MAGICNO                  200
#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT

#define HOSTNAME                 "WohnzimmerLicht"
#define HOST_DISCRIPTION         "Das Licht im Wohnzimmer"

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Wohnzimmerlicht", "wohnzimmerlicht", "wohnzimmerlicht", false, false, false, 12, true);
//#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Wohnzimmerlicht", "wohnzimmerlicht", "wohnzimmerlicht", false, false, false, 12, 0, true, true);
/*    void begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
               bool _start_value, bool _on_value, bool _is_state, uint8_t _hw_pin_relais, uint8_t _hw_pin2_taster, 
               bool _taster_ruhezustand, bool _show_diagramm = false); */

#define MODULE2_DEFINITION       Switch_OnOff module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("sw2", "WohnzimmerTest", "wohnzimmertest", "wohnzimmertest", false, false, false, 13, true);

#define MQTT_CLIENT              "wohnzimmerlicht"
#define MQTT_TOPICP2             "wohnzimmerlicht"

#endif
//*****************************************************
#if defined(NODE_WOHNZIMMER)

#define USE_ACTOR_LEDMATRIX
#include "actor_ledmatrix.h"

#define USE_SENSOR_18B20
#include "sensor_18B20.h"

#define HOSTNAME                 "wohnzimmernode"
#define HOST_DISCRIPTION         "Node mit LED Matrix"
#define MAGICNO                  288
#define DO_LOG_CRITICAL          true

#define MODULE1_DEFINITION       Actor_LEDMatrix module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anzeige", "display", true, true, true, 3, 1, "Helligkeit", "intensity", "mx_line", "mx_graph", false);

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1", "Temperatur", "Temperatur");

#define MQTT_CLIENT              "wohnzimmernode"
#define MQTT_TOPICP2             "wohnzimmernode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                103
#define DO_LOG_RF24              true

#endif

//-----------------------------------------------------

#if defined(NODE_KUECHENRADIO)
// Nur lauffähig auf ESP32 !!!
#ifdef ESP32
#define USE_AUDIOMODUL
#define USE_ROTARY
#define USE_DISPLAY_GC9A01A
#define TFT_ROT                    3
#define USE_AUDIO_RADIO
//#define USE_WIFIMULTI

#include "audiomodul.h"
#define MAGICNO                    89
#define ROTARY_ENCODER_A_PIN       1
#define ROTARY_ENCODER_B_PIN       2
#define ROTARY_ENCODER_SW_PIN      3
#define ROTARY_ENCODER_VCC_PIN     -1
#define ROTARY_ENCODER_STEPS       4
#define ROTARY_ENCODER_R_PULLDOWN  false
//#define I2S_BCLK                 0
//#define I2S_LRC                  0
//#define I2S_DOUT                 0

#define DEBUG_SERIAL
//#define DEBUG_SERIAL_MODULE
//#define DEBUG_SERIAL_WEB

#define HOSTNAME                 "Kuechenradio"
#define HOST_DISCRIPTION         "Radio in der Kueche"

#define MODULE1_DEFINITION       AudioModul module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anlage", "anlage", "anlage", true);

#define DO_LOG_WEB               false
#define DO_LOG_MODULE            false
#define DO_LOG_SYSTEM            false

#else
#warning "Audio läuft nur auf dem ESP32"
#endif
#endif
//-------------------------------------------------------
#if defined(NODE_WOHNZIMMERRADIO)

// Nur lauffähig auf ESP32 !!!
#ifdef ESP32
//#define USE_WIFIMULTI
#define USE_AUDIOMODUL
#define USE_AUDIO_RADIO
#define USE_AUDIO_MEDIA
#define USE_DISPLAY_GC9A01A
#define USE_ROTARY
//#define USE_BOOTMESSAGE
#define USE_AUDIO_RADIO

#define TFT_ROT                  4

#define MAGICNO                  78

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_WEB

#define DO_LOG_WEB               false
#define DO_LOG_MODULE            false
#define DO_LOG_SYSTEM            false

#define HOSTNAME                 "Wohnzimmerradio"
#define HOST_DISCRIPTION         "Ein Mediaplayer im Wohnzimmer"

#include "audiomodul.h"
#define MODULE1_DEFINITION       AudioModul module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anlage", "anlage", "anlage", true);

#else
#error "Audio läuft nur auf dem ESP32 / ESP32-S3"
#endif
#endif
//-----------------------------------------------------

//*****************************************************
//    Testnodes
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//    NODESIMPLE
//    Ein Node ohne externe Elemente.
//    Lauffähig auf ESP8266 oder ESP32,ESP32S3,...    
//-----------------------------------------------------
#ifdef NODEMINIMAL
#define HOSTNAME               "nodeminimal"
#define HOST_DISCRIPTION       "Ein minimaler Node als Zwischenschritt zum Update auf 1M ESP8266"
#define MAGICNO                 0
#define MINIMALNODE             true
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//    NODESIMPLE
//    Ein Node ohne externe Elemente.
//    Lauffähig auf ESP8266 oder ESP32,ESP32S3,...    
//-----------------------------------------------------
#ifdef NODESIMPLE
#ifdef CONFIG_IDF_TARGET_ESP32S3
#include "actor_neopixel.h"
#else
#include "switch_onoff.h"
#endif
#define HOSTNAME               "nodesimple"
#define HOST_DISCRIPTION       "Ein Node ohne externe Elemente"
#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT
#ifdef CONFIG_IDF_TARGET_ESP32S3
#define MODULE1_DEFINITION      Actor_NeoPixel module1;
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "Neopixel", "rgb", 48, 1);
#else
#define MODULE1_DEFINITION      Switch_OnOff module1;
#ifdef CONFIG_IDF_TARGET_ESP32
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", false, true, false, LED_BUILTIN, false);
#else
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", false, false, false, LED_BUILTIN, false);
#endif //CONFIG_IDF_TARGET_ESP32
#endif //CONFIG_IDF_TARGET_ESP32S3
#define DO_LOG_SYSTEM           true
#define MAGICNO                 0

#endif
//-----------------------------------------------------
//****************************************************
// Hier ein Node mit Temperatursensor 18B20.
// Der Temperatursensor MUSS an GPIO 4 angeschlossen sein oder die Datei "sensor_18B20.cpp" muss abgeändert werden!
#ifdef NODE18B20
//#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
//#define USE_SENSOR_18B20
#include "sensor_18B20.h"

#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT

#define HOSTNAME                 "Node18B20"
#define HOST_DISCRIPTION         "Ein Testnode für den Temperatursensor 18B20"
#define MAGICNO                  0

#define MQTT_CLIENT              "node18b20"
#define MQTT_TOPICP2             "node18b20"

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "int. LED", "led", false, false, false, 2, false);

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1","Temperatur","Temperatur");

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
#define MODULE2_BEGIN_STATEMENT  module2.begin("sw1", "interne LED", "led1", "led1", false, false, false, 2, false);

#define MODULE3_DEFINITION       Switch_OnOff module3;
#define MODULE3_BEGIN_STATEMENT  module3.begin("sw2", "zweite LED", "led2", "led2", false, false, true, 0, false);

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

#define MAGICNO                0

#define HOSTNAME               "wittynode"
#define HOST_DISCRIPTION       "A Witty Node"

//#define MQTT_CLIENT            "WittyNode"
//#define MQTT_TOPICP2           "wittynode"

#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT
#define DEBUG_SERIAL_SYSTEM

#define DO_LOG_MQTT              true
#define DO_LOG_MODULE            true
#define DO_LOG_WEB               true
#define DO_LOG_SYSTEM            true

#define MODULE1_DEFINITION      Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "LED int.", "int_led", false, false, true, WITTY_LED_PIN, true);

#define MODULE2_DEFINITION      Switch_OnOff module2;
#define MODULE2_BEGIN_STATEMENT module2.begin("sw2", "RGB rt", "rot", false, true, false, WITTY_RGB_RT, 100, 100, 1, "Rot Helligkeit", "rot_sl", true);

#define MODULE3_DEFINITION      Switch_OnOff module3;
#define MODULE3_BEGIN_STATEMENT module3.begin("sw3", "RGB gn", "gruen", false, true, false, WITTY_RGB_GN, 100, 100, 2, "Grün Helligkeit", "gruen_sl", true);

#define MODULE4_DEFINITION      Switch_OnOff module4;
#define MODULE4_BEGIN_STATEMENT module4.begin("sw4", "RGB bl", "blau", false, true, false, WITTY_RGB_BL, 100, 100, 3, "Blau Helligkeit", "blau_sl", true);

#define MODULE5_DEFINITION      Sensor_LDR module5;
#define MODULE5_BEGIN_STATEMENT module5.begin("out1", "LDR", "ldr");

#endif
//-----------------------------------------------------
//****************************************************
// Hier ein Node mit ESP32 S3 DevKitC 1 ohne externe Bauteile.
// Der ESP32S3 hat eine interne LED (GPIO 49), und eine RGB LED auf Basis des WS2812/Neopixel (GPIO 48).
// Jede LED kann ein- oder ausgeschaltet werden. Die maximale Helligkeit der RGB Leds wird mittels
// Schieberegler eingestellt.
#if defined(ESP32S3_NODE)
#ifdef CONFIG_IDF_TARGET_ESP32S3
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_ACTOR_NEOPIXEL
#include "actor_neopixel.h"

#define HOSTNAME               "esp32s3node"
#define HOST_DISCRIPTION       "Ein Node auf Basis ESP32S3 ohne externe Elemente"
//#define DEBUG_SERIAL_WEB
//#define DEBUG_SERIAL_MODULE
//#define DEBUG_SERIAL_MQTT
#define DEBUG_SERIAL
#define MODULE1_DEFINITION      Actor_NeoPixel module1;
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "Neopixel", "rgb", 48, 1);

#define DO_LOG_SYSTEM           true
#define MAGICNO                 0

#endif //CONFIG_IDF_TARGET_ESP32S3
#endif
//-----------------------------------------------------
#if defined(NODE_AUDIO)

#ifdef ESP32
//#define USE_AUDIOMODUL
#define USE_DISPLAY_GC9A01A
#define USE_AUDIO_RADIO
//#define USE_AUDIO_MEDIA
//#define USE_FTP
//#define USE_WIFIMULTI
#define USE_ROTARY
#define ROTARY_ENCODER_VCC_PIN     -1
#define ROTARY_ENCODER_STEPS       4
#define ROTARY_ENCODER_R_PULLDOWN  false
#define TFT_ROT                 0
//#define ROT_SW                  33
//#define ROT_S1                  35
//#define ROT_S2                  34

#include "audiomodul.h"
#define MAGICNO                 0

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL

#define HOSTNAME                 "Audiotestnode"
#define HOST_DISCRIPTION         "Ein Audio Testnode"

#define MODULE1_DEFINITION       AudioModul module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anlage", "anlage", false);

#define DO_LOG_WEB               true
#define DO_LOG_MODULE            true
#define DO_LOG_SYSTEM            true

#else
#warning "Audio läuft nur auf dem ESP32-S3"
#endif
#endif

//-----------------------------------------------------
#if defined(NODE_TTGO_T_DISPLAY)

#ifdef ESP32
#define USE_AUDIOMODUL
#define USE_AUDIODISPLAY_ST7789
#define USE_AUDIODISPLAY
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

#endif
//_NODE_SETTINGS_H_