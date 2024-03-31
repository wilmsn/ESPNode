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
#include "switch_onoff.h"
#define HOSTNAME                 "FlurNode"
#define HOST_DISCRIPTION         "Der Node im Flur: Hintergrundbeleuchtung"
#define MAGICNO                  479

#define SWITCH1_DEFINITION       Switch_OnOff switch1;
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "Flurlicht", "licht", "licht", 0, false, true, 150, 1, "intensity");

#include "sensor_18B20.h"
#define SENSOR1_DEFINITION       Sensor_18B20 sensor1;
#define SENSOR1_BEGIN_STATEMENT  sensor1.begin("sens1","Temperatur","Temperatur");

#define MQTT_CLIENT              "flurnode"
#define MQTT_TOPICP2             "flurnode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                103

#endif
//****************************************************
#if defined(NODE_TERASSE)
#include "switch_onoff.h"
#define MAGICNO                  475

#define HOSTNAME                 "TerassenNode"
#define HOST_DISCRIPTION         "Der Node auf der Terasse"

#define SWITCH1_DEFINITION       Switch_OnOff switch1;
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "Terasse", "terasse", "terasse", 2, false, false);

#define SWITCH2_DEFINITION       Switch_OnOff switch2;
#define SWITCH2_BEGIN_STATEMENT  switch2.begin("sw2", "Balkon", "balkon", "balkon", 0, false, false);

#include "sensor_18B20.h"
#define SENSOR1_DEFINITION       Sensor_18B20 sensor1;
#define SENSOR1_BEGIN_STATEMENT  sensor1.begin("sens1","Temperatur","Temperatur");

#define MQTT_CLIENT              "terassennode"
#define MQTT_TOPICP2             "terassennode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                104

#endif
//*****************************************************
#if defined(NODE_TEICH)
#include "switch_onoff.h"
#define HOSTNAME                 "TeichNode"
#define HOST_DISCRIPTION         "Der Node zur Steuerung der Teichpumpe"
#define MAGICNO                  479

#define SWITCH1_DEFINITION       Switch_OnOff switch1;
/*
void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               uint8_t hw_pin1, uint8_t hw_pin2, bool start_value, bool on_value);
*/
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "Teichpumpe", "pumpe", "relais", 0, 2, false, false);

#include "sensor_18B20.h"
#define SENSOR1_DEFINITION       Sensor_18B20 sensor1;
#define SENSOR1_BEGIN_STATEMENT  sensor1.begin("sens1","Temperatur","Temperatur");

#define MQTT_CLIENT              "TeichNode"
#define MQTT_TOPICP2             "TeichNode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                101

#endif
//-----------------------------------------------------
#if defined(NODE_WOHNZIMMER)

#include "actor_ledmatrix.h"

#define DEBUG_SERIAL

#define HOSTNAME                 "wohnzimmernode"
#define HOST_DISCRIPTION         "Node mit LED Matrix"
#define MAGICNO                  475
#define LOG_CRITICAL             true

#define SWITCH1_DEFINITION       Actor_LEDMatrix switch1;
// html_place, label, mqtt_name, keyword, start_value, on_value, slider_val, slider_no, mqtt_line, mqtt_graph, slider_mqtt_name
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "Display", "display", "display", false, true, 3, 1, "mx_line", "mx_graph", "intensity");

#include "sensor_18B20.h"
#define SENSOR1_DEFINITION       Sensor_18B20 sensor1;
#define SENSOR1_BEGIN_STATEMENT  sensor1.begin("sens1","Temperatur","Temperatur");

#define MQTT_CLIENT              "wohnzimmernode"
#define MQTT_TOPICP2             "wohnzimmernode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO                102
#define LOG_RF24                 true

#endif
//*****************************************************
//    Individual settings
//-----------------------------------------------------
#if defined(NODESIMPLE)
#include "switch_onoff.h"
#define HOSTNAME               "nodesimple"
#define HOST_DISCRIPTION       "Ein ESP8266 Node ohne externe Elemente"
//#define MQTT_CLIENT            "espmini"
//#define MQTT_SERVER            "rpi1.fritz.box"
#define DEBUG_SERIAL_HTML
#define DEBUG_SERIAL_SENSOR
#define DEBUG_SERIAL_MQTT
#define SWITCH1_DEFINITION      Switch_OnOff switch1;
#define SWITCH1_BEGIN_STATEMENT switch1.begin("sw1", "interne LED", "int_led", "int_led", 2, false, false);

#endif
//-----------------------------------------------------
#ifdef ESP32SIMPLE
#include "switch_onoff.h"
#define HOSTNAME               "nodesimple"
#define HOST_DISCRIPTION       "Ein ESP32 Node ohne externe Elemente"
//#define MQTT_CLIENT            "esp32mini"
//#define MQTT_SERVER            "rpi1.fritz.box"
#define DEBUG_SERIAL_HTML
#define DEBUG_SERIAL_SENSOR
#define DEBUG_SERIAL_MQTT
#define SWITCH1_DEFINITION      Switch_OnOff switch1;
#define SWITCH1_BEGIN_STATEMENT switch1.begin("sw1", "interne LED", "int_led", "int_led", 2, false, true);
#define LOG_SYS                 true
#define MAGICNO                  471

#endif
//-----------------------------------------------------
// Hier ein Witty Node, dessen 4 LED (interne + RGB) als einzelne Schalter angesteuert werden.
// Dabei kann jede LED nur ein- oder ausgeschaltet werden.
#if defined(WITTYNODE1)
#define WITTY_RGB_RT           15
#define WITTY_RGB_GN           12
#define WITTY_RGB_BL           13
#define WITTY_LED_PIN          2
#include "switch_onoff.h"
#include "sensor_ldr.h"

#define MAGICNO                479

#define HOSTNAME               "wittynode"
#define HOST_DISCRIPTION       "A Witty Node"

#define MQTT_CLIENT            "wittynode"
#define MQTT_TOPICP2           "wittynode"

//#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
//#define RF24GW_NO                155

//#define DEBUG_SERIAL
#define DEBUG_SERIAL_HTML
#define DEBUG_SERIAL_SENSOR
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_MQTT

#define SWITCH1_DEFINITION      Switch_OnOff switch1;
#define SWITCH1_BEGIN_STATEMENT switch1.begin("sw1", "interne LED", "int_led", "int_led", WITTY_LED_PIN, false, false);

#define SWITCH2_DEFINITION      Switch_OnOff switch2;
#define SWITCH2_BEGIN_STATEMENT switch2.begin("sw2", "RGB rot", "rot", "rot", WITTY_RGB_RT, false, true);

#define SWITCH3_DEFINITION      Switch_OnOff switch3;
#define SWITCH3_BEGIN_STATEMENT switch3.begin("sw3", "RGB gruen", "gruen", "gruen", WITTY_RGB_GN, false, true);

#define SWITCH4_DEFINITION      Switch_OnOff switch4;
#define SWITCH4_BEGIN_STATEMENT switch4.begin("sw4", "RGB blau", "blau", "blau", WITTY_RGB_BL, false, true);

#define SENSOR1_DEFINITION      Sensor_LDR sensor1;
#define SENSOR1_BEGIN_STATEMENT sensor1.begin("sens1", "ldr");

#endif
//-----------------------------------------------------
#if defined(NODEBOSCH)

#include "sensor_bosch.h"
#include "switch_onoff.h"

#define MAGICNO                  471
#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_HTML

#define HOSTNAME                 "BoschNode"
#define HOST_DISCRIPTION         "Ein Bosch Sensor Testnode mit RF24 Gatway"

#define SENSOR1_DEFINITION       Sensor_Bosch sensor1;
#define SENSOR1_BEGIN_STATEMENT  sensor1.begin("sens1","Temperatur","Temp","sens2","Luftdruck","Pres","sens3","Luftfeuchte","Humi");

#define SWITCH1_DEFINITION       Switch_OnOff switch1;
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "interne LED", "led1", "led1", 2, false, false);

#define SWITCH2_DEFINITION       Switch_OnOff switch2;
#define SWITCH2_BEGIN_STATEMENT  switch2.begin("sw2", "zweite LED", "led2", "led2", 0, false, false);

#define MQTT_CLIENT              "BoschNode"

#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW_NO               198

#define LOG_CRITICAL            true
#define LOG_RF24                true
#define LOG_MQTT                true

#endif
//-----------------------------------------------------
#if defined(NODEMATRIX)

#include "actor_ledmatrix.h"

#define DEBUG_SERIAL

#define HOSTNAME                 "MatrixNode"
#define HOST_DISCRIPTION         "Ein Matrix Testnode"

#define SWITCH1_DEFINITION       Actor_LEDMatrix switch1;
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "Display", "display", "display",true, true, 3, 1, "mx_line", "mx_graph", "intensity");

#define MQTT_CLIENT              "MatrixNode"

#endif
//-----------------------------------------------------
#if defined(NODESLIDER)

//#include "sensor_ldr.h"
#include "switch_onoff.h"

#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_HTML

#define HOSTNAME                 "SliderNode"
#define HOST_DISCRIPTION         "Ein Slider Testnode"

#define SWITCH1_DEFINITION       Switch_OnOff switch1;
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "blau", "blau", "blau", 13, false, true, 150, 1, "intensity");

//#define MQTT_CLIENT              "SliderNode"

//#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
//#define RF24GW_NO                198

#endif
//-----------------------------------------------------
#if defined(NODE_WEBRADIO)

#include "webradio.h"
#define CONFIG_ESP32S3
//#define CONFIG_ESP32
#define MAGICNO                  471
#define WEBRADIO

//#define DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL_HTML

#define HOSTNAME                 "Webradio"
#define HOST_DISCRIPTION         "Ein Webradio Testnode"

#define SWITCH1_DEFINITION       Webradio switch1;
#define SWITCH1_BEGIN_STATEMENT  switch1.begin("sw1", "radio", "radio", "radio");

//#define MQTT_CLIENT              "RadioNode"

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

/// Actors
#if defined(LEDMATRIX_DEVICES_X) && defined(LEDMATRIX_DEVICES_Y)
#define LEDMATRIX
#endif
#if defined(SWITCH1_DEFINITION)
#define SWITCH1
#endif
#if defined(SWITCH2_DEFINITION)
#define SWITCH2
#endif
#if defined(SWITCH3_DEFINITION)
#define SWITCH3
#endif
#if defined(SWITCH4_DEFINITION)
#define SWITCH4
#endif

/// Sensors
#if defined(SENSOR1_DEFINITION)
#define SENSOR1
#endif
#if defined(SENSOR2_DEFINITION)
#define SENSOR2
#endif


#endif
//_NODE_SETTINGS_H_