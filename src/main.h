/*

On Branch: websockets@Dell_7280  !!!!!

*/

#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef ESP32
#include <WiFi.h>
#include "AsyncTCP.h"
#include <rom/rtc.h>
#else
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

#if defined(MQTT)
#include <PubSubClient.h>
#endif

#if defined(RF24GW)
#include <RF24.h>
#endif


typedef enum {
  ret_html = 0,
  ret_json,
  ret_text,
  ret_stat
} call_t;

typedef enum {
  log_rf24 = 0,
  log_sys,
  log_mqtt,
  log_sens,
  log_web,
  log_critical
} log_t;

typedef struct {
  uint16_t magicno;
  char mqttserver[SERVERNAMESIZE];
  char mqttclient[SERVERNAMESIZE];
  bool log_sensor;
  bool log_mqtt;
  bool log_webcmd;
  bool log_sysinfo;
  bool log_fs_sysinfo;
  bool log_rf24;
  char rf24gw_hub_name[SERVERNAMESIZE];
  uint16_t rf24gw_hub_port;
  uint16_t rf24gw_gw_port;
  uint16_t rf24gw_gw_no;
  bool rf24gw_enable;
  bool rf24node_enable;
  NODE_DATTYPE rf24node_node_id;
  uint8_t rf24node_val1_channel;
  bool rf24node_val2_enable;
  uint8_t rf24node_val2_channel;
  bool rf24node_val3_enable;
  uint8_t rf24node_val3_channel;
} prefs_t;

// Create AsyncWebServer object on port 80
AsyncWebServer httpServer(80);
AsyncWebSocket ws("/ws");

#if defined(RF24GW)
payload_t payload;
uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
RF24 radio(RF24_RADIO_CE_PIN, RF24_RADIO_CSN_PIN);
#endif

#if defined(MQTT)
WiFiClient mqtt_wifi_client;
PubSubClient mqttClient(mqtt_wifi_client);
String mqtt_topic;
String mqtt_json;
unsigned int mqtt_json_length_old;
#endif

WiFiUDP udp;
//Logger logger = Logger(LOGGER_NUMLINES, LOGGER_LINESIZE);
prefs_t preference;
bool rebootflag = false;

// Variablen zur Nutzung im Umfeld der HTML Seite
String html_json;

// Variablen zur generischen Nutzung
//String info_str;
String tmp_str;
String debug_str;
char timeStr[12];

// Schleifensteuerung
//unsigned long sens_last_stat = 0;
/// 
unsigned long last_stat = 0;
unsigned long mqtt_last_tele = 0;
unsigned long measure_starttime = 0;
unsigned long lastalive = 0;
unsigned int loopcount = 0;
bool do_send_mqtt_stat = false;
bool measure_started = false;

// Veränderung der Preferences
bool preferencechange = false;
// Zeitmanagement
const char *NTP_SERVER = "de.pool.ntp.org";
const char *TZ_INFO = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";
// enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)

Uptime uptime;
tm timeinfo;
time_t now;
long unsigned lastNTPtime;
unsigned long lastEntryTime;

#if defined(SENSOR_18B20)
OneWire oneWire(SENSOR_18B20);
DallasTemperature sensor(&oneWire);
float temperature;
uint8_t resolution = 0;
#endif

#if defined(SENSOR_BOSCH)
BMX_SENSOR sensor;
float temp, pres, humi;
#endif

#if defined(ANALOGINPUT)
//int analog_value;
#else
#ifdef ESP32
// tbd interne Spannung messen
#else
ADC_MODE(ADC_VCC);
#endif
#endif

#if defined(RF24GW)
udpdata_t udpdata;
#endif


#if defined(NEOPIXEL)
Adafruit_NeoPixel pixels(NEOPIXELNUM, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
#endif

// Erweiterungen Variablen

// ENDE Erweiterungen Variablen

/*
 * Eine Funktion als Dummy die nichts macht
 */
void noop() {}

/************************************************
 * Füllt den "timeStr" mit der aktuellen Zeit
 ***********************************************/
void fill_timeStr();

/***********************************************************
 * getVcc: Fügt die gemessene Betriebsspannung dem übergebenen 
 *         String hinzu.
 * Achtung: ESP32 noch nicht implementiert
 **********************************************************/
void getVcc(String& json);

/*******************************************************
 * do_restart: Startet das System neu
 ******************************************************/
//const char* do_restart();

/*******************************************************
 * write2log: Schreibt Dateien in die LogKanäle
 *
 *
 ******************************************************/
void write2log(log_t kat, int count, ...);

/******************************************************
 * handleWebSocketMessage: Verarbeitet die Daten,
 * die von der Webseite auf das Websocket gesendet werden.
 * Daten folgen der Form: <item>:<value>
 ******************************************************/
//void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

/**********************************************************
 * Eventverwaltung des Websockets
 *********************************************************/
//void ws_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
//                void *arg, uint8_t *data, size_t len);

/**********************************************
 * Initialisierung des Websockets
 *********************************************/
//void initWebSocket();

/***********************************************
 * ServFile bedient die Webserver URL, die nicht direkt
 * per httpServer.on() festgelegt wurden.
 * Ist die geforderte Datei nicht im Dateisystem
 * dann wird die Datei "index.html" gesendet.
 **********************************************/
//void serveFile(AsyncWebServerRequest *request);

/*
 *
 */
bool getNTPtime(long unsigned int sec);

/**********************************
 * Connect or reconnect to WIFI
 *********************************/
void wifi_con(void);

/*
 * read_preferences:
 * Liest die Voreinstellungen aus dem Filesystem ein.
 */
void read_preferences(void);

/*
 * save_preferences:
 * Sichert die Voreinstellungen ins Filesystem.
 */
void save_preferences(void);

/****************************************************************
 * Funktionen für den Webserver
 ***************************************************************/

//char *mk_cmd(AsyncWebServerRequest *request);

/*
 * Das folgende JSON bildet den Status der Logeinstellungen ab
 * Es wird NUR für die Webseite "settings.html" genutzt
 */
//const char *mk_logcfg(void);

/*
 * Das folgende JSON bildet den Status der Sensoren und Aktoren ab
 * Es wird für die Webseite und für MQTT genutzt
 */
//char *mk_setcfg(void);

/*
 *
 */
const char *mk_wifiscan(void);

/*
 *
 */
const char *mk_wifishow(void);

/*
 * Das folgende JSON bildet den Status der Sensoren und Aktoren ab
 * Es wird für die Webseite und für MQTT genutzt
 */
const char *mk_sysinfo1(void);

/*
 * Das folgende JSON bildet den Status der Sensoren und Aktoren ab
 * Es wird für die Webseite und für MQTT genutzt
 */
const char *mk_sysinfo2(void);

/*
 * Das folgende JSON bildet den Status der Sensoren und Aktoren ab
 * Es wird für die Webseite und für MQTT genutzt
 */
const char *mk_sysinfo3(void);

/// @brief 
/// @param name 
/// @param value 
void prozess_cmd(const String cmd, const String value);

#if defined(MQTT)

const char* mk_topic(const char* part1, const char* part3);

void reconnect_mqtt();

void send_mqtt_stat();

void send_mqtt_tele();

void callback_mqtt(char* topic, byte* payload, unsigned int length) ;
#endif

void setup();

void loop();

/* NUR für ESP32 !!!!!!
 * Da der ESP Core keine ResetReason zurückgibt
 * wird diese hier nachgebildet
 */
char *getResetReason(char *tmp);
