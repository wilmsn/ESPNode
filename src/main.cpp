#include "main.h"


#ifdef ESP32
WiFiMulti wifiMulti;
#else

#endif

#ifdef USE_FTP
FTPServer        ftp;
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

// Kommentiert in main.h
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
  if (count > 12) count = 12; 
  char *c[12];

  /* Parameterabfrage initialisieren */
  va_start(args, count);
  while (n < count) {
    c[n] = (char *)va_arg(args, char *);
    n++;
  }
  // Im AP-Mode wird nichts gelogged !!!
  if ( ! ap_mode ) {
    snprintf(timeStr, 15, "[%02d:%02d:%02d.%03u]", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, (unsigned int)millis()%1000);
    switch(kat) {
      case LOG_WEB: snprintf(katStr,6,"[ WEB]"); break;
      case LOG_SYSTEM: snprintf(katStr,6,"[ SYS]"); break;
      case LOG_MQTT: snprintf(katStr,6,"[MQTT]"); break;
      case LOG_RF24: snprintf(katStr,6,"[RF24]"); break;
      case LOG_MODULE: snprintf(katStr,6,"[ MOD]"); break;
      case LOG_CRITICAL: snprintf(katStr,6,"[CRIT]"); break;
      default: snprintf(katStr,6,"[----]"); break;
    }
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
        f.printf("%s ",timeStr);
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
      log_str = "{\"log\":\"";
      log_str += timeStr;
      log_str += katStr;
      n = 0;
      while (n < count) {
        log_str += " ";
        // Anführungszeichen innerhalb des Log-Strings führen zu Frontendproblemen.
        // Deshalb wird " gegen ' ersetzt!
        for ( unsigned int i=0; i<strlen(c[n]); i++ ) {
          if ( c[n][i] == '"' ) {
            log_str += "'";  
          } else {
            log_str += c[n][i];
          }
        }
        n++;
      }
      log_str += "\"}";
      ws.textAll(log_str);
    }
#if defined(DEBUG_SERIAL)
    Serial.print(timeStr);
    n = 0;
    while (n < count) {
      Serial.print(c[n]);
      Serial.print(" ");
      n++;
    }
    Serial.println();
#endif
    va_end(args);
  }
}

// Kommentiert in main.h
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
//  write2log(LOG_SYSTEM, 4, "Try to connect to ", wifi_ssid1.c_str(), " with password ", wifi_pass1.c_str());
  WiFi.mode(WIFI_STA);
#ifdef ESP32
// Wichtig fuer ESP32S3, sonst wird der Hostname nicht gesetzt!
  WiFi.mode(WIFI_AP);
  WiFi.setHostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
#ifdef USE_WIFIMULTI  
  wifiMulti.addAP(wifi_ssid.c_str(), wifi_pass.c_str());
  wifiMulti.addAP(wifi_ssid1.c_str(), wifi_pass1.c_str());
  wifiMulti.addAP(wifi_ssid2.c_str(), wifi_pass2.c_str());
  write2log(LOG_SYSTEM, 4, "WIFI try to connect to ", wifi_ssid.c_str(), " with Password ", wifi_pass.c_str());
  write2log(LOG_SYSTEM, 4, "WIFI try to connect to ", wifi_ssid1.c_str(), " with Password ", wifi_pass1.c_str());
  write2log(LOG_SYSTEM, 4, "WIFI try to connect to ", wifi_ssid2.c_str(), " with Password ", wifi_pass2.c_str());
#else  
//  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
  WiFi.begin(WIFI_SSID, WIFI_PASS);
#endif  
#else
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
//    write2log(LOG_SYSTEM, 2, " OK connected!", wifi_ssid.c_str());
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
// dokumentiert in main.h
char *getResetReason(char *tmp)
{
#if defined(DEBUG_SERIAL_WEB)
  Serial.println("Reset Reason roh:");
  Serial.println(rtc_get_reset_reason(0));
#endif
  switch (rtc_get_reset_reason(0))
  {
  case 1:
    sprintf(tmp, "%s", "POWERON_RESET");
    break; //1,  Vbat power on reset
  case 3:
    sprintf(tmp, "%s", "SW_RESET");
    break; //3,  Software reset digital core
  case 4:
    sprintf(tmp, "%s", "OWDT_RESET");
    break; //4,  Legacy watch dog reset digital core
  case 5:
    sprintf(tmp, "%s", "DEEPSLEEP_RESET");
    break; //5,  Deep Sleep reset digital core
  case 6:
    sprintf(tmp, "%s", "SDIO_RESET");
    break; //6,  Reset by SLC module, reset digital core
  case 7:
    sprintf(tmp, "%s", "TG0WDT_SYS_RESET");
    break; //7,  Timer Group0 Watch dog reset digital core
  case 8:
    sprintf(tmp, "%s", "TG1WDT_SYS_RESET");
    break; //8,  Timer Group1 Watch dog reset digital core
  case 9:
    sprintf(tmp, "%s", "RTCWDT_SYS_RESET");
    break; //9,  RTC Watch dog Reset digital core
  case 10:
    sprintf(tmp, "%s", "INTRUSION_RESET");
    break; //10, Instrusion tested to reset CPU
  case 11:
    sprintf(tmp, "%s", "TGWDT_CPU_RESET");
    break; //11, Time Group reset CPU
  case 12:
    sprintf(tmp, "%s", "SW_CPU_RESET");
    break; //12, Software reset CPU
  case 13:
    sprintf(tmp, "%s", "RTCWDT_CPU_RESET");
    break; //13, RTC Watch dog Reset CPU
  case 14:
    sprintf(tmp, "%s", "EXT_CPU_RESET");
    break; //14, for APP CPU, reseted by PRO CPU
  case 15:
    sprintf(tmp, "%s", "RTCWDT_BROWN_OUT_RESET");
    break; //15, Reset when the vdd voltage is not stable
  case 16:
    sprintf(tmp, "%s", "RTCWDT_RTC_RESET");
    break; //16, RTC Watch dog reset digital core and rtc module
  default:
    sprintf(tmp, "%s", "NO_MEAN");
  }
#if defined(DEBUG_SERIAL_WEB)
  Serial.println("Reset Reason:");
  Serial.println(tmp);
#endif
  return tmp;
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

#if defined(DEBUG_SERIAL)
  // Serial port for debugging purposes
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
#endif
  // Zunächst werden die Preferences im Schreibmodus geöffnet.
  // Sollte die "magicno" nicht mit der gespeicherten übereinstimmen
  // werden die Einstellungen aus der Umgebung in die Preferences geschrieben
  preferences.begin("settings",true);
  magicno = preferences.getInt("magicno");
  preferences.end();
#if defined(DEBUG_SERIAL)
  Serial.print("MagicNo(Prefs) = ");
  Serial.println(magicno);
  Serial.print("MagicNo(Prg) = ");
  Serial.println(MAGICNO);
#if defined(MQTT)
  Serial.print("MQTT_SERVER = ");
  Serial.println(MQTT_SERVER);
#endif
#endif
// MagicNo ist unterschiedlich oder 0: Defaultwerte werden neu gesetzt!
  if ( (magicno != MAGICNO) || (MAGICNO == 0) ) {
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
    preferences.begin("settings",false);
    preferences.putInt("magicno", MAGICNO);
    preferences.putString("wifi_ssid", wifi_ssid); 
    preferences.putString("wifi_pass", wifi_pass);
#ifdef ESP32
#ifdef WIFI_SSID1
    preferences.putString("wifi_ssid1", wifi_ssid1); 
    preferences.putString("wifi_pass1", wifi_pass1);
#endif
#ifdef WIFI_SSID2
    preferences.putString("wifi_ssid2", wifi_ssid2); 
    preferences.putString("wifi_pass2", wifi_pass2);
#endif
#endif
    preferences.putUInt("loop_time_alarm", loop_time_alarm);
#if defined(MQTT)
    do_mqtt      = MQTT;
    mqtt_server  = MQTT_SERVER;
    mqtt_client  = MQTT_CLIENT;
    mqtt_topicP2 = MQTT_TOPICP2;
    do_log_mqtt  = DO_LOG_MQTT;
    preferences.putBool("do_mqtt", do_mqtt);
    preferences.putString("mqtt_server", mqtt_server);
    preferences.putString("mqtt_client", mqtt_client);
    preferences.putString("mqtt_topicP2", mqtt_topicP2);
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
    preferences.putInt("rf24gw_hub_port", rf24gw_hub_port);
    preferences.putInt("rf24gw_gw_port", rf24gw_gw_port);
    preferences.putInt("rf24gw_gw_no", rf24gw_gw_no);
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
    preferences.begin("settings",true);
// Wenn sich die MagicNo nicht geändert hat werden die gespeicherten Werte genommen
    wifi_ssid         = preferences.getString("wifi_ssid"); 
    wifi_pass         = preferences.getString("wifi_pass");
#ifdef ESP32
    if (preferences.isKey("wifi_ssid1")) wifi_ssid1 = preferences.getString("wifi_ssid1"); 
    if (preferences.isKey("wifi_pass1")) wifi_pass1 = preferences.getString("wifi_pass1");
    if (preferences.isKey("wifi_ssid2")) wifi_ssid2 = preferences.getString("wifi_ssid2"); 
    if (preferences.isKey("wifi_pass1")) wifi_pass2 = preferences.getString("wifi_pass2");
#endif
    loop_time_alarm   = preferences.getUInt("loop_time_alarm");
#if defined(MQTT)
    do_mqtt           = preferences.getBool("do_mqtt");
    mqtt_server       = preferences.getString("mqtt_server");
    mqtt_client       = preferences.getString("mqtt_client");
    mqtt_topicP2      = preferences.getString("mqtt_topicP2");
    do_log_mqtt       = preferences.getBool("do_log_mqtt");
#endif
#if defined(RF24GW)
    do_rf24gw         = preferences.getBool("do_rf24gw");
    rf24gw_hub_server = preferences.getString("rf24gw_hub_server");
    rf24gw_hub_port   = preferences.getInt("rf24gw_hub_port");
    rf24gw_gw_port    = preferences.getInt("rf24gw_gw_port");
    rf24gw_gw_no      = preferences.getInt("rf24gw_gw_no");
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
#endif

  if (!LittleFS.begin()) {
    ESP.restart();
    return;
  } else {
    write2log(LOG_SYSTEM,1, "++ Begin Startup: LittleFS mounted ++");
  }

  // Connect to Wi-Fi
  if ( ! do_wifi_con() ) {
    start_AP();
  } else {
#if defined(DEBUG_SERIAL)
    IPAddress IP = WiFi.localIP();
    write2log(LOG_SYSTEM,2, "Node Address is ", IP.toString().c_str());
#endif
    setupTime();
    if ( !getNTPtime(10) ) {
      write2log(LOG_SYSTEM,1, "Error getting NTP Time");
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
#ifdef USE_FTP
  ftp.addUser("ftp", "ftp");
  ftp.addFilesystem("LittleFS", &LittleFS);
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
#ifdef USE_FTP
  ftp.begin();
#endif
#ifdef USE_SDCARD
  SD.begin(SD_CS);
  sd_cardsize = SD.cardSize();
  sd_cardType = SD.cardType();
  sd_usedbytes = SD.usedBytes();
#endif
  write2log(LOG_SYSTEM,1, "Setup Ende");
}

/************************************************
 * Main Loop
*************************************************/
void loop() {
  ElegantOTA.loop();
#ifdef USE_FTP
  ftp.handle();
#endif
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
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(loopmsg,29,"Looptime WiFi: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,loopmsg);
    }
    yield();
#if defined(RF24GW)
    rf24gw_loop();
    yield();
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(loopmsg,29,"Looptime RF24GW: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,loopmsg);
    }
#endif
#if defined(MQTT)
    mqtt_loop(now);
//    delay(0);
    yield();
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(loopmsg,29,"Looptime MQTT: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,loopmsg);
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
      snprintf(loopmsg,29,"Looptime Modules: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,loopmsg);
    }
    yield();
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(loopmsg,29,"Looptime Stat: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,loopmsg);
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
      tmp_str = "Wifi: " + WiFi.SSID() + "/" + String(WiFi.channel()) +"/" + String(WiFi.RSSI()) + "; Mem: " + String(free) + "(" + String(max) + "/" +
                String(frag) + ")";
      write2log(LOG_CRITICAL,1,tmp_str.c_str());
      uptime.update();
      lastHour = timeinfo.tm_hour;
    }
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(loopmsg,29,"Looptime LoopEnd: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,loopmsg);
    }
  }
}
