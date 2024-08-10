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
#include <WiFiMulti.h>
typedef unsigned char uint8_t;
#endif

#ifdef ESP8266
#include <Preferences8266.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncTCP.h"
#endif

//#ifdef USE_FTP
#include "ESP-FTP-Server-Lib.h"
#include "FTPFilesystem.h"
//#endif

#include "config.h"

// externe Referenzen

// Alle Module
extern void write2log(uint8_t kat, int count, ...);
extern bool do_log_module;
extern bool do_log_system;
extern bool do_log_critical;
extern bool do_log_web;
extern time_t now;
extern FTPServer        ftp;
extern void sendWsMessage(String& _myMsg);
extern void sendWsMessage(String& _myMsg, uint8_t kat);


// Modul: webserver
extern AsyncWebSocket ws;
extern uint64_t sd_cardsize;
extern uint64_t sd_usedbytes;
extern uint8_t sd_cardType;
extern int rssi;
extern int rssi_quality;
extern char *getResetReason(char *tmp);
extern Uptime uptime;
extern void getVcc(String& json);

// Modul: mqtt
extern PubSubClient mqttClient;
extern String mqtt_server;
extern String mqtt_client;
extern String mqtt_topicP2;
extern String mqtt_topic;
extern bool do_mqtt;
extern bool do_log_mqtt;
extern String mqtt_json;
extern unsigned int mqtt_json_length_old;
void mqtt_loop(time_t now);
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
extern String wifi_ssid;
extern String wifi_pass;
#ifdef ESP32
extern String wifi_ssid1;
extern String wifi_pass1;
extern String wifi_ssid2;
extern String wifi_pass2;
#endif
extern int cmd_no;
extern Preferences preferences;
extern unsigned long loop_time_alarm;
extern tm timeinfo;
extern String mqtt_topicP2;

#if defined(MODULE1)
extern MODULE1_DEFINITION
#endif

#if defined(MODULE2)
extern MODULE2_DEFINITION
#endif

#if defined(MODULE3)
extern MODULE3_DEFINITION
#endif

#if defined(MODULE4)
extern MODULE4_DEFINITION
#endif

#if defined(MODULE5)
extern MODULE5_DEFINITION
#endif

#if defined(MODULE6)
extern MODULE6_DEFINITION
#endif

/// @brief Der Komandoprozessor. Hier werden alle Befehle in der Form "Kommando = Wert" abgearbeitet
/// @param cmd Das Komando, der bezeichner des Komandos
/// @param value Der Wert für dieses Kommando
void prozess_cmd(const String cmd, const String value);

const char* mk_topic(const char* part1, const char* part3);


#endif