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
#if defined(SENSOR3)
SENSOR3_DEFINITION
#endif
#if defined(SENSOR4)
SENSOR4_DEFINITION
#endif


void fill_timeStr() {
  time(&now);                   // read the current time
  localtime_r(&now, &timeinfo); // update the structure tm with the current time
  snprintf(timeStr, 11, "[%02d:%02d:%02d]", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void getVcc(String& json) {
#if defined(ANALOGINPUT)
  json += "n.a.";
#else
#ifdef ESP32
  json += "n.a.";
#else
  char vcc_str[7];
  snprintf(vcc_str,6,"%.1f V",(float)ESP.getVcc() / 1000.0);
  json += vcc_str;
#endif
#endif
}

void write2log(log_t kat, int count, ...) {
  va_list args;
  int n = 0;
  char *c[12];

  /* Parameterabfrage initialisieren */
  va_start(args, count);
  while (n < count) {
    c[n] = (char *)va_arg(args, char *);
    n++;
  }
  fill_timeStr();
  if (preference.log_fs_sysinfo && kat == log_critical) {
    File f = LittleFS.open(DEBUGFILE, "a");
    if (f) {
      f.print(timeStr);
      n = 0;
      while (n < count) {
        f.print(c[n]);
        n++;
      }
    }
  }
  if ((preference.log_rf24 && (kat == log_rf24)   ) || (preference.log_sensor && (kat == log_sens)    ) ||
      (preference.log_mqtt && (kat == log_mqtt)   ) || (preference.log_webcmd && (kat == log_web)     ) ||
      (preference.log_sysinfo && (kat == log_sys) ) || (preference.log_sysinfo && (kat == log_critical) )) {
    debug_str = "{\"log\":\"";
    debug_str += timeStr;
    n = 0;
    while (n < count) {
      debug_str += " ";
      // Anführungszeichen innerhalb des Log-Strings führen zu Frontendproblemen.
      // Deshalb wird " gegen ' ersetzt!
      for ( unsigned int i=0; i<strlen(c[n]); i++ ) {
        if ( c[n][i] == '"' ) {
          debug_str += "'";  
        } else {
          debug_str += c[n][i];
        }
      }
      n++;
    }
    debug_str += "\"}";
//    Serial.println(debug_str);
    ws.textAll(debug_str);
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

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String mycmd = (char *)data;
    int pos1 = mycmd.indexOf(":");
    int pos2 = mycmd.indexOf("=");
    int pos = pos1;
    if (pos2 > 0) pos = pos2;
    String cmd = mycmd.substring(0, pos);
    String value = mycmd.substring(pos + 1);
#if defined(DEBUG_SERIAL_HTML)
    Serial.print("Websocket cmd: ");
    Serial.print(cmd);
    Serial.print("value: ");
    Serial.println(value); 
#endif
    prozess_cmd(cmd,value);
  }
}

void ws_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      html_json = "{\"titel1\":\"" + String(HOSTNAME) + "\"";
#if defined(HOST_DISCRIPTION)
      html_json += ",\"titel2\":\"";
      html_json += HOST_DISCRIPTION;
      html_json += "\"";
#endif
#if defined(SWITCH1)
      switch1.html_create_json_part(html_json);
#endif
#if defined(SWITCH2)
      switch2.html_create_json_part(html_json);
#endif
#if defined(SWITCH3)
      switch3.html_create_json_part(html_json);
#endif
#if defined(SWITCH4)
      switch4.html_create_json_part(html_json);
#endif
#if defined(SENSOR1)
      sensor1.html_create_json_part(html_json);
#endif
#if defined(SENSOR2)
      sensor2.html_create_json_part(html_json);
#endif
#if defined(SENSOR3)
      sensor3.html_create_json_part(html_json);
#endif
#if defined(MQTT)
      html_json += ",\"set_mqtt\":1,\"set_mqttserver\":\"";
      html_json += preference.mqttserver;
      html_json += "\",\"set_mqttclient\":\"";
      html_json += preference.mqttclient;
      html_json += "\"";
#else
      html_json += ",\"set_mqtt\":0";
#endif
// Setzen der Logging Flags 
      html_json += ",\"log_rf24\":";
      html_json += preference.log_rf24? "1": "0";
      html_json += ",\"log_mqtt\":";
      html_json += preference.log_mqtt? "1": "0";
      html_json += ",\"log_sensor\":";
      html_json += preference.log_sensor? "1": "0";
      html_json += ",\"log_sysinfo\":";
      html_json += preference.log_sysinfo? "1": "0";
      html_json += ",\"log_webcmd\":";
      html_json += preference.log_webcmd? "1": "0";
// RF24 Gateway
#if defined(RF24GW)
      html_json += ",\"set_rf24gw_enable\":1";
      html_json += ",\"set_rf24gw\":";
      html_json += preference.rf24gw_enable? "1":"0";
      html_json += ",\"set_RF24HUB-Server\":\"";
      html_json += preference.rf24gw_hub_name;
      html_json += "\",\"set_RF24HUB-Port\":\"";
      html_json += preference.rf24gw_hub_port;
      html_json += "\",\"set_RF24GW-Port\":\"";
      html_json += preference.rf24gw_gw_port;
      html_json += "\",\"set_RF24GW-No\":";
      html_json += preference.rf24gw_gw_no;
#else      
      html_json += ",\"set_rf24gw_enable\":0";
#endif      
      html_json += "}";
      write2log(log_web,1,html_json.c_str());
      ws.textAll(html_json);

#if defined(SENSOR1)
      html_json = sensor1.html_stat_json();
      ws.textAll(html_json);
#endif
#if defined(SENSOR2)
      sensor2.html_stat_json(info_str);
      ws.textAll(info_str);
#endif
#if defined(SENSOR3)
      sensor3.html_stat_json(info_str);
      ws.textAll(info_str);
#endif
#if defined(SWITCH1)
      html_json = switch1.html_stat_json();
      ws.textAll(html_json);
#endif
#if defined(SWITCH2)
//      switch2.html_stat_json(info_str);
//      ws.textAll(info_str);
#endif
#if defined(SWITCH3)
//      switch3.html_stat_json(info_str);
//      ws.textAll(info_str);
#endif
#if defined(SWITCH4)
//      switch4.html_stat_json(info_str);
//      ws.textAll(info_str);
#endif

    break;  //    case WS_EVT_CONNECT

    case WS_EVT_DISCONNECT:
//      Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
    break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket() {
  ws.onEvent(ws_onEvent);
  httpServer.addHandler(&ws);
}

void serveFile(AsyncWebServerRequest *request) {
  String myurl = request->url();
  String dataType = "text/plain";
#if defined(DEBUG_SERIAL)
  Serial.print("Requested: ");
  Serial.println(myurl);
#endif
  if (myurl == "/")
    myurl = "/index.html";
  if (LittleFS.exists(myurl)) {
#if defined(DEBUG_SERIAL)
    Serial.print(myurl);
    Serial.println("  exists");
#endif
  } else {
#if defined(DEBUG_SERIAL)
    Serial.print(myurl);
    Serial.println("  does not exist  Send: /index.html");
    Serial.println("Fileliste:");
#endif
#ifdef ESP32
    File root = LittleFS.open("/");
    if (!root) {
//      Serial.println("- failed to open / directory");
      return;
    }
    if (!root.isDirectory()) {
//      Serial.println(" - not a directory");
      return;
    }

    File file = root.openNextFile();
//    while (file) {
//      if (file.isDirectory()) {
//        Serial.print("  DIR : ");
//        Serial.println(file.name());
        /*            if(levels){
                        listDir(LittleFS, file.path(), levels -1);
                    } */
//      } else {
//        Serial.print("  FILE: ");
//        Serial.print(file.name());
//        Serial.print("\tSIZE: ");
//        Serial.println(file.size());
//      }
//      file = root.openNextFile();
//    }
#else
    Dir dir = LittleFS.openDir("/");
//    if (dir.isDirectory()) {
//      Serial.println("/ is a Directory");
//    } else {
//      Serial.println("/ is NOT a Directory");
//    }
    File f;
    while (dir.next()) {
      f = dir.openFile("r");
      f.close();
    }
#endif
    myurl = "/index.html";
  }
  if (myurl.endsWith(".html") || myurl.endsWith(".htm")) {
    dataType = "text/html";
  } else if (myurl.endsWith(".css")) {
    dataType = "text/css";
  } else if (myurl.endsWith(".js")) {
    dataType = "application/javascript";
  } else if (myurl.endsWith(".png")) {
    dataType = "image/png";
  } else if (myurl.endsWith(".gif")) {
    dataType = "image/gif";
  } else if (myurl.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if (myurl.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if (myurl.endsWith(".xml")) {
    dataType = "text/xml";
  } else if (myurl.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if (myurl.endsWith(".zip")) {
    dataType = "application/zip";
  }
  request->send(LittleFS, myurl, dataType);
}

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

void wifi_con(void) {
  if (WiFi.status() != WL_CONNECTED) {
    write2log(log_sys, 4, "Try to connect to ", ssid1, " with password ", password1);
    WiFi.mode(WIFI_STA);
#ifdef ESP32
    WiFi.setHostname(HOSTNAME);
#else
    //    WiFi.persistent(false);
    WiFi.hostname(HOSTNAME);
#endif
    WiFi.begin(ssid1, password1);
    write2log(log_sys, 2, "WIFI try to connect to ", ssid1);

    // ... Give ESP 10 seconds to connect to station.
    unsigned int i = 0;
    while (WiFi.status() != WL_CONNECTED && i < 100) {
      delay(200);
      i++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      write2log(log_sys,2, "WIFI connected to ", ssid1);
      write2log(log_sys,1, WiFi.localIP().toString().c_str());
    } else {
      write2log(log_sys,4, "Try to connect to ", ssid2, " with password ", password2);
      //      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid2, password2);
      // ... Give ESP 10 seconds to connect to station.
      unsigned int i = 0;
      while (WiFi.status() != WL_CONNECTED && i < 100) {
        delay(200);
        i++;
      }
      if (WiFi.status() == WL_CONNECTED) {
        write2log(log_sys,2, "WIFI (re)connect to ", ssid2);
        write2log(log_sys,1, WiFi.localIP().toString().c_str());
      }
    }
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.scanNetworks(true, false);
      while ( WiFi.scanComplete() < 0 ) { 
        delay(200); 
      }
      int8_t numberOfNetworks = WiFi.scanComplete();
      for (int i = 0; i < numberOfNetworks; i++) {
        write2log(log_sys,1,WiFi.SSID(i).c_str());
      }
      WiFi.scanDelete();
      File f = LittleFS.open( DEBUGFILE, "a");
      if (f) {
        fill_timeStr();
        f.printf("%s Reboot Reason: No Wifi connection\n",timeStr);
        f.close();
      }
      write2log(log_critical,1, "WIFI No connection => reboot");
      delay(3000);
      ESP.restart();
    }
    configTime(0, 0, NTP_SERVER);
    // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
    setenv("TZ", TZ_INFO, 1);
    tzset();
    if (getNTPtime(10)) { // wait up to 10sec to sync
    } else {
      ESP.restart();
    }
    lastNTPtime = time(&now);
    lastEntryTime = millis();
  }
}

void read_preferences(void) {
  File myFile = LittleFS.open("/prefs", "r");
  if (myFile) {
    myFile.read((byte *)&preference, sizeof(preference));
    myFile.close();
  }
}

void save_preferences(void) {
  File myFile = LittleFS.open("/prefs", "w");
  myFile.write((byte *)&preference, sizeof(preference));
  myFile.close();
}

// Funktionen für den Webserver

const char *mk_cmd(AsyncWebServerRequest *request) {
//  bool prefs_change = false;
  int args = request->args();
  html_json = "";
  for (int argNo = 0; argNo < args; argNo++) {
#if defined(DEBUG_SERIAL_HTML)
    Serial.print("prozess_cmd: "); Serial.print(request->argName(argNo)); Serial.print(": "); Serial.println(request->arg(argNo));
#endif
    prozess_cmd(request->argName(argNo), request->arg(argNo) );
  }
  if (html_json.length() < 2 ) html_json = "ok";
  return html_json.c_str();
}
const char *mk_wifiscan(String& info_str) {
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Generiere wifiscan ... ");
#endif
  html_json = "{ \"Wifi\": \"Scan started\" }";
  WiFi.scanNetworks(true, false);
#if defined(DEBUG_SERIAL_HTML)
  Serial.print(" ok (");
  Serial.print(info_str.length());
  Serial.println(" byte)");
#endif
  return info_str.c_str();
}

const char *mk_wifishow(String& info_str) {
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Generiere wifishow ... ");
#endif
  info_str = "{";
  int numberOfNetworks = WiFi.scanComplete();
  if (numberOfNetworks < 0) {
    switch (numberOfNetworks) {
      case -1:
        info_str += "\"Wifi\": \"Scan not finished\"";
      break;
      case -2:
        info_str += "\"Wifi\": \"Scan not started\"";
      break;
    }
  } else {
    for (int i = 0; i < numberOfNetworks; i++) {
      if ( i > 0 ) info_str += ",";
      info_str += "\"Wifi";
      info_str += String(i);
      info_str += "\":\"";
      info_str += WiFi.SSID(i);
      info_str += ", Ch:";
      info_str += String(WiFi.channel(i));
      info_str += " (";
      info_str += WiFi.RSSI(i);
      info_str += " dBm ";
      switch (WiFi.encryptionType(i)) {
      case ENC_TYPE_WEP:
          info_str += "WEP";
        break;
      case ENC_TYPE_TKIP:
          info_str += "TKIP";
        break;
      case ENC_TYPE_CCMP:
          info_str += "CCMP";
        break;
      case ENC_TYPE_AUTO:
          info_str += "Auto";
        break;
      case ENC_TYPE_NONE:
          info_str += "None";
        break;
      default:
          info_str += "unknown";
        break;
      }
      info_str += ")\"";
    }
  }
  info_str += "}";
  WiFi.scanDelete();
#if defined(DEBUG_SERIAL_HTML)
  Serial.print(" ok (");
  Serial.print(info_str.length());
  Serial.println(" byte)");
#endif
  return info_str.c_str();
}

const char *mk_sysinfo1(String& info_str) {
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
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Generiere sysinfo1 ... ");
#endif
  info_str = "{";
#ifdef ESP32
  snprintf(tmp, 99, "\"Platform\":\"%s\"", ESP.getChipModel());
#else
  info_str += "\"Platform\":\"ESP8266\"";
#endif
#ifdef ESP32
  info_str += ",\"Cores\":";
  info_str += String(ESP.getChipCores());
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
  info_str += String((float)frag / 1024.0);
  info_str += "%\"";
#ifdef ESP32
// TBD
//  char tmp1[25];
//  snprintf(tmp, 99, "\"ResetReason\":\"%s\"", getResetReason(tmp1));
#else
  info_str += ",\"ResetReason\":\"";
  info_str += ESP.getResetReason();
  info_str += "\"";
#endif
  info_str += ",\"Vcc\":\"";
  getVcc(info_str);
  info_str += "\"";
  info_str += ",\"UpTime\":\"";
  info_str += uptime.uptimestr();
  info_str += "\"";
  info_str += "}";
#if defined(DEBUG_SERIAL_HTML)
  Serial.print(" ok (");
  Serial.print(info_str.length());
  Serial.println(" byte)");
#endif
  return info_str.c_str();
}

#ifdef ESP32
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
  Serial.print("Generiere sysinfo2 ... ");
#endif
//  uptime::calculateUptime();
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
  snprintf(tmp, 99, "\"CoreVer\":\"%s\"", "unknown");
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
#if defined(DEBUG_SERIAL_HTML)
  Serial.print(" ok (");
  Serial.print(info_str.length());
  Serial.println(" byte)");
#endif
  return info_str.c_str();
}

const char *mk_sysinfo3(String& info_str) {
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Generiere sysinfo3 ... ");
#endif
  info_str = "{";
#if defined(MQTT)  
  info_str += "\"mqttserver\":\"";
  info_str += MQTT_SERVER;
  info_str += "\",\"mqttclient\":\"";
  info_str += MQTT_CLIENT;
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
  info_str += sensor1.sensorinfo();
#endif
#if defined(SENSOR2)
  if ( info_str.length() > 3 ) info_str += ",";
  info_str += sensor2.sensorinfo();
#endif
#if defined(SENSOR3)
  if ( info_str.length() > 3 ) info_str += ",";
  info_str += sensor3.sensorinfo();
#endif
  info_str += "}";
#if defined(DEBUG_SERIAL_HTML)
  Serial.print(" ok (");
  Serial.print(info_str.length());
  Serial.println(" byte)");
#endif
  return info_str.c_str();
}

/// @brief 
/// @param name 
/// @param value 
void prozess_cmd(const String cmd, const String value)  {
  write2log(log_sys,4,"prozess_cmd Cmd:",cmd.c_str(),"Val:",value.c_str());
#if defined(SWITCH1)
  if ( switch1.set( cmd, value ) ) {
    html_json = switch1.html_stat_json();
    write2log(log_sens,1,html_json.c_str());
    ws.textAll(html_json);
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch1.obj_mqtt_name.c_str()), switch1.obj_values_str.c_str());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(SWITCH2)
  if ( switch2.set( cmd, value ) ) {
    html_json = switch2.html_stat_json();
    write2log(log_sens,1,html_json.c_str());
    ws.textAll(html_json);
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch2.obj_mqtt_name.c_str()), switch2.obj_values_str.c_str());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(SWITCH3)
  if ( switch3.set( cmd, value ) ) {
    html_json = switch3.html_stat_json();
    write2log(log_sens,1,html_json.c_str());
    ws.textAll(html_json);
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch3.obj_mqtt_name.c_str()), switch3.obj_values_str.c_str());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(SWITCH4)
  if ( switch4.set( cmd, value ) ) {
    html_json = switch4.html_stat_json();
    write2log(log_sens,1,html_json.c_str());
    ws.textAll(html_json);
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch4.obj_mqtt_name.c_str()), switch4.obj_values_str.c_str());
#endif
  }
#endif
#if defined(MQTT)
  if ( cmd == "log_mqtt" ) {
    if ( (value == "1") != preference.log_mqtt) {
      preference.log_mqtt = (value == "1");
    }
  }
#endif
#if defined(RF24GW)
  if ( cmd == "rf24gw" ) {
#if defined(DEBUG_SERIAL)
    Serial.print("RF24GW is ");
    Serial.println(preference.rf24gw_enable? "1":"0");
#endif
    if ( (value == "1") != preference.rf24gw_enable) {
      preference.rf24gw_enable = (value == "1");
    }
  }
  if ( cmd == "log_rf24" ) {
    if ( (value == "1") != preference.log_rf24) {
      preference.log_rf24 = (value == "1");
    }
  }
  if ( cmd == "rf24hubname" ) {
    snprintf(preference.rf24gw_hub_name, SERVERNAMESIZE, "%s", value.c_str() );
  }
  if ( cmd == "rf24hubport" ) {
    preference.rf24gw_hub_port = value.toInt();
  }
  if ( cmd == "rf24gwport" ) {
    preference.rf24gw_gw_port = value.toInt();
  }
  if ( cmd == "rf24gwno" ) {
    preference.rf24gw_gw_no = value.toInt();
  }
#endif
  if ( cmd == "log_sensor" ) {
    preference.log_sensor = ( value == "1" );
  }
  if ( cmd == "log_webcmd" ) {
    preference.log_webcmd = ( value == "1" );
  }
  if ( cmd == "log_sysinfo" ) {
    preference.log_sysinfo = ( value == "1" );
  }
  if ( (cmd == "saveeprom") && (value == "1") ) {
    save_preferences();
    html_json = "dauerhaft gespeichert";
  }
  if ( cmd == "dellogfile" ) {
    LittleFS.remove(DEBUGFILE);
  }
  if ( cmd == "restart" ) {
    rebootflag = true;
    html_json = "Restart => OK";
  }
}

#if defined(MQTT)
const char* mk_topic(const char* part1, const char* part3) {
  mqtt_topic = part1;
  mqtt_topic += "/";
  mqtt_topic += MQTT_CLIENT;
  mqtt_topic += "/";
  mqtt_topic += part3;
  return mqtt_topic.c_str();
}

void reconnect_mqtt() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    // Attempt to connect
    if (mqttClient.connect(MQTT_CLIENT)) {
      // Once connected, publish an announcement...
      mqttClient.publish(mk_topic(MQTT_STATUS, "state"), "online");
      // ... and resubscribe
      mqttClient.subscribe(mk_topic(MQTT_COMMAND, "#"));
      if (preference.log_mqtt) {
        write2log(log_mqtt, 2, mqtt_topic.c_str()," => subcribed");
      }
    } else {
      // Wait 2 seconds before retrying
#if defined(DEBUG_SERIAL)
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 2 seconds");
#endif
      delay(2000);
    }
  }
}

void send_mqtt_stat() {
  if (!mqttClient.connected()) {
    reconnect_mqtt();
  }
  mqtt_json = "{";
#if defined(SENSOR1)  
  mqtt_json += sensor1.mqtt_json_part();
  html_json = sensor1.html_stat_json();
  Serial.println("###");
  Serial.println(html_json);
  write2log(log_sens,1,html_json.c_str());
  ws.textAll(html_json);
#endif
#if defined(SENSOR2)
  if ( info_str.length() > 3 ) info_str += ",";
  info_str += sensor2.mqtt_json_part();
#endif
#if defined(SENSOR3)  
  if ( info_str.length() > 3 ) info_str += ",";
  info_str += sensor3.mqtt_json_part();
#endif
  mqtt_json += "}";
  mqttClient.publish(mk_topic(MQTT_STATUS, "sensordata"), mqtt_json.c_str());
  write2log(log_mqtt,2, mqtt_topic.c_str(), mqtt_json.c_str());

  mqtt_json = "{";
  mqtt_json_length_old = mqtt_json.length();
#if defined(SWITCH1)  
  mqttClient.publish(mk_topic(MQTT_STATUS, switch1.show_mqtt_name()), switch1.show_value());
  write2log(log_mqtt,2, mqtt_topic.c_str(), switch1.show_value());
  mqtt_json += switch1.mqtt_json_part();
#endif
#if defined(SWITCH2)  
  if ( mqtt_json.length() > mqtt_json_length_old ) {
    mqtt_json += ",";
    mqtt_json_length_old = mqtt_json.length();
  }
  mqttClient.publish(mk_topic(MQTT_STATUS, switch2.show_mqtt_name()), switch2.show_value());
  write2log(log_mqtt,2, mqtt_topic.c_str(), switch2.show_value());
  mqtt_json += switch2.mqtt_json_part();
#endif
#if defined(SWITCH3)  
  if ( mqtt_json.length() > mqtt_json_length_old ) {
    mqtt_json += ",";
    mqtt_json_length_old = mqtt_json.length();
  }
  mqttClient.publish(mk_topic(MQTT_STATUS, switch3.show_mqtt_name()), switch3.show_value());
  write2log(log_mqtt,2, mqtt_topic.c_str(), switch3.show_value());
  mqtt_json += switch3.mqtt_json_part();
#endif
#if defined(SWITCH4)  
  if ( mqtt_json.length() > mqtt_json_length_old ) {
    mqtt_json += ",";
    mqtt_json_length_old = mqtt_json.length();
  }
  mqttClient.publish(mk_topic(MQTT_STATUS, switch4.show_mqtt_name()), switch4.show_value());
  write2log(log_mqtt,2, mqtt_topic.c_str(), switch4.show_value());
  mqtt_json += switch4.mqtt_json_part();
#endif
  mqtt_json += "}";
  mqttClient.publish(mk_topic(MQTT_STATUS, "switchdata"), mqtt_json.c_str());
  write2log(log_mqtt,2, mqtt_topic.c_str(), mqtt_json.c_str());
}

void send_mqtt_tele() {
  if (!mqttClient.connected()) {
    reconnect_mqtt();
  }
  mk_sysinfo1(mqtt_json); // Hier wird die Variable "info_str" gefüllt!
  mqttClient.publish(mk_topic(MQTT_TELEMETRIE,"info1"), mqtt_json.c_str());
  if (preference.log_mqtt) {
    write2log(log_mqtt,2, mqtt_topic.c_str(), mqtt_json.c_str());
  }
  mk_sysinfo2(mqtt_json);
  mqttClient.publish(mk_topic(MQTT_TELEMETRIE, "info2"), mqtt_json.c_str());
  if (preference.log_mqtt) {
    write2log(log_mqtt,2, mqtt_topic.c_str(), mqtt_json.c_str());
  }
  mk_sysinfo3(mqtt_json);
  mqttClient.publish(mk_topic(MQTT_TELEMETRIE, "info3"), mqtt_json.c_str());
  if (preference.log_mqtt) {
    write2log(log_mqtt,2, mqtt_topic.c_str(), mqtt_json.c_str());
  }
}

void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  char delimiter[] = "/";
  char *ptr;
  char part1[TOPIC_PART1_SIZE];
  char part2[TOPIC_PART2_SIZE];
  char part3[TOPIC_PART3_SIZE];
  char* cmd = (char*)malloc(length + 2);
  snprintf(cmd, length + 1, "%s", (char*)payload);
  write2log(log_mqtt,2, topic, cmd);
  ptr = strtok(topic, delimiter);
  if (ptr != NULL) snprintf(part1, TOPIC_PART1_SIZE, "%s", ptr);
  ptr = strtok(NULL, delimiter);
  if (ptr != NULL) snprintf(part2, TOPIC_PART2_SIZE, "%s", ptr);
  ptr = strtok(NULL, delimiter);
  if (ptr != NULL) snprintf(part3, TOPIC_PART3_SIZE, "%s", ptr);
  if ( strncmp(part1, MQTT_COMMAND, sizeof MQTT_COMMAND) == 0 ) {
    if ( strncmp(part2, MQTT_CLIENT, sizeof MQTT_CLIENT) == 0 ) {
      prozess_cmd(part3, cmd);
    }
  }
  // Free the memory
  free(cmd);
}
#endif

/****************************************************
 * Setup
 *****************************************************/
void setup() {
#if defined(DEBUG_SERIAL)
  // Serial port for debugging purposes
  Serial.begin(115200);
  int numberOfNetworks = WiFi.scanNetworks();
  for (int i = 0; i < numberOfNetworks; i++) {
    Serial.println("Available Networks: ");
    Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
  }
#endif

  // Connect to Wi-Fi
  wifi_con();

  if (!LittleFS.begin()) {
    ESP.restart();
    return;
  } else {
    write2log(log_sys,1, "++ Begin Startup: LittleFS mounted ++");
  }
  File f = LittleFS.open( DEBUGFILE, "a");
  if (f) {
    fill_timeStr();
    f.printf("%s %s Reboot Reason:%s\n",timeStr,"Setup executed",ESP.getResetReason().c_str());
    f.close();
  }

// Fill preference structure
  read_preferences();
  if ( (preference.magicno != MAGICNO) || (MAGICNO == 0) ) {
#if defined(MQTT)
    snprintf(preference.mqttclient, SERVERNAMESIZE, MQTT_CLIENT);
    snprintf(preference.mqttserver, SERVERNAMESIZE, MQTT_SERVER);
#endif
#if defined(RF24GW)
    snprintf(preference.rf24gw_hub_name, SERVERNAMESIZE, RF24GW_HUB_SERVER);
    preference.rf24gw_hub_port = RF24GW_HUB_UDP_PORTNO;
    preference.rf24gw_gw_port = RF24GW_GW_UDP_PORTNO;
    preference.rf24gw_gw_no = RF24GW_NO;
    preference.rf24gw_enable = true;
#endif
    preference.magicno = MAGICNO;
  }
  initWebSocket();
  write2log(log_web,1, "initWebsocket ok");

  httpServer.on("/wifiscan", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "application/json", mk_wifiscan(html_json)); });
  httpServer.on("/wifishow", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "application/json", mk_wifishow(html_json)); });
  httpServer.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/plain", mk_cmd(request)); });
  httpServer.on("/sysinfo1", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/plain", mk_sysinfo1(html_json)); });
  httpServer.on("/sysinfo2", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/plain", mk_sysinfo2(html_json)); });
  httpServer.on("/sysinfo3", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/plain", mk_sysinfo3(html_json)); });
//  httpServer.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
//                { request->send(200, "text/plain", do_restart()); });
  httpServer.on("/update_fw", HTTP_POST, [&](AsyncWebServerRequest *request) {},
   [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      if (!index) {
        Update.runAsync(true);
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace, U_FLASH, -1, LOW)) {
          write2log(log_sys, 1, "Error Updater begin");
        }
      }
      if(len) { Update.write(data, len); }
      if (final) { 
        if ( !Update.end(true)) {
          write2log(log_sys, 1, "Error Updater end"); 
        } else {
          rebootflag = true;
        }
      }
  });
  httpServer.on("/update_fs", HTTP_POST, [&](AsyncWebServerRequest *request) {},
    [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      // Upload handler chunks in data
      if (!index) {
        Update.runAsync(true);
        size_t fsSize = ((size_t)&_FS_end - (size_t)&_FS_start);
        if (!Update.begin(fsSize, U_FS, -1, LOW)) {
          write2log(log_sys, 1, "Error Updater begin");
        }
      }
      if (len) {
        Update.write(data, len);
      }
      if (final) {
        if (!Update.end(true)) {
          write2log(log_sys, 1, "Error Updater end");
        } else {
          rebootflag = true;
        }
      }
  });
  // This serves all static web content
  httpServer.onNotFound(serveFile);
  // Start server
  httpServer.begin();
  #if defined(RF24GW)
  // init rf24
  radio.begin();
  delay(100);
  radio.setChannel(RF24_CHANNEL);
  radio.setDataRate(RF24_SPEED);
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(0, 0);
  radio.setAutoAck(false);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(32);
  radio.setCRCLength(RF24_CRC_16);
  radio.openWritingPipe(rf24_hub2node);
  radio.openReadingPipe(1, rf24_node2hub);
  radio.startListening();
  radio.printDetails();
#endif

#if defined(RF24GW)
  udp.begin(RF24GW_GW_UDP_PORTNO);
  if (preference.log_rf24) {
    write2log(log_sys,2, "RF24: Opened UDP Port:", RF24GW_GW_UDP_PORTNO );
  }
#endif

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
#if defined(SENSOR3)
  SENSOR3_BEGIN_STATEMENT
#endif
#if defined(SENSOR1)
  sensor1.start_measure();
#endif
#if defined(SENSOR2)
  sensor2.start_measure();
#endif
#if defined(SENSOR3)
  sensor3.start_measure();
#endif
  measure_started = millis();
#if defined(MQTT)
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(callback_mqtt);
  mqttClient.setBufferSize(512);
  if (preference.log_mqtt) {
    write2log(log_mqtt,3, "MQTT: Connected to Server:", MQTT_SERVER, "Port: 1883");
  }
  send_mqtt_tele();
#endif
  write2log(log_sys,1, "Setup Ende");
}

/************************************************
 * Main Loop
*************************************************/
void loop() {
  ws.cleanupClients();
#if defined(RF24GW)
  if ( radio.available() ) {
    radio.read(&payload, sizeof(payload));
    if (preference.log_rf24) {
/*      info_str = "N>G O#:";
      info_str += payload.orderno;
      info_str += " N:";
      info_str += payload.node_id;
      info_str += " Msg:";
      info_str += payload.msg_id;
      info_str += " Mtyp:";
      info_str += payload.msg_type;
      info_str += " Hb#:";
      info_str += payload.heartbeatno; */
      write2log(log_rf24, 10, "N>G O#:",payload.orderno,"N:",payload.node_id,"Msg:",payload.msg_id,"Mtyp:",payload.msg_type,"Hb#:",payload.heartbeatno);
    }
    udpdata.gw_no = RF24GW_NO;
    memcpy(&udpdata.payload, &payload, sizeof(payload));
    udp.beginPacket(RF24GW_HUB_SERVER, RF24GW_HUB_UDP_PORTNO);
    udp.write((char*)&udpdata, sizeof(udpdata));
    udp.endPacket();
  }
  if (udp.parsePacket() > 0 ) {
    udp.read((char*)&udpdata, sizeof(udpdata));
    memcpy(&payload, &udpdata.payload, sizeof(payload));
    if (preference.log_rf24) {
/*      info_str = "G>N O#:";
      info_str += payload.orderno;
      info_str += " N:";
      info_str += payload.node_id;
      info_str += " Msg:";
      info_str += payload.msg_id;
      info_str += " Mtyp:";
      info_str += payload.msg_type;
      info_str += " Hb#:";
      info_str += payload.heartbeatno;
      write2log(log_rf24, 1, info_str.c_str()); */
      write2log(log_rf24, 10, "G>N O#:",payload.orderno,"N:",payload.node_id,"Msg:",payload.msg_id,"Mtyp:",payload.msg_type,"Hb#:",payload.heartbeatno);
    }
    radio.stopListening();
    radio.write(&payload, sizeof(payload));
    radio.startListening();
  }
#endif
#if defined(MQTT)
  if (!mqttClient.connected()) {
    reconnect_mqtt();
  }
  mqttClient.loop();
  delay(0);
#endif
#if defined(SWITCH1)
  switch1.loop();
//  if (switch1.changed()) {
//    do_send_mqtt_stat = true;
//    info_str = switch1.html_stat_json();
//    ws.textAll(info_str);
#if defined(MQTT)
//    mqttClient.publish(mk_topic(MQTT_STATUS, switch1.obj_mqtt_name.c_str()), switch1.obj_values_str.c_str());
// tbd: Zusätzliche Werte müssen noch übermittelt werden
#endif
//  }
#endif
#if defined(SWITCH2)
  switch2.loop();
//  if (switch2.changed()) {
//    do_send_mqtt_stat = true;
//    info_str = switch2.html_stat_json();
//    ws.textAll(info_str);
#if defined(MQTT)
//    mqttClient.publish(mk_topic(MQTT_STATUS, switch2.obj_mqtt_name.c_str()), switch2.obj_values_str.c_str());
// tbd: Zusätzliche Werte müssen noch übermittelt werden
#endif
//  }
#endif
#if defined(SWITCH3)
  switch3.loop();
//  if (switch3.changed()) {
//    do_send_mqtt_stat = true;
//    info_str = switch3.html_stat_json();
//    ws.textAll(info_str);
#if defined(MQTT)
//    mqttClient.publish(mk_topic(MQTT_STATUS, switch3.obj_mqtt_name.c_str()), switch3.obj_values_str.c_str());
// tbd: Zusätzliche Werte müssen noch übermittelt werden
#endif
//  }
#endif
#if defined(SWITCH4)
  switch4.loop();
//  if (switch4.changed()) {
//    do_send_mqtt_stat = true;
//    info_str = switch4.html_stat_json();
//    ws.textAll(info_str);
#if defined(MQTT)
//    mqttClient.publish(mk_topic(MQTT_STATUS, switch4.obj_mqtt_name.c_str()), switch4.obj_values_str.c_str());
// tbd: Zusätzliche Werte müssen noch übermittelt werden
#endif
//  }
#endif
#if defined(SENSOR1)
  sensor1.loop();
//  if (sensor1.changed()) { 
//    do_send_mqtt_stat = true;
//    info_str = sensor1.html_stat_json();
//    ws.textAll(info_str);
//  }
#endif
#if defined(MQTT)
  if ( do_send_mqtt_stat ) {
    send_mqtt_stat();
    do_send_mqtt_stat = false;
  }
#endif  
  if (rebootflag) {
    File f = LittleFS.open( DEBUGFILE, "a");
    if (f) {
      fill_timeStr();
      f.printf("%s Reboot Flag gesetzt\n",timeStr);
      f.close();
    }
    yield();
    delay(5000);
    yield();
    ESP.restart();
  }
  if ( (millis() - last_stat) > (STATINTERVAL * 1000) ) {
#if defined(SENSOR1)
    sensor1.start_measure();
#endif
#if defined(SENSOR2)
    sensor2.start_measure();
#endif
#if defined(SENSOR3)
    sensor3.start_measure();
#endif
    measure_starttime = millis();
    measure_started = true;
    last_stat = millis();
  }
  if ( ((millis() - measure_starttime) > (MESSINTERVAL * 1000)) && measure_started ) {
#if defined(MQTT)
    do_send_mqtt_stat = true;
#endif
    measure_started = false;
  }
#if defined(MQTT)
  if ( do_send_mqtt_stat ) { 
    send_mqtt_stat(); 
  }
  if ( (millis() - mqtt_last_tele) > (TELEINTERVAL*1000) ) {
    mqtt_last_tele = millis();
    send_mqtt_tele();
  }
#endif
  loopcount++;
  if ( loopcount > 100000) {
    uptime.update();
    loopcount = 0;
  }
}