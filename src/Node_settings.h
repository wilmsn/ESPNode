#ifndef _NODE_SETTINGS_H_
#define _NODE_SETTINGS_H_

/******************************************************
The following settings ca be used for the individual node:
(m) = mandatory to enable; (o) = optional

++++++++Debugging:+++++++++++
Enable RF24 debugging
(o) #define DEBUG_SERIAL_RF24
Enable Sensor debugging
(o) #define DEBUG_SERIAL_SENSOR

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
#define MAGICNO                  480

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Flurlicht", "licht", "licht", false, true, 0, 150, 1, "intensity", "Helligkeit");

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1","Temperatur","Temperatur");

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

#define SWITCH1_DEFINITION       Switch_OnOff switch1;
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "Terasse", "terasse", "terasse", false, false, 2);

#define SWITCH2_DEFINITION       Switch_OnOff switch2;
#define SWITCH2_BEGIN_STATEMENT  switch2.begin("sw2", "Balkon", "balkon", "balkon", false, false, 0);

#define SENSOR1_DEFINITION       Sensor_18B20 sensor1;
#define SENSOR1_BEGIN_STATEMENT  sensor1.begin("sens1","Temperatur","Temperatur");

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

#define SWITCH1_DEFINITION       Switch_OnOff switch1;
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "Teichpumpe", "pumpe", "relais", false, false, 0, 2);

#define SENSOR1_DEFINITION       Sensor_18B20 sensor1;
#define SENSOR1_BEGIN_STATEMENT  sensor1.begin("sens1","Temperatur","Temperatur");

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
#define MAGICNO                  103
#define DO_LOG_CRITICAL          true

#define MODULE1_DEFINITION       Actor_LEDMatrix module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anzeige", "display", "display", false, true, 3, 1, "intensity", "Helligkeit", "mx_line", "mx_graph");

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1","Temperatur","Temperatur");

#define MQTT_CLIENT              "wohnzimmernode"
#define MQTT_TOPICP2             "wohnzimmernode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                102
#define DO_LOG_RF24              true

#endif
//*****************************************************
//    Testnodes
//-----------------------------------------------------
#ifdef ESP8266SIMPLE

#define USE_SWITCH_ONOFF
#include "switch_onoff.h"

#define HOSTNAME               "nodesimple"
#define HOST_DISCRIPTION       "Ein ESP8266 Node ohne externe Elemente"
#define DEBUG_SERIAL_HTML
#define DEBUG_SERIAL_SENSOR
#define DEBUG_SERIAL_MQTT
#define MODULE1_DEFINITION      Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, false, 2);

#endif
//-----------------------------------------------------
#ifdef ESP32SIMPLE
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"

#define HOSTNAME               "nodesimple"
#define HOST_DISCRIPTION       "Ein ESP32 Node ohne externe Elemente"
#define DEBUG_SERIAL_HTML
#define DEBUG_SERIAL_SENSOR
#define DEBUG_SERIAL_MQTT
#define MODULE1_DEFINITION      Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, true, LED_BUILTIN);
#define DO_LOG_SYSTEM           true
#define MAGICNO                 0

#endif
//-----------------------------------------------------
//****************************************************
// Hier ein Node mit Temperatursensor 18B20.
// Der Temperatursensor MUSS an GPIO 4 angeschlossen sein oder die Datei "sensor_18B20.cpp" muss abgeändert werden!
#ifdef NODE_18B20_TEST
#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_SENSOR_18B20
#include "sensor_18B20.h"

#define DEBUG_SERIAL_HTML
#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT

#define HOSTNAME                 "Node 18B20 Test"
#define HOST_DISCRIPTION         "Ein Testnode für den Temperatursensor 18B20"
#define MAGICNO                  0

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "int. LED", "led", "led", false, false, 2);

#define MODULE2_DEFINITION       Sensor_18B20 module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("out1","Temperatur","Temperatur");

#endif
//****************************************************
// Hier ein Witty Node, dessen 4 LED (interne + RGB) als einzelne Schalter angesteuert werden.
// Dabei kann jede LED nur ein- oder ausgeschaltet werden.
#ifdef WITTYNODE

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

#define DEBUG_SERIAL_HTML
#define DEBUG_SERIAL_WEB
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT

#define MODULE1_DEFINITION      Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, false, WITTY_LED_PIN);

#define MODULE2_DEFINITION      Switch_OnOff module2;
#define MODULE2_BEGIN_STATEMENT module2.begin("sw2", "RGB rot", "rot", "rot", false, true, WITTY_RGB_RT);

#define MODULE3_DEFINITION      Switch_OnOff module3;
#define MODULE3_BEGIN_STATEMENT module3.begin("sw3", "RGB gruen", "gruen", "gruen", false, true, WITTY_RGB_GN);

#define MODULE4_DEFINITION      Switch_OnOff module4;
#define MODULE4_BEGIN_STATEMENT module4.begin("sw4", "RGB blau", "blau", "blau", false, true, WITTY_RGB_BL);

#define MODULE5_DEFINITION      Sensor_LDR module5;
#define MODULE5_BEGIN_STATEMENT module5.begin("out1", "ldr");

#endif
//-----------------------------------------------------
#ifdef NODEBOSCH

#define USE_SWITCH_ONOFF
#include "switch_onoff.h"
#define USE_SENSOR_BOSCH
#include "sensor_bosch.h"

#define MAGICNO                  0
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_HTML

#define HOSTNAME                 "BoschNode"
#define HOST_DISCRIPTION         "Ein Bosch Sensor Testnode mit RF24 Gatway"

#define MODULE1_DEFINITION       Sensor_Bosch module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("out1","Temperatur","Temp","out2","Luftdruck","Pres","out3","Luftfeuchte","Humi");

#define MODULE2_DEFINITION       Switch_OnOff module2;
#define MODULE2_BEGIN_STATEMENT  module2.begin("sw1", "interne LED", "led1", "led1", false, false, 2);

#define MODULE3_DEFINITION       Switch_OnOff module3;
#define MODULE3_BEGIN_STATEMENT  module3.begin("sw2", "zweite LED", "led2", "led2", false, false, 0);

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
#ifdef NODESLIDER

#define USE_SWITCH_ONOFF
#include "switch_onoff.h"

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_HTML

#define HOSTNAME                 "SliderNode"
#define HOST_DISCRIPTION         "Ein Slider Testnode"

#define MODULE1_DEFINITION       Switch_OnOff module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "blau", "blau", "blau", false, true, 13, 150, 1, "intensity", "intensity");

#endif
//-----------------------------------------------------
#if defined(NODE_AUDIO)

#define USE_AUDIOMODUL
#define DISPLAY_GC9A01A

#include "audiomodul.h"
#define MAGICNO                  0

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_HTML

#define HOSTNAME                 "Audionode"
#define HOST_DISCRIPTION         "Ein Audio Testnode"

#define MODULE1_DEFINITION       AudioModul module1;
#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "Anlage", "anlage", "anlage");

#define DO_LOG_WEB               true
#define DO_LOG_MODULE            true
#define DO_LOG_SYSTEM            true

#endif

//-----------------------------------------------------
#if defined(NODE_ROTARYTEST)


#define USE_ROTARYTEST
#define USE_ROTARYMODUL
#include "rotarytest.h"
#define MAGICNO                  0

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_HTML

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
#if defined(DEBUG_SERIAL_SENSOR)
#define DEBUG_SERIAL
#endif
#if defined(DEBUG_SERIAL_RF24)
#define DEBUG_SERIAL
#endif
#if defined(DEBUG_SERIAL_HTML)
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
#if defined(DO_WEBRADIO) || defined(DO_MEDIAPLAYER) || defined(DO_BTSPEAKER)
#define AUDIOBOX
#warning "do we need this?"
#endif


#endif
//_NODE_SETTINGS_H_