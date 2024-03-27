#include "main.h"

#if defined(SWITCH1)
SWITCH1_DEFINITION
#endif
#if defined(SWITCH2)
SWITCH2_DEFINITION
#endif
#if defined(SWITCH3)
SWITCH3_DEFINITION
#endif
#if defined(SWITCH4)
SWITCH4_DEFINITION
#endif
#if defined(SENSOR1)
SENSOR1_DEFINITION
#endif
#if defined(SENSOR2)
SENSOR2_DEFINITION
#endif

void setupTime() {                              
  // deinen NTP Server einstellen (von 0 - 5 aus obiger Liste) alternativ lassen sich durch Komma getrennt bis zu 3 Server angeben
#ifdef ESP32
  configTzTime("CET-1CEST,M3.5.0/03,M10.5.0/03", ntp_server);
#else
  configTime("CET-1CEST,M3.5.0,M10.5.0/3", ntp_server);  
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



// Kommentiert in main.h
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
  // Im AP-Mode wird nichts in Filesystem gelogged !!!
  if ( ! ap_mode ) {
    snprintf(timeStr, 15, "[%02d:%02d:%02d.%03u]", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, (unsigned int)millis()%1000);
    if ( do_log_critical && kat == LOG_DAYBREAK ) {
      File f = LittleFS.open( DEBUGFILE, "a" );
      if (f) {
        f.printf("----%d.%d.%d----\n",timeinfo.tm_mday, 1 + timeinfo.tm_mon, 1900 + timeinfo.tm_year);
        f.close();
      }
    }
    if (do_log_critical && kat == LOG_CRITICAL) {
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
  }
  if (do_log_sensor && (kat == LOG_SENSOR    ) ||
      (do_log_web && (kat == LOG_WEB)     ) ||
#if defined(MQTT)
      (do_log_mqtt && (kat == LOG_MQTT)   ) ||
#endif
#if defined(RF24GW)
      (do_log_rf24 && (kat == LOG_RF24)   ) ||
#endif
      (do_log_sys && (kat == LOG_SYS) ) || 
      (do_log_critical && (kat == LOG_CRITICAL) )) {
    log_str = "{\"log\":\"";
    log_str += timeStr;
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
  write2log(LOG_SYS, 4, "Try to connect to ", wifi_ssid.c_str(), " with password ", wifi_pass.c_str());
  WiFi.mode(WIFI_STA);
#ifdef ESP32
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
#else
    //    WiFi.persistent(false);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(wifi_ssid, wifi_pass);
#endif
  write2log(LOG_SYS, 2, "WIFI try to connect to ", wifi_ssid.c_str());

  // ... Give ESP 10 seconds to connect to station.
  unsigned int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < 20) {
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
    write2log(LOG_SYS, 2, " OK connected!", wifi_ssid.c_str());
  } else {
    write2log(LOG_SYS, 2, " ERROR not connected!", wifi_ssid.c_str());
    retval = false;
  }
  
  return retval;
}




// Kommentiert in main.h
const char *mk_sysinfo1(String& info_str) {
  uint32_t free;
  uint32_t max;
  uint8_t frag;
#ifdef ESP32
  free = ESP.getFreeHeap();
  max = ESP.getMaxAllocHeap();
#else
  ESP.getHeapStats(&free, &max, &frag);
#endif
#if defined(DEBUG_SERIAL_HTML)
  Serial.println("Generiere sysinfo1 ... ");
#endif
  info_str = "{";
#ifdef ESP32
  info_str +=  "\"Platform\":\"";
  info_str +=  ESP.getChipModel();
  info_str += "\"";
#else
  info_str += "\"Platform\":\"ESP8266\"";
#endif
#ifdef ESP32
  info_str += ",\"Cores\":";
  info_str += String(ESP.getChipCores());
  info_str += ",\"PSRamSize\":\"";
  info_str += String((float)ESP.getPsramSize()/1024.0);
  info_str += " KB\"";
  info_str += ",\"PsRamFree\":\"";
  info_str += String((float)ESP.getFreePsram()/1024.0);
  info_str += " KB\"";
#else
  info_str += ",\"Cores\":\"1\"";
#endif
  info_str += ",\"Hostname\":\"";
#ifdef ESP32
  info_str += HOSTNAME;
#else
  info_str += WiFi.hostname();
#endif
  info_str += "\"";
  info_str += ",\"CpuFreq\":\"";
  info_str += String((int)(F_CPU / 1000000));
  info_str += " Mhz\"";
  info_str += ",\"FlashSize\":\"";
  info_str += String((int)(ESP.getFlashChipSize() / 1024 / 1024));
  info_str += " MB \"";
  info_str += ",\"FlashFreq\":\"";
  info_str += String((int)(ESP.getFlashChipSpeed() / 1000000));
  info_str += " Mhz\"";
  info_str += ",\"Sketchsize\":\"";
  info_str += String(ESP.getSketchSize() / 1024.0);
  info_str += " kB\"";
  info_str += ",\"Freespace\":\"";
  info_str += String(ESP.getFreeSketchSpace() / 1024.0);
  info_str += " kB\"";
  info_str += ",\"Heap_free\":\"";
  info_str += String((float)free / 1024.0);
  info_str += " kB\"";
  info_str += ",\"Heap_max\":\"";
  info_str += String((float)max / 1024.0);
  info_str += " kB\"";
  info_str += ",\"Heap_frag\":\"";
#ifdef ESP32
  info_str += "n.a.\"";
#else
  info_str += String((float)frag / 1024.0);
  info_str += "%\"";
#endif
  info_str += ",\"ResetReason\":\"";
#ifdef ESP32
  char tmp1[20];
  info_str += getResetReason(tmp1);
#else
  info_str += ESP.getResetReason();
#endif
  info_str += "\"";
  info_str += ",\"Vcc\":\"";
  getVcc(info_str);
  info_str += "\"";
  info_str += ",\"UpTime\":\"";
  info_str += uptime.uptimestr();
  info_str += "\"";
  info_str += "}";
  write2log(LOG_WEB,1,info_str.c_str());
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Sysinfo1: ");
  Serial.print(info_str.length());
  Serial.println(" byte");
#endif
  return info_str.c_str();
}

#ifdef ESP32
// dokumentiert in main.h
char *getResetReason(char *tmp)
{
#if defined(DEBUG_SERIAL_HTML)
  Serial.println("Reset Reason roh:");
  Serial.println(rtc_get_reset_reason(0));
#endif
  switch (rtc_get_reset_reason(0))
  {
  case 1:
    sprintf(tmp, "%s", "POWERON_RESET");
    break; /**<1,  Vbat power on reset*/
  case 3:
    sprintf(tmp, "%s", "SW_RESET");
    break; /**<3,  Software reset digital core*/
  case 4:
    sprintf(tmp, "%s", "OWDT_RESET");
    break; /**<4,  Legacy watch dog reset digital core*/
  case 5:
    sprintf(tmp, "%s", "DEEPSLEEP_RESET");
    break; /**<5,  Deep Sleep reset digital core*/
  case 6:
    sprintf(tmp, "%s", "SDIO_RESET");
    break; /**<6,  Reset by SLC module, reset digital core*/
  case 7:
    sprintf(tmp, "%s", "TG0WDT_SYS_RESET");
    break; /**<7,  Timer Group0 Watch dog reset digital core*/
  case 8:
    sprintf(tmp, "%s", "TG1WDT_SYS_RESET");
    break; /**<8,  Timer Group1 Watch dog reset digital core*/
  case 9:
    sprintf(tmp, "%s", "RTCWDT_SYS_RESET");
    break; /**<9,  RTC Watch dog Reset digital core*/
  case 10:
    sprintf(tmp, "%s", "INTRUSION_RESET");
    break; /**<10, Instrusion tested to reset CPU*/
  case 11:
    sprintf(tmp, "%s", "TGWDT_CPU_RESET");
    break; /**<11, Time Group reset CPU*/
  case 12:
    sprintf(tmp, "%s", "SW_CPU_RESET");
    break; /**<12, Software reset CPU*/
  case 13:
    sprintf(tmp, "%s", "RTCWDT_CPU_RESET");
    break; /**<13, RTC Watch dog Reset CPU*/
  case 14:
    sprintf(tmp, "%s", "EXT_CPU_RESET");
    break; /**<14, for APP CPU, reseted by PRO CPU*/
  case 15:
    sprintf(tmp, "%s", "RTCWDT_BROWN_OUT_RESET");
    break; /**<15, Reset when the vdd voltage is not stable*/
  case 16:
    sprintf(tmp, "%s", "RTCWDT_RTC_RESET");
    break; /**<16, RTC Watch dog reset digital core and rtc module*/
  default:
    sprintf(tmp, "%s", "NO_MEAN");
  }
#if defined(DEBUG_SERIAL_HTML)
  Serial.println("Reset Reason:");
  Serial.println(tmp);
#endif
  return tmp;
}
#endif

// Kommentiert in main.h
const char *mk_sysinfo2(String& info_str) {
  int rssi = WiFi.RSSI();
  int rssi_quality = 0;
  if ( rssi > -50) {
    rssi_quality = 100;
  } else {
    if ( rssi > -100 ) {
      rssi_quality = 2 * (rssi + 100);
    } else {
      rssi_quality = 0;
    }
  }
#if defined(DEBUG_SERIAL_HTML)
  Serial.println("Generiere sysinfo2 ... ");
#endif
  info_str = "{";
  info_str += "\"IP\":\"";
  info_str += WiFi.localIP().toString();
  info_str += "\"";
  info_str += ",\"SubNetMask\":\"";
  info_str += WiFi.subnetMask().toString();
  info_str += "\"";
  info_str += ",\"GW-IP\":\"";
  info_str += WiFi.gatewayIP().toString();
  info_str += "\"";
  info_str += ",\"DnsIP\":\"";
  info_str += WiFi.dnsIP().toString();
  info_str += "\"";
  info_str += ",\"SSID\":\"";
  info_str += WiFi.SSID();
  info_str += " (";
  info_str += String(rssi);
  info_str += "dBm / ";
  info_str += String(rssi_quality);
  info_str += "%)\"";
  info_str += ",\"Channel\":\"";
  info_str += String(WiFi.channel());
  info_str += "\"";
  info_str += ",\"BSSID\":\"";
  info_str += WiFi.BSSIDstr();
  info_str += "\"";
  info_str += ",\"MAC\":\"";
  info_str += WiFi.macAddress();
  info_str += "\"";
  info_str += ",\"IdeVer\":\"";
  info_str += String(ARDUINO);
  info_str += "\"";
#ifdef ESP32
  info_str += ",\"CoreVer\":\"unknown\"";
#else
  info_str += ",\"CoreVer\":\"";
  info_str += ESP.getCoreVersion();
  info_str += "\"";
#endif
  info_str += ",\"SdkVer\":\"";
  info_str += ESP.getSdkVersion();
  info_str += "\"";
  info_str += ",\"SW\":\"";
  info_str += SWVERSION;
  info_str += " (";
  info_str += __DATE__;
  info_str += ")\"";
  info_str += "}";
  write2log(LOG_WEB,1,info_str.c_str());
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Sysinfo2: ");
  Serial.print(info_str.length());
  Serial.println(" byte");
#endif
  return info_str.c_str();
}

// Kommentiert in main.h
const char *mk_sysinfo3(String& info_str, bool format_mqtt) {
#if defined(DEBUG_SERIAL_HTML)
  Serial.println("Generiere sysinfo3 ... ");
#endif
  info_str = "{";
#if defined(MQTT)  
  info_str += "\"mqttserver\":\"";
  info_str += mqtt_server;
  info_str += "\",\"mqttclient\":\"";
  info_str += mqtt_client;
  info_str += "\",\"mqtttopicp2\":\"";
  info_str += mqtt_topicP2;
  info_str += "\"";
#endif
#if defined(RF24GW)  
  if ( info_str.length() > 3 ) info_str += ",";
  info_str += "\"RF24HUB-Server\":\"";
  info_str += RF24GW_HUB_SERVER;
  info_str += "\",\"RF24HUB-Port\":";
  info_str += String(RF24GW_HUB_UDP_PORTNO);
  info_str += ",\"RF24GW-Port\":";
  info_str += String(RF24GW_GW_UDP_PORTNO);
  info_str += ",\"RF24GW-No\":";
  info_str += String(RF24GW_NO);  
#endif
#if defined(SENSOR1)
  if ( info_str.length() > 3 ) info_str += ",";
  if (format_mqtt) {
    info_str += sensor1.sensorinfo_mqtt();
  } else {
    info_str += sensor1.sensorinfo_html();
  }
#endif
#if defined(SENSOR2)
  if ( info_str.length() > 3 ) info_str += ",";
  if (format_mqtt) {
    info_str += sensor2.sensorinfo_mqtt();
  } else {
    info_str += sensor2.sensorinfo_html();
  }
#endif
  info_str += "}";
  write2log(LOG_WEB,1,info_str.c_str());
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Sysinfo3: ");
  Serial.print(info_str.length());
  Serial.println(" byte");
#endif
  return info_str.c_str();
}


void start_AP() {
  write2log(LOG_SYS,1, "Start Accesspoint: ESPNode");
  ap_mode = true;
  if (WiFi.mode(WIFI_AP)) {
    write2log(LOG_SYS,1, "WIFI_AP => OK");
  } else {
    write2log(LOG_SYS,1, "WIFI_AP => ERROR");
  }
  delay(500);  
  if (WiFi.softAP("ESPNode")) {
    IPAddress IP = WiFi.softAPIP();
    write2log(LOG_SYS,2, "Http Server started on", IP.toString().c_str());
  } else {
    write2log(LOG_SYS,1, "Error starting AP");
  }
  wifi_ap_starttime = millis();
#if defined(MQTT)    
  do_mqtt = false;
#endif
#if defined(RF24GW)
  do_rf24gw = false;
#endif
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

  preferences.begin("credentials");
  magicno = preferences.getInt("magicno");
#if defined(DEBUG_SERIAL)
  Serial.print("MagicNo = ");
  Serial.println(magicno);
  Serial.print("MagicNo = ");
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
    loop_time_alarm = LOOP_TIME_ALARM;
#if defined(MQTT)
    do_mqtt = MQTT;
    mqtt_server = MQTT_SERVER;
    mqtt_client = MQTT_CLIENT;
    mqtt_topicP2 = MQTT_TOPICP2;
    do_log_mqtt = LOG_MQTT;
#endif
// Save the defaults for the next start!
    preferences.putInt("magicno", MAGICNO);
    preferences.putString("wifi_ssid", wifi_ssid); 
    preferences.putString("wifi_pass", wifi_pass);
    preferences.putUInt("loop_time_alarm", loop_time_alarm);
#if defined(MQTT)
    preferences.putBool("do_mqtt", do_mqtt);
    preferences.putString("mqtt_server", mqtt_server);
    preferences.putString("mqtt_client", mqtt_client);
    preferences.putString("mqtt_topicP2", mqtt_topicP2);
    preferences.putBool("do_log_mqtt", do_log_mqtt);
#endif
#if defined(RF24GW)
    do_rf24gw = RF24GW;
    rf24gw_hub_server = RF24GW_HUB_SERVER;
    rf24gw_hub_port = RF24GW_HUB_UDP_PORTNO;
    rf24gw_gw_port = RF24GW_GW_UDP_PORTNO;
    rf24gw_gw_no = RF24GW_NO;
    do_log_rf24 = LOG_RF24;
    preferences.putBool("do_rf24gw", do_rf24gw);
    preferences.putString("rf24gw_hub_server", rf24gw_hub_server);
    preferences.putInt("rf24gw_hub_port", rf24gw_hub_port);
    preferences.putInt("rf24gw_gw_port", rf24gw_gw_port);
    preferences.putInt("rf24gw_gw_no", rf24gw_gw_no);
    preferences.putBool("do_log_rf24", do_log_rf24);
#endif
    do_log_sensor = LOG_SENSOR;
    do_log_web = LOG_WEB;
    do_log_sys = LOG_SYS;
    do_log_critical = LOG_CRITICAL;
    preferences.putBool("do_log_sensor", do_log_sensor);
    preferences.putBool("do_log_web", do_log_web);
    preferences.putBool("do_log_sys", do_log_sys);
    preferences.putBool("do_log_critical", do_log_critical);
  } else {
// Wenn sich die MagicNo nicht geändert hat werden die gespeicherten Werte genommen
    wifi_ssid = preferences.getString("wifi_ssid"); 
    wifi_pass = preferences.getString("wifi_pass");
    loop_time_alarm = preferences.getUInt("loop_time_alarm");
#if defined(MQTT)
    do_mqtt = preferences.getBool("do_mqtt");
    mqtt_server = preferences.getString("mqtt_server");
    mqtt_client = preferences.getString("mqtt_client");
    mqtt_topicP2 = preferences.getString("mqtt_topicP2");
    do_log_mqtt = preferences.getBool("do_log_mqtt");
#endif
#if defined(RF24GW)
    do_rf24gw = preferences.getBool("do_rf24gw");
    rf24gw_hub_server = preferences.getString("rf24gw_hub_server");
    rf24gw_hub_port = preferences.getInt("rf24gw_hub_port");
    rf24gw_gw_port = preferences.getInt("rf24gw_gw_port");
    rf24gw_gw_no = preferences.getInt("rf24gw_gw_no");
    do_log_rf24 = preferences.getBool("do_log_rf24");
#endif
    do_log_sensor = preferences.getBool("do_log_sensor");
    do_log_web = preferences.getBool("do_log_web");
    do_log_sys = preferences.getBool("do_log_sys");
    do_log_critical = preferences.getBool("do_log_critical");
  }
//  preferences.end();
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
  Serial.print("Sensor: ");
  Serial.println(do_log_sensor?"ja":"nein");
  Serial.print("Web: ");
  Serial.println(do_log_web?"ja":"nein");
  Serial.print("Sys: ");
  Serial.println(do_log_sys?"ja":"nein");
  Serial.print("Critical: ");
  Serial.println(do_log_critical?"ja":"nein");
#endif

  if (!LittleFS.begin()) {
    ESP.restart();
    return;
  } else {
    write2log(LOG_SYS,1, "++ Begin Startup: LittleFS mounted ++");
  }

  // Connect to Wi-Fi
  if ( ! do_wifi_con() ) {
    start_AP();
  } else {
#if defined(DEBUG_SERIAL)
    IPAddress IP = WiFi.localIP();
    write2log(LOG_SYS,2, "Node Address is ", IP.toString().c_str());
#endif
    setupTime();
    if ( !getNTPtime(10) ) {
      write2log(LOG_SYS,1, "Error getting NTP Time");
    }
    lastDay = timeinfo.tm_mday;
    write2log(LOG_DAYBREAK, 0);
#ifdef ESP32
    //ToDo
#else
    write2log(log_critical, 2, "Reboot: ", ESP.getResetReason().c_str());
#endif
  }
  setup_webserver();
#if defined(SWITCH1)
  SWITCH1_BEGIN_STATEMENT
#endif
#if defined(SWITCH2)
  SWITCH2_BEGIN_STATEMENT
#endif
#if defined(SWITCH3)
  SWITCH3_BEGIN_STATEMENT
#endif
#if defined(SWITCH4)
  SWITCH4_BEGIN_STATEMENT
#endif
#if defined(SENSOR1)
  SENSOR1_BEGIN_STATEMENT
#endif
#if defined(SENSOR2)
  SENSOR2_BEGIN_STATEMENT
#endif
#if defined(SENSOR1)
  sensor1.start_measure();
#endif
#if defined(SENSOR2)
  sensor2.start_measure();
#endif
  measure_starttime = millis();
#if defined(RF24GW)
  rf24gw_setup(); 
#endif
#if defined(MQTT)
  mqtt_setup();
#endif
  write2log(LOG_SYS,1, "Setup Ende");
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
    mqtt_loop();
//    delay(0);
    yield();
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(loopmsg,29,"Looptime MQTT: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,loopmsg);
    }
#endif
#if defined(SWITCH1)
    switch1.loop();
    yield();
#endif
#if defined(SWITCH2)
    switch2.loop();
    yield();
#endif
#if defined(SWITCH3)
    switch3.loop();
    yield();
#endif
#if defined(SWITCH4)
    switch4.loop();
    yield();
#endif
#if defined(SENSOR1)
    sensor1.loop();
    yield();
#endif
#if defined(SENSOR2)
    sensor2.loop();
    yield();
#endif
    if ((millis() - loop_starttime) > loop_time_alarm) {
      snprintf(loopmsg,29,"Looptime Modules: %d",(int)(millis() - loop_starttime));
      write2log(LOG_CRITICAL,1,loopmsg);
    }
    yield();
    if ( (millis() - mqtt_last_stat) > (STATINTERVAL * 1000) ) {
#if defined(SENSOR1)
      sensor1.start_measure();
      yield();
#endif
#if defined(SENSOR2)
      sensor2.start_measure();
      yield();
#endif
      measure_starttime = millis();
      measure_started = true;
      mqtt_last_stat = millis();
    }
    if ( ((millis() - measure_starttime) > (MESSINTERVAL * 1000)) && measure_started ) {
#if defined(MQTT)
      do_send_mqtt_stat = true;
#endif
      measure_started = false;
#if defined(SENSOR1)  
      html_json = sensor1.html_stat_json();
      write2log(LOG_SENSOR,1,html_json.c_str());
      ws.textAll(html_json);
      yield();
#endif
#if defined(SENSOR2)  
      html_json = sensor2.html_stat_json();
      write2log(LOG_SENSOR,1,html_json.c_str());
      ws.textAll(html_json);
      yield();
#endif
    }
#if defined(MQTT)
    if ( (millis() - mqtt_last_tele) > (TELEINTERVAL*1000) ) {
      mqtt_last_tele = millis();
      do_send_mqtt_tele = true;
    }
#endif
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
#ifdef ESP32
      free = ESP.getFreeHeap();
      frag = 0;
      max = 0;
#else
      ESP.getHeapStats(&free, &max, &frag);
#endif
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