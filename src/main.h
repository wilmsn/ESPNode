/*

On Branch: main@github  !!!!!

*/
#include "common.h"
#include "secrets.h"

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



// preferences
/// @brief  @brief Ein Objekt für die Preferences
Preferences preferences;
int magicno;

/// @brief Eine Variable zur Aufnahme der Preferences Struktur
//prefs_t preference;
/// @brief Das reboot Flag, ist es auf "true" wird im nächsten Loop Durchgang der Node neu gestartet.
bool rebootflag = false;

/// @brief Variablen zur Nutzung im Umfeld der HTML Seite
String html_json;
// WiFi
String wifi_ssid1;
String wifi_pass1;
String wifi_ssid2;
String wifi_pass2;

bool ap_mode = false;

// Logging
bool do_log_sensor;
bool do_log_module;
bool do_log_sys;
bool do_log_critical;

/// @brief Ein String zum Einsatz in der Funktion write2log. Darf nicht genutzt werden wenn diese Funktion mit gefülltem String aufgerufen wird!
String log_str;

/// @brief Ein String zur temporären Nutzung im Programm.
//String tmp_str;

/// @brief Ein fixes Array zur Aufnahme des Zeitstempels
char timeStr[16];

// Schleifensteuerung

/// @brief Zeitpunkt der letzten Statusdatenerstellung 
unsigned long mqtt_last_stat = 0;
/// @brief Zeitpunkt der letzten Telemetriedatenübertragung
unsigned long mqtt_last_tele = 0;
/// @brief Startzeitpunkt des Messvorgangs
unsigned long measure_starttime = 0;
/// @brief
unsigned long loop_starttime = 0;
/// @brief
unsigned long wifi_ap_starttime = 0;
/// @brief Ein Flag der anzeigt ob die Messdatengenerierung gestartet ist
bool measure_started = false;
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
