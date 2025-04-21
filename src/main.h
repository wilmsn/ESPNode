/*

On Branch: main@github  !!!!!

*/
#include "common.h"
#include "secrets.h"

#ifdef USE_FTP
#ifdef CONFIG_IDF_TARGET_ESP32S3
#ifdef ESP32
#warning "ESP32 is defined"
#else
#define ESP32
#endif
//#define DEFAULT_FTP_SERVER_NETWORK_TYPE_ESP32 		NETWORK_ESP32
//#define DEFAULT_STORAGE_TYPE_ESP32 					STORAGE_SD
//#define DEFAULT_STORAGE_TYPE_ESP32 					STORAGE_LITTLEFS
#endif
#include <FtpServer.h>
#endif

#ifdef ESP32
//ToDo
#else
#ifndef ANALOGINPUT
ADC_MODE(ADC_VCC);
#endif
#endif


// Definiert in den Modulen
// MQTT
extern String mqtt_topicP2;
void callback_mqtt(char* topic, byte* payload, unsigned int length);
void send_mqtt_stat();
void send_mqtt_tele();
void reconnect_mqtt();
// Webserver
void setup_webserver();

// Zeitmanagement
/// @brief Der NTP Server
const char* ntp_server = NTP_SERVER;
/// @brief TimeZone Info
//const char* tz_info = TZ_INFO;

int rssi_quality;
int rssi;


// preferences
/// @brief  @brief Ein Objekt für die Preferences
Preferences preferences;
int magicno;

/// @brief Eine Variable zur Aufnahme der Preferences Struktur
//prefs_t preference;
/// @brief Das reboot Flag, ist es auf "true" wird im nächsten Loop Durchgang der Node neu gestartet.
bool rebootflag = false;

// WiFi
String wifi_ssid;
String wifi_pass;
#ifdef ESP32
String wifi_ssid1;
String wifi_pass1;
String wifi_ssid2;
String wifi_pass2;
#endif

bool ap_mode = false;

// Logging
bool do_log_module;
bool do_log_system;
bool do_log_critical;
bool do_log_web;

/// @brief Ein String zum Einsatz in der Funktion write2log. Darf nicht genutzt werden wenn diese Funktion mit gefülltem String aufgerufen wird!
String log_str;

/// @brief Ein String zur temporären Nutzung im Programm.
//String tmp_str;

/// @brief Ein fixes Array zur Aufnahme des Zeitstempels
char timeStr[16];

/// @brief Ein fixes Array zur Aufnahme des Log-Kategorie
char katStr[7];

// Schleifensteuerung

/// @brief Zeitpunkt der letzten Statusdatenerstellung 
unsigned long mqtt_last_stat = 0;
/// @brief Zeitpunkt der letzten Telemetriedatenübertragung
unsigned long mqtt_last_tele = 0;
/// @brief Startzeitpunkt des Messvorgangs
//unsigned long measure_starttime = 0;
/// @brief
unsigned long loop_starttime = 0;
/// @brief
unsigned long wifi_ap_starttime = 0;
int lastHour = 0;
int lastDay  = 0;
unsigned long loop_time_alarm;

char loopmsg[30];

/// @brief Ein Objekt zur Verwaltung der uptime
Uptime uptime;
/// @brief Eine Variable zur Aufnahme der Zeitzoneninfo
tm timeinfo;
/// @brief Eine Variable für Zeitinformationen
time_t now;

#ifdef USE_AUDIO_MEDIA
#include "FS.h"
#include "SD.h"
#if defined(CONFIG_IDF_TARGET_ESP32S3)
// ESP32S3: SD Card mit HW-SPI (ok bei 3V3 Adaptern)
//#define SD_SCK                        12
//#define SD_MISO                       13 
//#define SD_MOSI                       11 
#ifndef SD_CS
#define SD_CS                           10
#endif
#endif
#if defined(CONFIG_IDF_TARGET_ESP32)
// ESP32: SD Card mit HW-SPI (ok bei 3V3 Adaptern)
//#define SD_SCK                        18
//#define SD_MISO                       19
//#define SD_MOSI                       23 
#ifndef SD_CS
#define SD_CS                           5
#endif
#endif
uint64_t sd_cardsize;
uint64_t sd_usedbytes;
uint8_t sd_cardType;
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
void write2log(uint8_t kat, int count, ...);

/// @brief Beziehen der Netzzeit
/// @param sec Wartezeit für die Syncronisation
/// @return true = erfolgreich; false = nicht erfolgreich
bool getNTPtime(long unsigned int sec);

/// @brief Connect or reconnect to WIFI
void wifi_con(void);

/****************************************************************
 * Funktionen für den Webserver
 ***************************************************************/






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
