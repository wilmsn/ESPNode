/****************************************************************
 * common.h enthält allgemeine Elemente und kann in jedes
 * Modul eingebunden werden.
 * 
 ****************************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_
#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
//#include <AsyncElegantOTA.h>
#include <LittleFS.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include <Uptime.h>

#ifdef ESP32
#include <Preferences.h>
#include <WiFi.h>
#include "AsyncTCP.h"
#include <rom/rtc.h>
typedef unsigned char uint8_t;
#endif

#ifdef ESP8266
#include <Preferences8266.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncTCP.h"
#endif

#include "config.h"

/// @brief Ein emum für den Logtype. Dieser wird bei write2log übergeben und dort gegen die Voreinstellungen geprüft.
typedef enum {
  log_rf24 = 0,
  log_sys,
  log_mqtt,
  log_sensor,
  log_web,
  log_critical,
  log_daybreak
} log_t;

// externe Referenzen
// Modul: webserver
extern AsyncWebSocket ws;
extern bool do_log_web;

// Modul: mqtt
extern PubSubClient mqttClient;
extern String mqtt_server;
extern String mqtt_client;
extern String mqtt_topicP2;
extern String mqtt_topic;
extern bool do_mqtt;
extern bool do_send_mqtt_stat;
extern bool do_send_mqtt_tele;
extern bool do_log_mqtt;
extern String mqtt_json;
extern unsigned int mqtt_json_length_old;
void mqtt_loop();
void mqtt_setup();

// Modul: rf24gw
extern bool do_rf24gw;
extern bool do_log_rf24;
extern String rf24gw_hub_server;
extern int rf24gw_hub_port;
extern int rf24gw_gw_port;
extern int rf24gw_gw_no;

void rf24gw_setup();
void rf24gw_loop();

// Modul: main
//extern AsyncWebServer httpServer;
extern bool rebootflag;
extern bool do_log_sensor;
extern bool do_log_sys;
extern bool do_log_critical;
extern String wifi_ssid;
extern String wifi_pass;
extern String tmp_str;
extern int cmd_no;
extern Preferences preferences;
extern unsigned long loop_time_alarm;
extern tm timeinfo;
extern String mqtt_topicP2;

/// @brief Erzeugt ein JSON mit Informationen über das System (Teil1)
/// Es wird für die Webseite und für MQTT genutzt.
/// @param Ein String zur Aufnahme der Informationen
/// @return Ein Json zur Übergabe an die Webseite oder MQTT
const char *mk_sysinfo1(String& info_str);

/// @brief Erzeugt ein JSON mit Informationen über das System (Teil2)
/// Es wird für die Webseite und für MQTT genutzt.
/// @param Ein String zur Aufnahme der Informationen
/// @return Ein Json zur Übergabe an die Webseite oder MQTT
const char *mk_sysinfo2(String& info_str);

/// @brief Erzeugt ein JSON mit Informationen über das System (Teil3)
/// Es wird für die Webseite und für MQTT genutzt.
/// @param Ein String zur Aufnahme der Informationen
/// @return Ein Json zur Übergabe an die Webseite oder MQTT
const char *mk_sysinfo3(String& info_str, bool format_mqtt);


#if defined(SWITCH1)
extern SWITCH1_DEFINITION
#endif

#if defined(SWITCH2)
extern SWITCH2_DEFINITION
#endif

#if defined(SWITCH3)
extern SWITCH3_DEFINITION
#endif

#if defined(SWITCH4)
extern SWITCH4_DEFINITION
#endif

#if defined(SENSOR1)
extern SENSOR1_DEFINITION
#endif

#if defined(SENSOR2)
extern SENSOR2_DEFINITION
#endif

void write2log(log_t kat, int count, ...);

/// @brief Der Komandoprozessor. Hier werden alle Befehle in der Form "Kommando = Wert" abgearbeitet
/// @param cmd Das Komando, der bezeichner des Komandos
/// @param value Der Wert für dieses Kommando
void prozess_cmd(const String cmd, const String value);

/// @brief Variablen zur Nutzung im Umfeld der HTML Seite
extern String html_json;

const char* mk_topic(const char* part1, const char* part3);
#endif