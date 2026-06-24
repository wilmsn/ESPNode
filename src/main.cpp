#include "secrets.h"
#include "common.h"

#ifdef ESP32
//TODO: Kann der ESP32 seine eigene Betriebsspannung messen?
#else
#ifndef ANALOGINPUT
ADC_MODE(ADC_VCC);
#endif
#endif

// Display
#ifdef USE_DISPLAY_GC9A01A
Adafruit_GC9A01A* display = NULL;
#endif

#ifdef USE_DISPLAY_ST7796
//Adafruit_ST7796S* display = NULL;
#endif

#ifdef USE_DISPLAY
uint8_t bootline = 0;
#endif
// Ende Display

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
int lastMinute = 0;
int lastHour = 0;
int lastDay  = 0;
unsigned long loop_time_alarm;

/// @brief Ein Array of Char für Messages.
char mymsg[30];

/// @brief Ein Objekt zur Verwaltung der uptime
Uptime uptime;
/// @brief Eine Variable zur Aufnahme der Zeitzoneninfo
tm timeinfo;
/// @brief Eine Variable für Zeitinformationen
time_t now;
/// @brief Eine Variable zur Aufnahme der Minuten seit Start
/// Achtung: Ungenauigkeit wird hier tolleriert
unsigned long minutes = 0; 

/// @brief Fügt die gemessene Betriebsspannung dem übergebenen String hinzu.
/// Achtung: ESP32 noch nicht implementiert
void getVcc(String& json);

/// @brief Schreibt Daten in die LogKanäle
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

/// @brief Logfunktion für RF24 Daten
/// @param senddir Entweder "N>G" oder "G>N"
/// @param pl Der Payload
void writeRf242log(const char* senddir, payload_t pl);

/// @brief Das Setup
void setup();

/// @brief Die Hauptschleife
void loop();

/// @brief Ermittlung des Resetgrundes für den ESP32, der ermittelte Grund wird dem übergebenen String angehängt.
void getResetReason(String& tmp);

bool cmd_result;

#ifdef USE_WIFIMULTI
#ifdef ESP32
WiFiMulti wifiMulti;
#endif
#endif

#if defined(MODULE1)
MODULE1_DEFINITION
#endif
#if defined(MODULE2)
MODULE2_DEFINITION
#endif
#if defined(MODULE3)
MODULE3_DEFINITION
#endif
#if defined(MODULE4)
MODULE4_DEFINITION
#endif
#if defined(MODULE5)
MODULE5_DEFINITION
#endif
#if defined(MODULE6)
MODULE6_DEFINITION
#endif

#ifdef USE_DISPLAY
uint8_t boot_line = 0;
// - txtcolor: 0 = weiss, 1 = grün, 2 = rot
void bootMessage(uint8_t txtcolor, const char* msg, bool newline, bool align_right) {
  if (display) {
    display->setTextSize(BOOTMESSAGE_TEXTSIZE);
    switch (txtcolor) {
    case 0:
      display->setTextColor(TFT_COLOR_WHITE);
    break;
    case 1:
      display->setTextColor(TFT_COLOR_GREEN);
      break;
    case 2:
      display->setTextColor(TFT_COLOR_RED);
      break;
    }
    if (newline) {
      boot_line++;
    }
    if (align_right) {
      display->setCursor(BOOTWINDOW_X + BOOTWINDOW_WIDTH - (10 + (strlen(msg) * BOOTLINE_X)), BOOTWINDOW_Y + ( boot_line * BOOTLINE_Y) );
    } else {
      display->setCursor(BOOTWINDOW_X + 10, BOOTWINDOW_Y + ( boot_line * BOOTLINE_Y) );
    }
    display->print(msg);
  }
}
#endif

void setupTime() {                              
  // deinen NTP Server einstellen (von 0 - 5 aus obiger Liste) alternativ lassen sich durch Komma getrennt bis zu 3 Server angeben
#ifdef ESP32
//  configTzTime("CET-1CEST,M3.5.0/03,M10.5.0/03", ntp_server);
  configTzTime(TZ_INFO, ntp_server);
#else
//  configTime("CET-1CEST,M3.5.0,M10.5.0/3", ntp_server);
  configTime(TZ_INFO, ntp_server);
#endif
  // Zeitzone einstellen https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
}

void getVcc(String& json) {
#if defined(ANALOGINPUT)
  json += "n.a.";
#else
#ifdef ESP32
  json += "n.a.";
#else
  char vcc_str[8];
  snprintf(vcc_str,7,"%.1f V",(float)ESP.getVcc() / 1000.0);
  json += vcc_str;
#endif
#endif
}

void write2log(uint8_t kat, int count, ...) {
  va_list args;
  int n = 0;
  char timestr[16];
  if (count > 12) count = 12; 
  char * c[12];
  // Parameterabfrage initialisieren
  va_start(args, count);
  while (n < count) {
    c[n] = (char *)va_arg(args, char *);
    n++;
  }
  // Im AP-Mode wird nichts gelogged !!!
  if ( ! ap_mode ) {
    snprintf(timestr, 15, "[%02d:%02d:%02d.%03u]", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, (unsigned int)millis()%1000);
    if ( do_log_critical && kat == LOG_DAYBREAK ) {
      File f = LittleFS.open( DEBUGFILE, "a" );
      if (f) {
        f.printf("----%d.%d.%d----\n",timeinfo.tm_mday, 1 + timeinfo.tm_mon, 1900 + timeinfo.tm_year);
        f.close();
      }
    }
    if ( do_log_critical && (kat == LOG_CRITICAL) ) {
      File f = LittleFS.open( DEBUGFILE, "a" );
      if (f) {
        f.print(timestr);
        n = 0;
        while (n < count) {
          f.print(c[n]);
          n++;
        }
        f.printf("\n");
        f.close();
      }
    }
    if (  (do_log_module   && ( kat == LOG_MODULE )) ||
          (do_log_web      && ( kat == LOG_WEB ))    ||
#if defined(MQTT)
          (do_log_mqtt     && ( kat == LOG_MQTT ))   ||
#endif
#if defined(RF24GW)
          (do_log_rf24     && ( kat == LOG_RF24 ))   ||
#endif
          (do_log_system   && ( kat == LOG_SYSTEM )) || 
          (do_log_critical && ( kat == LOG_CRITICAL )) ) {
      log_str = String("{\"log\":\"");
      log_str += timestr;
      switch(kat) {
        case LOG_WEB:       log_str += String("[ WEB]"); break;
        case LOG_SYSTEM:    log_str += String("[ SYS]"); break;
        case LOG_MQTT:      log_str += String("[MQTT]"); break;
        case LOG_RF24:      log_str += String("[RF24]"); break;
        case LOG_MODULE:    log_str += String("[ MOD]"); break;
        case LOG_CRITICAL:  log_str += String("[CRIT]"); break;
        default:            log_str += String("[----]"); break;
      }
      n = 0;
      while (n < count) {
        log_str += String(" ");
        // Anführungszeichen innerhalb des Log-Strings führen zu Frontendproblemen.
        // Deshalb wird " gegen ' ersetzt!
        for ( unsigned int i=0; i < strlen(c[n]); i++ ) {
          if ( c[n][i] == '"' ) {
            log_str += String("'");
          } else {
            log_str += String(c[n][i]);
          }
        }
        n++;
      }
      log_str += String("\"}");
      ws.textAll(log_str);
    }
#if defined(DEBUG_SERIAL)
    Serial.print(timestr);
    n = 0;
    while (n < count) {
      Serial.print(c[n]);
      Serial.print(" ");
      n++;
    }
    Serial.println();
#endif
  }
  va_end(args);
}

// todo Schleifen maximaldauer könnte kritisch sein
bool getNTPtime(long unsigned int sec) {
  bool retval = true;
  uint32_t start = millis();
  do {
    time(&now);
    localtime_r(&now, &timeinfo);
    delay(10);
  } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));
  if (timeinfo.tm_year <= (2016 - 1900)) retval = false; // the NTP call was not successful
  return retval;
}

bool do_wifi_con(void) {
  bool retval = false;
  WiFi.mode(WIFI_STA);
#ifdef ESP32
// Wichtig fuer ESP32S3, sonst wird der Hostname nicht gesetzt!
  WiFi.mode(WIFI_AP);
  WiFi.setHostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
//  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
#ifdef USE_WIFIMULTI
#ifdef WIFI_SSID
  wifiMulti.addAP(wifi_ssid.c_str(), wifi_pass.c_str());
  write2log(LOG_SYSTEM, 4, "WIFI try to connect to ", wifi_ssid.c_str(), " with Password ", wifi_pass.c_str());
#endif
#ifdef WIFI_SSID1
  wifiMulti.addAP(wifi_ssid1.c_str(), wifi_pass1.c_str());
  write2log(LOG_SYSTEM, 4, "WIFI try to connect to ", wifi_ssid1.c_str(), " with Password ", wifi_pass1.c_str());
#endif
#ifdef WIFI_SSID2
  wifiMulti.addAP(wifi_ssid2.c_str(), wifi_pass2.c_str());
  write2log(LOG_SYSTEM, 4, "WIFI try to connect to ", wifi_ssid2.c_str(), " with Password ", wifi_pass2.c_str());
#endif
#else  // USE_WIFIMULTI
  int numberOfNetworks = WiFi.scanNetworks();
  int32_t rssi_max = -999;
  int bestNetworkIndex = -1;
  for (int i = 0; i < numberOfNetworks; i++) {
    if ( WiFi.SSID(i) == wifi_ssid ) {
      if (WiFi.RSSI(i) > rssi_max) {
        rssi_max = WiFi.RSSI(i);
        bestNetworkIndex = i;
      }
    }
  }
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str(), bestNetworkIndex >=0 ? WiFi.channel(bestNetworkIndex) : 0,
             bestNetworkIndex >=0 ? WiFi.BSSID(bestNetworkIndex) : NULL);
#endif  
#else // if ESP32
    //    WiFi.persistent(false);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(wifi_ssid, wifi_pass);
#endif  

  // ... Give ESP 10 seconds to connect to station.
  unsigned int i = 0;
#ifdef USE_WIFIMULTI
  while ( (wifiMulti.run() != WL_CONNECTED) && (i < 20) ) {
#else
  while ( (WiFi.status() != WL_CONNECTED) && (i < 100) ) {
#endif
    delay(500);
    i++;
#if defined(DEBUG_SERIAL)
    Serial.print(".");
#endif
  }
#if defined(DEBUG_SERIAL)
  Serial.println();
#endif
  if ( WiFi.status() == WL_CONNECTED ) {
    retval = true;
  } else {
    write2log(LOG_SYSTEM, 1, " ERROR WiFi not connected!");
#ifdef USE_WIFIMULTI
    write2log(LOG_SYSTEM,4, "Tested SSID: ",wifi_ssid.c_str(),wifi_ssid1.c_str(),wifi_ssid2.c_str());
#else
    write2log(LOG_SYSTEM,2, "Tested SSID: ",wifi_ssid.c_str());
#endif
    retval = false;
  } 
  return retval;
}

#ifdef ESP32
void getResetReason(String& tmp) {
  switch (rtc_get_reset_reason(0)) {
  case 1:
    tmp +=  String("POWERON_RESET");
    break; //1,  Vbat power on reset
  case 3:
    tmp += String("SW_RESET");
    break; //3,  Software reset digital core
  case 4:
    tmp += String("OWDT_RESET");
    break; //4,  Legacy watch dog reset digital core
  case 5:
    tmp += String("DEEPSLEEP_RESET");
    break; //5,  Deep Sleep reset digital core
  case 6:
    tmp += String("SDIO_RESET");
    break; //6,  Reset by SLC module, reset digital core
  case 7:
    tmp += String("TG0WDT_SYS_RESET");
    break; //7,  Timer Group0 Watch dog reset digital core
  case 8:
    tmp += String("TG1WDT_SYS_RESET");
    break; //8,  Timer Group1 Watch dog reset digital core
  case 9:
    tmp += String("RTCWDT_SYS_RESET");
    break; //9,  RTC Watch dog Reset digital core
  case 10:
    tmp += String("INTRUSION_RESET");
    break; //10, Instrusion tested to reset CPU
  case 11:
    tmp += String("TGWDT_CPU_RESET");
    break; //11, Time Group reset CPU
  case 12:
    tmp += String("SW_CPU_RESET");
    break; //12, Software reset CPU
  case 13:
    tmp += String("RTCWDT_CPU_RESET");
    break; //13, RTC Watch dog Reset CPU
  case 14:
    tmp += String("EXT_CPU_RESET");
    break; //14, for APP CPU, reseted by PRO CPU
  case 15:
    tmp += String("RTCWDT_BROWN_OUT_RESET");
    break; //15, Reset when the vdd voltage is not stable
  case 16:
    tmp += String("RTCWDT_RTC_RESET");
    break; //16, RTC Watch dog reset digital core and rtc module
  default:
    tmp += String("NO_MEAN");
  }
}
#endif

void start_AP() {
  write2log(LOG_SYSTEM,1, "Start Accesspoint: ESPNode");
  ap_mode = true;
  if (WiFi.mode(WIFI_AP)) {
    write2log(LOG_SYSTEM,1, "WIFI_AP => OK");
  } else {
    write2log(LOG_SYSTEM,1, "WIFI_AP => ERROR");
  }
  delay(500);  
  if (WiFi.softAP("ESPNode")) {
    IPAddress IP = WiFi.softAPIP();
    write2log(LOG_SYSTEM,2, "Http Server started on", IP.toString().c_str());
  } else {
    write2log(LOG_SYSTEM,1, "Error starting AP");
  }
  wifi_ap_starttime = millis();
#if defined(MQTT)    
  do_mqtt = false;
#endif
#if defined(RF24GW)
  do_rf24gw = false;
#endif
  setup_webserver();
}

/****************************************************
 * Setup
 *****************************************************/
void setup() {
#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  delay(1000);
  Serial.println("Begin Setup");
#endif
#ifdef USE_DISPLAY_GC9A01A
//  pinMode(TFT_BL, OUTPUT);
//  digitalWrite(TFT_BL, HIGH);
  display = new Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);
  if (display) {
    display->begin();
    display->setRotation(TFT_ROTATION);
    display->fillScreen(TFT_COLOR_BLACK);
    display->setTextSize(2);
    display->setTextColor(TFT_COLOR_GREEN);
    display->setCursor(80, 10);
    display->print("ESPNode");
    display->setCursor(80, 30);
    display->print("Booting...");
    display->fillRect(BOOTWINDOW_X, BOOTWINDOW_Y, BOOTWINDOW_WIDTH, BOOTWINDOW_HEIGHT, BOOTWINDOW_COLOR);
  }
#else
#warning "NOT Compiling Display GC9A01A with Settings for ESP32" 
#endif
#ifdef USE_DISPLAY_ST7796
  display = new Adafruit_ST7796S(TFT_CS, TFT_DC, TFT_RST);
  display->begin();
  display->setRotation(TFT_ROTATION);
#endif
#ifdef USE_DISPLAY
  if (display) {
  } else {
    Serial.println("Display not found");
//    Serial.println("TFT_CS = " + String(TFT_CS) + " TFT_DC = " + String(TFT_DC) + 
//                   " TFT_RST = " + String(TFT_RST) + " TFT_BL = " + String(TFT_BL));
  }
#else
#warning "USE_DISPLAY not set" 
#endif
  // Achtung: Wenn die Prefs zu schnell nach Systemstart aufgerufen werden gibt es einen Feler bei den Preferences!
  //          Die Werte werden nicht ausgelesen, das Programm steht!!!!!!!
  // !!!!!!!! Diesen DELAY nicht entfernen !!!!!!!!!
  delay(1000);
  // Serial port for debugging purposes
#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  Serial.println("Available Networks: ");
  int numberOfNetworks = WiFi.scanNetworks();
  for (int i = 0; i < numberOfNetworks; i++) {
#ifdef ESP32
    Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
#else
    Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
#endif
  }
#endif // DEBUG_SERIAL

#ifdef USE_DISPLAY
  bootMessage(0,"Loading Prefs",true,false);
#endif

  // Zunächst werden die Preferences im Schreibmodus geöffnet.
  // Sollte die "magicno" nicht mit der gespeicherten übereinstimmen
  // werden die Einstellungen aus der Umgebung in die Preferences geschrieben
  if (preferences.begin("settings",true)) {
    magicno = preferences.getUShort("magicno", 0);
#ifdef USE_DISPLAY
    bootMessage(1,"OK",false,true);
    bootMessage(1,"MagicNo:",true,false);
    bootMessage(1,String(magicno).c_str(),false,true);
#endif
    preferences.end();
  } else {
#ifdef USE_DISPLAY
    bootMessage(2,"Error",false, true);
    bootMessage(2,"Reboot !!!",true, true);
#endif
    ESP.restart();
  }
#ifdef DEBUG_SERIAL
#ifdef ESP32
  Serial.println(String("IdeVer: ") + String(ESP.getSdkVersion()));
  Serial.println(String("Arduino: ")+ String(ARDUINO));
  Serial.println(String("ESP-Arduino: ")  + String(ESP_ARDUINO_VERSION_MAJOR)+ String(".")
              + String(ESP_ARDUINO_VERSION_MINOR) + String(".") 
              + String(ESP_ARDUINO_VERSION_PATCH));
  Serial.println(String("ESP: ")  + String(ESP_IDF_VERSION_MAJOR)+ String(".")
              + String(ESP_IDF_VERSION_MINOR) + String(".") 
              + String(ESP_IDF_VERSION_PATCH));
#endif
  Serial.print("MagicNo(Prefs) = ");
  Serial.println(magicno);
  Serial.print("MagicNo(Prg) = ");
  Serial.println(MAGICNO);
#ifdef MQTT
  Serial.print("MQTT_SERVER = ");
  Serial.println(MQTT_SERVER);
#endif
#endif // DEBUG_SERIAL
// MagicNo ist unterschiedlich oder 0: Defaultwerte werden neu gesetzt!
  if ( (magicno != MAGICNO) || (MAGICNO == 0) ) {
#ifdef USE_DISPLAY
    bootMessage(1,"Using default Environment",true, true);
#endif
    wifi_ssid = WIFI_SSID;
    wifi_pass = WIFI_PASS;
#ifdef ESP32
#ifdef WIFI_SSID1
    wifi_ssid1 = WIFI_SSID1;
    wifi_pass1 = WIFI_PASS1;
#endif
#ifdef WIFI_SSID2
    wifi_ssid2 = WIFI_SSID2;
    wifi_pass2 = WIFI_PASS2;
#endif
#endif
    loop_time_alarm = LOOP_TIME_ALARM;
#if defined(MQTT)
#endif
// Save the defaults for the next start!
    preferences.clear();
    preferences.begin("settings",false);
    preferences.putUShort("magicno", MAGICNO);
    preferences.putString("wifi_ssid", wifi_ssid); 
    preferences.putString("wifi_pass", wifi_pass);
#ifdef ESP32
#ifdef USE_WIFIMULTI
    preferences.putString("wifi_ssid1", wifi_ssid1); 
    preferences.putString("wifi_pass1", wifi_pass1);
    preferences.putString("wifi_ssid2", wifi_ssid2); 
    preferences.putString("wifi_pass2", wifi_pass2);
#endif
#endif
    preferences.putUInt("loop_time_alarm", loop_time_alarm);
#if defined(MQTT)
    do_mqtt      = MQTT;
    mqtt_server  = MQTT_SERVER;
    mqtt_client  = MQTT_CLIENT;
    mqtt_topic_part2 = MQTT_TOPICP2;
    do_log_mqtt  = DO_LOG_MQTT;
    preferences.putBool("do_mqtt", do_mqtt);
    preferences.putString("mqtt_server", mqtt_server);
    preferences.putString("mqtt_client", mqtt_client);
    preferences.putString("mqtt_topic_part2", mqtt_topic_part2);
    preferences.putBool("do_log_mqtt", do_log_mqtt);
#endif
#if defined(RF24GW)
    do_rf24gw         = RF24GW;
    rf24gw_hub_server = RF24GW_HUB_SERVER;
    rf24gw_hub_port   = RF24GW_HUB_UDP_PORTNO;
    rf24gw_gw_port    = RF24GW_GW_UDP_PORTNO;
    rf24gw_gw_no      = RF24GW_NO;
    do_log_rf24       = DO_LOG_RF24;
    preferences.putBool("do_rf24gw", do_rf24gw);
    preferences.putString("rf24gw_hub_server", rf24gw_hub_server);
    preferences.putUInt("rf24gw_hub_port", rf24gw_hub_port);
    preferences.putUInt("rf24gw_gw_port", rf24gw_gw_port);
    preferences.putUInt("rf24gw_gw_no", rf24gw_gw_no);
    preferences.putBool("do_log_rf24", do_log_rf24);
#endif
    do_log_module   = DO_LOG_MODULE;
    do_log_web      = DO_LOG_WEB;
    do_log_system   = DO_LOG_SYSTEM;
    do_log_critical = DO_LOG_CRITICAL;
    preferences.putBool("do_log_module", do_log_module);
    preferences.putBool("do_log_web", do_log_web);
    preferences.putBool("do_log_system", do_log_system);
    preferences.putBool("do_log_critical", do_log_critical);
    preferences.end();
  } else {
#ifdef USE_DISPLAY
    bootMessage(1,"Using Env. from Prefs",true, true);
#endif
    preferences.begin("settings",true);
// Wenn sich die MagicNo nicht geändert hat werden die gespeicherten Werte genommen
    wifi_ssid         = preferences.getString("wifi_ssid"); 
    wifi_pass         = preferences.getString("wifi_pass");
#ifdef ESP32
#ifdef USE_WIFIMULTI
    if (preferences.isKey("wifi_ssid1")) wifi_ssid1 = preferences.getString("wifi_ssid1"); 
    if (preferences.isKey("wifi_pass1")) wifi_pass1 = preferences.getString("wifi_pass1");
    if (preferences.isKey("wifi_ssid2")) wifi_ssid2 = preferences.getString("wifi_ssid2"); 
    if (preferences.isKey("wifi_pass1")) wifi_pass2 = preferences.getString("wifi_pass2");
#endif
#endif
    loop_time_alarm   = preferences.getUInt("loop_time_alarm");
#if defined(MQTT)
    do_mqtt           = preferences.getBool("do_mqtt");
    mqtt_server       = preferences.getString("mqtt_server");
    mqtt_client       = preferences.getString("mqtt_client");
    mqtt_topic_part2      = preferences.getString("mqtt_topic_part2");
    do_log_mqtt       = preferences.getBool("do_log_mqtt");
#endif
#if defined(RF24GW)
    do_rf24gw         = preferences.getBool("do_rf24gw");
    rf24gw_hub_server = preferences.getString("rf24gw_hub_server");
    rf24gw_hub_port   = preferences.getUInt("rf24gw_hub_port");
    rf24gw_gw_port    = preferences.getUInt("rf24gw_gw_port");
    rf24gw_gw_no      = preferences.getUInt("rf24gw_gw_no");
    do_log_rf24       = preferences.getBool("do_log_rf24");
#endif
    do_log_module     = preferences.getBool("do_log_module");
    do_log_web        = preferences.getBool("do_log_web");
    do_log_system     = preferences.getBool("do_log_system");
    do_log_critical   = preferences.getBool("do_log_critical");
    preferences.end();
  }
#if defined(DEBUG_SERIAL)
#if defined(MQTT)
  Serial.print("MQTT: ");
  Serial.println(do_mqtt?"Ein":"Aus");
#endif
#if defined(RF24GW)
  Serial.print("RF24GW: ");
  Serial.println(do_rf24gw?"Ein":"Aus");
#endif
  Serial.println("Status Logging:");
#if defined(MQTT)
  Serial.print("Mqtt: ");
  Serial.println(do_log_mqtt?"ja":"nein");
#endif
#if defined(RF24GW)
  Serial.print("RF24GW: ");
  Serial.println(do_log_rf24?"ja":"nein");
#endif
  Serial.print("Modul: ");
  Serial.println(do_log_module?"ja":"nein");
  Serial.print("Web: ");
  Serial.println(do_log_web?"ja":"nein");
  Serial.print("Sys: ");
  Serial.println(do_log_system?"ja":"nein");
  Serial.print("Critical: ");
  Serial.println(do_log_critical?"ja":"nein");
#endif // DEBUG_SERIAL
#ifdef USE_DISPLAY
  bootMessage(0,"mount FS",true, false);
#endif

  if (!LittleFS.begin()) {
#ifdef USE_DISPLAY
    bootMessage(2,"Error",false, true);
    bootMessage(2,"REBOOT",true, true);
#endif
    ESP.restart();
    return;
  } else {
    write2log(LOG_SYSTEM,1, "++ Begin Startup: LittleFS mounted ++");
#ifdef USE_DISPLAY
    bootMessage(1,"OK",false, true);
#endif
  }

#ifdef USE_DISPLAY
  bootMessage(0,"Con WiFi",true, false);
#endif
  // Connect to Wi-Fi
  if ( ! do_wifi_con() ) {
#ifdef USE_DISPLAY
    bootMessage(2,"Error",false, true);
    bootMessage(2,"Start AP",true, true);
#endif
    start_AP();
  } else {
    
#ifdef USE_DISPLAY
    bootMessage(1,WiFi.localIP().toString().c_str(),false, true);
#endif
    write2log(LOG_SYSTEM,2, "Node Address is ", WiFi.localIP().toString().c_str());
#ifdef USE_DISPLAY
  bootMessage(0,"get Time",true, false);
#endif
    setupTime();
    if ( ! getNTPtime(30) ) {
      write2log(LOG_SYSTEM,1, "Error getting NTP Time");
#ifdef USE_DISPLAY
      bootMessage(2,"Error",false, true);
#endif
    } else {
#ifdef USE_DISPLAY
      char timestr[20];
      sprintf(timestr,"%d.%d.%d %02d:%02d",timeinfo.tm_mday, 1 + timeinfo.tm_mon, 1900 + timeinfo.tm_year,  timeinfo.tm_hour, timeinfo.tm_min);
      bootMessage(1,timestr,false, true);
#endif
    }
    lastDay = timeinfo.tm_mday;
    write2log(LOG_DAYBREAK, 0);
#ifdef ESP32
    //ToDo
#else
    write2log(LOG_CRITICAL, 2, "Reboot: ", ESP.getResetReason().c_str());
#endif
  }
  setup_webserver();
#if defined(RF24GW)
  rf24gw_setup(); 
#endif
#if defined(MQTT)
  mqtt_setup();
#endif
#ifdef ESP32
#if defined(DEBUG_SERIAL)
  Serial.print("ESP32: (Cores Model Revision) ");
  Serial.print(ESP.getChipCores());
  Serial.print(" ");
  Serial.print(ESP.getChipModel());
  Serial.print(" ");
  Serial.println(ESP.getChipRevision());
  Serial.print("Gesamt PSRAM: ");
  Serial.println(ESP.getPsramSize());
  Serial.print("Free PSRAM: ");
  Serial.println(ESP.getFreePsram());
  Serial.print("Cycle Count: ");
  Serial.println(ESP.getCycleCount());
#endif
#endif
#ifdef USE_DISPLAY
  bootMessage(0,"Loading Modules",true, false);
#endif
#if defined(MODULE1)
  MODULE1_BEGIN_STATEMENT
#endif
#if defined(MODULE2)
  MODULE2_BEGIN_STATEMENT
#endif
#if defined(MODULE3)
  MODULE3_BEGIN_STATEMENT
#endif
#if defined(MODULE4)
  MODULE4_BEGIN_STATEMENT
#endif
#if defined(MODULE5)
  MODULE5_BEGIN_STATEMENT
#endif
#if defined(MODULE6)
  MODULE6_BEGIN_STATEMENT
#endif
#ifdef USE_DISPLAY
    bootMessage(1,"OK",false, true);
#endif
#ifdef USE_DISPLAY
  bootMessage(0,"Ende Setup",true, false);
  delay(3000);
#endif
  write2log(LOG_SYSTEM,1, "Setup Ende");
}

/************************************************
 * Main Loop
*************************************************/
void loop() {
  ElegantOTA.loop();
  if ( rebootflag ) {
    preferences.end();
    write2log(LOG_CRITICAL,1,"Reboot Flag gesetzt => reboot");
    yield();
    delay(2000);
    yield();
    ESP.restart();
  }
  if ( ap_mode) {
    // IF we have an SSID stored we test every 5 Minutes to connect
    if (millis() - wifi_ap_starttime > 300000 && wifi_ssid.length() > 2) {
      if ( do_wifi_con() ) {
        // If connection is posible we restart 
        delay(1000);
        ESP.restart();
      } else {
        start_AP();
      }
    }
  } else {
    loop_starttime = millis();
    if (WiFi.status() != WL_CONNECTED) {
      write2log(LOG_CRITICAL,1,"WiFi connection lost");
      delay(1000);
      int i=0;
      bool hourFlag=false;
      while ( ! do_wifi_con() ) {
        delay(10000);
        i++;
        if ( i>360 && !hourFlag) {
          write2log(LOG_CRITICAL,1,"Wifi already 1 hour offline");
          hourFlag = true;
        }
      }
      write2log(LOG_CRITICAL,1,"WiFi reconnected");
    }
    time(&now);                   // read the current time
    localtime_r(&now, &timeinfo); // update the structure tm with the current time
    yield();
    ws.cleanupClients();
    webserver_loop(now);
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(mymsg,29,"Looptime WiFi: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,mymsg);
    }
    yield();
#if defined(RF24GW)
    rf24gw_loop();
    yield();
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(mymsg,29,"Looptime RF24GW: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,mymsg);
    }
#endif
#if defined(MQTT)
    mqtt_loop(now);
//    delay(0);
    yield();
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(mymsg,29,"Looptime MQTT: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,mymsg);
    }
#endif
#if defined(MODULE1)
    module1.loop(now);
    yield();
#endif
#if defined(MODULE2)
    module2.loop(now);
    yield();
#endif
#if defined(MODULE3)
    module3.loop(now);
    yield();
#endif
#if defined(MODULE4)
    module4.loop(now);
    yield();
#endif
#if defined(MODULE5)
    module5.loop(now);
    yield();
#endif
#if defined(MODULE6)
    module6.loop(now);
    yield();
#endif
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(mymsg,29,"Looptime Modules: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,mymsg);
    }
    yield();
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(mymsg,29,"Looptime Stat: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,mymsg);
    }
// Dinge die täglich erledigt werden sollen
    yield();
    if ( lastDay != timeinfo.tm_mday ) {
      setupTime();
      write2log(LOG_DAYBREAK,0);
      lastDay = timeinfo.tm_mday;
    }
// Dinge die stündlich erledigt werden sollen
    if ( lastHour != timeinfo.tm_hour ) {
      getNTPtime(10);
      uint32_t free;
      uint32_t max;
      uint8_t frag;
      rssi = WiFi.RSSI();
      rssi_quality = 0;
      if ( rssi > -50) {
        rssi_quality = 100;
      } else {
        if ( rssi > -100 ) {
          rssi_quality = 2 * (rssi + 100);
        } else {
          rssi_quality = 0;
        }
      }
#ifdef ESP32
      free = ESP.getFreeHeap();
      frag = 0;
      max = 0;
#else
      ESP.getHeapStats(&free, &max, &frag);
#endif
      String tmp_str;
      tmp_str = String("Wifi: ") + WiFi.SSID() + String("/") + String(WiFi.channel()) + String("/") + 
                String(WiFi.RSSI()) + String("; Mem: ") + String(free) + String("(") + String(max) + String("/") +
                String(frag) + String(")");
      write2log(LOG_CRITICAL,1,tmp_str.c_str());
      uptime.update();
      lastHour = timeinfo.tm_hour;
    }
// Dinge die minütlich erledigt werden sollen
    if (timeinfo.tm_min != lastMinute) {
      minutes++;
      lastMinute = timeinfo.tm_min;
    }
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(mymsg,29,"Looptime LoopEnd: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,mymsg);
    }
  }
}
