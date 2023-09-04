/*

On Branch: main@github  !!!!!

*/

#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef ESP32
#include <Preferences.h>
#include <WiFi.h>
#include "AsyncTCP.h"
#include "Update.h"
#include <rom/rtc.h>
typedef unsigned char uint8_t;
#else
#include <PreferencesESP8266.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncTCP.h"
#endif

#include <LittleFS.h>
#include <WiFiUdp.h>
#include <time.h>
#include <ESPAsyncWebServer.h>
#include <Uptime.h>
#include "config.h"
#include "Node_settings.h"
#include "secrets.h"

#ifdef ESP32
//ToDo
#else
#ifndef ANALOGINPUT
ADC_MODE(ADC_VCC);
#endif
#endif

#if defined(MQTT)
#include <PubSubClient.h>
#endif

#if defined(RF24GW)
#include <RF24.h>
#endif

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
/// @brief Create AsyncWebServer object on port 80
AsyncWebServer httpServer(80);
/// @brief Ein Server für die Websockets
AsyncWebSocket ws("/ws");


#if defined(RF24GW)
/// @brief Der Payload vom Typ payload_t
payload_t payload;
/// @brief Funkkennung Node in Richtung Hub
uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
/// @brief Funkkennung Hub in Richtung Node
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
/// @brief Ein Objekt zur Verwaltung des RF24 Interfaces
/// @param  Radio CE Pin
/// @param  Radio CSN Pin
RF24 radio(RF24_RADIO_CE_PIN, RF24_RADIO_CSN_PIN);
#endif

#if defined(MQTT)
/// @brief Ein Objekt für den MQTT Wifi Client
WiFiClient mqtt_wifi_client;
/// @brief Ein Objekt als MQTT Client
/// @param   Der Wifi Client
PubSubClient mqttClient(mqtt_wifi_client);
/// @brief In diesem String wird der Topic der aktuellen MQTT Message gespeichert
String mqtt_topic;
/// @brief Ein String zur Aufnahme der Informationen die als JSON übertragen werden 
String mqtt_json;
/// @brief Variable zur Zwischenspeicherung der aktuellen JSON Länge
unsigned int mqtt_json_length_old;
#endif

/// @brief  @brief Ein Objekt für die Preferences
Preferences preferences;

/// @brief Ein Objekt für udp Daten
WiFiUDP udp;
/// @brief Eine Variable zur Aufnahme der Preferences Struktur
//prefs_t preference;
/// @brief Das reboot Flag, ist es auf "true" wird im nächsten Loop Durchgang der Node neu gestartet.
bool rebootflag = false;

/// @brief Variablen zur Nutzung im Umfeld der HTML Seite
String html_json;

// WiFi
String wifi_ssid;
String wifi_pass;

// Mqtt
#if defined(MQTT)
bool do_mqtt;
String mqtt_server;
String mqtt_client;
String mqtt_topicP2;
bool do_log_mqtt;
#endif

// RF24GW
#if defined(RF24GW)
bool do_rf24gw;
String rf24gw_hub_server;
int rf24gw_hub_port;
int rf24gw_gw_port;
int rf24gw_gw_no;
bool do_log_rf24;
#endif

bool ap_mode = false;
int cmd_no = 0;
bool cmd_valid = false;

// Logging
bool do_log_sensor;
bool do_log_sys;
bool do_log_web;
bool do_log_critical;
int magicno;

/// @brief Ein String zum Einsatz in der Funktion write2log. Darf nicht genutzt werden wenn diese Funktion mit gefülltem String aufgerufen wird!
String debug_str;

/// @brief Ein String zur temporären Nutzung im Programm.
String tmp_str;

/// @brief Ein fixes Array zur Aufnahme des Zeitstempels
char timeStr[16];

// Schleifensteuerung

/// @brief Zeitpunkt der letzten Statusdatenerstellung 
unsigned long last_stat = 0;
/// @brief Zeitpunkt der letzten Telemetriedatenübertragung
unsigned long mqtt_last_tele = 0;
/// @brief Startzeitpunkt des Messvorgangs
unsigned long measure_starttime = 0;
/// @brief
unsigned long loop_starttime = 0;
/// @brief Anzahl der Loopschleifendurchgänge werden hier festgehalten
unsigned int loopcount = 0;
/// @brief Ein Flag zur Triggerung der MQTT Status Daten
bool do_send_mqtt_stat = false;
/// @brief Ein Flag zur Triggerung der MQTT Telemetrie Daten
bool do_send_mqtt_tele = false;
/// @brief Ein Flag der anzeigt ob die Messdatengenerierung gestartet ist
bool measure_started = false;
int lastHour = 0;
int lastDay  = 0;
unsigned long loop_time_alarm;

char loopmsg[30];


/// @brief "true" bei Veränderung der Preferences, sonst "false"
bool preferencechange = false;

/// @brief Ein Objekt zur Verwaltung der uptime
Uptime uptime;
/// @brief Eine Variable zur Aufnahme der Zeitzoneninfo
tm timeinfo;
/// @brief Eine Variable für Zeitinformationen
time_t now;

#if defined(RF24GW)
/// @brief Eine Variable zur Aufnahme der UDP Daten Struktur
udpdata_t udpdata;
#endif

/// @brief Eine Funktion als Dummy die nichts macht
void noop() {}

/// @brief Fügt die gemessene Betriebsspannung dem übergebenen String hinzu.
/// Achtung: ESP32 noch nicht implementiert
void getVcc(String& json);

/// @brief Schreibt Dateien in die LogKanäle
/// @param kat Die Logkategorie dieses Eintrags
/// @param count Anzahl der übergebenen Textblöcke
/// @param Maximal 10 übergebene Textblöcke 
void write2log(log_t kat, int count, ...);

/// @brief Der Handler für Websocket Messages
/// @param arg 
/// @param data 
/// @param len 
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

/// @brief Eventhandler für die Websockets
/// @param server Keine direkte Verwendung
/// @param client Keine direkte Verwendung
/// @param type Der Eventtyp
/// @param arg Wird durchgereicht an handleWebSocketMessage
/// @param data Wird durchgereicht an handleWebSocketMessage
/// @param len Wird durchgereicht an handleWebSocketMessage
void ws_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                void *arg, uint8_t *data, size_t len);

/// @brief Inittialisierung des WebSockets
void initWebSocket();

/// @brief Sendet die angeforderte Seite inkl mime Kodierung an den Webserver
/// @param request Der Webserverrequest
void serveFile(AsyncWebServerRequest *request);

/// @brief Beziehen der Netzzeit
/// @param sec Wartezeit für die Syncronisation
/// @return true = erfolgreich; false = nicht erfolgreich
bool getNTPtime(long unsigned int sec);

/// @brief Connect or reconnect to WIFI
void wifi_con(void);

/// @brief Liest die Voreinstellungen aus dem Filesystem ein.
void read_preferences(void);

/// @brief Sichert die Voreinstellungen ins Filesystem.
void save_preferences(void);

/****************************************************************
 * Funktionen für den Webserver
 ***************************************************************/

/// @brief Verarbeitung der Webserveraufrufe "/cmd?[cmd]=[value]"
/// @param request Der übergebene Datensatz
/// @return Rückmeldung für die Webseite
const char *mk_cmd(AsyncWebServerRequest *request);

/// @brief Startet einen Scan nach verfügbaren Wifi Netzwerken
/// @return Ein Json zur Übergabe an die Webseite
const char *mk_wifiscan(void);

/// @brief Erzeugt ein JSON zur Anzeige der gefundenen Netzwerke
/// @return Ein Json zur Übergabe an die Webseite
const char *mk_wifishow(void);

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
const char *mk_sysinfo3(String& info_str, bool do_mqtt);

/// @brief Der Komandoprozessor. Hier werden alle Befehle in der Form "Kommando = Wert" abgearbeitet
/// @param cmd Das Komando, der bezeichner des Komandos
/// @param value Der Wert für dieses Kommando
void prozess_cmd(const String cmd, const String value);

#if defined(MQTT)

/// @brief Erzeugt das MQTT-Topic aus den übergebenen Teilen Part1, dem MQTT-Client Namen und dem Part3
/// @param part1 Part 1 des Topics ("stat","tele","cmnd")
/// @param part3 Part 3 des Topics (Messwertname, etc.)
/// @return 
const char* mk_topic(const char* part1, const char* part3);

/// @brief Prüft ob MQTT noch verbunden ist, wenn nicht wird verbunden
void reconnect_mqtt();

/// @brief Erzeugt die Statusdaten und verschickt sie
void send_mqtt_stat();

/// @brief Erzeugt die Telemetriedaten und verschickt sie
void send_mqtt_tele();

/// @brief Die Callbackfunktion wird immer dann angesprochen wenn Meldungen für diesen Clienten vorliegen
/// @param topic Das MQTT Topic
/// @param payload Die MQTT Nachricht
/// @param length Die Länge der Nachricht
void callback_mqtt(char* topic, byte* payload, unsigned int length) ;
#endif

/// @brief Logfunktion für RF24 Daten
/// @param senddir Entweder "N>G" oder "G>N"
/// @param pl Der Payload
void writeRf242log(const char* senddir, payload_t pl);

/// @brief Das Setup
void setup();

/// @brief Die Hauptschleife
void loop();

/// @brief Ermittlung des Resetgrundes für den ESP32, da der ESP Core keine ResetReason zurückgibt wird diese hier nachgebildet
/// @return Eine Zeichenfolge mit der Reset Reason
char *getResetReason(char*);
