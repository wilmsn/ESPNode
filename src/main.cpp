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

// Kommentiert in main.h
void fill_timeStr() {
  time(&now);                   // read the current time
  localtime_r(&now, &timeinfo); // update the structure tm with the current time
  snprintf(timeStr, 11, "[%02d:%02d:%02d]", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void setupTime() {                              
  // deinen NTP Server einstellen (von 0 - 5 aus obiger Liste) alternativ lassen sich durch Komma getrennt bis zu 3 Server angeben
#ifdef ESP32
  configTzTime("CET-1CEST,M3.5.0/03,M10.5.0/03", NTP_SERVER);
#else
  configTime("CET-1CEST,M3.5.0,M10.5.0/3", NTP_SERVER);  
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
  // Im AP-Mode wird nichts gelogged !!!
  if ( ! ap_mode ) {
    fill_timeStr();
    if ( do_log_critical && kat == log_daybreak ) {
      File f = LittleFS.open( DEBUGFILE, "a" );
      if (f) {
        f.printf("----%d.%d.%d----\n",timeinfo.tm_mday, 1 + timeinfo.tm_mon, 1900 + timeinfo.tm_year);
        f.close();
      }
    }
    if (do_log_critical && kat == log_critical) {
      File f = LittleFS.open( DEBUGFILE, "a" );
      if (f) {
        f.print(timeStr);
        n = 0;
        while (n < count) {
          f.print(c[n]);
          n++;
        }
        f.printf("\n");
        f.close();
      }
    }
    if ((do_log_sensor && (kat == log_sensor)    ) ||
        (do_log_web && (kat == log_web)     ) ||
#if defined(MQTT)
        (do_log_mqtt && (kat == log_mqtt)   ) ||
#endif
#if defined(RF24GW)
        (do_log_rf24 && (kat == log_rf24)   ) ||
#endif
        (do_log_sys && (kat == log_sys) ) || 
        (do_log_critical && (kat == log_critical) )) {
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
  }
  va_end(args);
}

// Kommentiert in main.h
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

// Kommentiert in main.h
void ws_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      html_json = "{\"titel1\":\"" + String(HOSTNAME) + "\"";
      html_json += ",\"wifi_ssid\":\"";
      html_json += wifi_ssid;
      html_json += "\",\"wifi_pass\":\"";
      html_json += wifi_pass;
      html_json += "\"";
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
#if defined(MQTT)
      html_json += ",\"set_mqtt_enable\":1";
      html_json += ",\"set_mqtt\":";
      html_json += do_mqtt_set?"1":"0";
      html_json += ",\"set_mqttserver\":\"";
      html_json += mqtt_server;
      html_json += "\",\"set_mqttclient\":\"";
      html_json += mqtt_client;
      html_json += "\",\"set_mqtttopicp2\":\"";
      html_json += mqtt_topicP2;
      html_json += "\"";
#else
      html_json += ",\"set_mqtt_enable\":0";
#endif
// Setzen der Logging Flags 
#if defined(RF24GW)
      html_json += ",\"log_rf24\":";
      html_json += do_log_rf24? "1": "0";
#endif
#if defined(MQTT)
      html_json += ",\"log_mqtt\":";
      html_json += do_log_mqtt? "1": "0";
#endif
      html_json += ",\"log_sensor\":";
      html_json += do_log_sensor? "1": "0";
      html_json += ",\"log_sys\":";
      html_json += do_log_sys? "1": "0";
      html_json += ",\"log_critical\":";
      html_json += do_log_critical? "1": "0";
      html_json += ",\"log_web\":";
      html_json += do_log_web? "1": "0";
// RF24 Gateway
#if defined(RF24GW)
      html_json += ",\"set_rf24gw_enable\":1";
      html_json += ",\"set_rf24gw\":";
      html_json += do_rf24gw? "1":"0";
      html_json += ",\"set_RF24HUB-Server\":\"";
      html_json += rf24gw_hub_server;
      html_json += "\",\"set_RF24HUB-Port\":\"";
      html_json += rf24gw_hub_port;
      html_json += "\",\"set_RF24GW-Port\":\"";
      html_json += rf24gw_gw_port;
      html_json += "\",\"set_RF24GW-No\":";
      html_json += rf24gw_gw_no;
#else      
      html_json += ",\"set_rf24gw_enable\":0";
#endif      
      html_json += "}";
      write2log(log_web,1,html_json.c_str());
#if defined(DEBUG_SERIAL_HTML)
      Serial.println(html_json);
#endif
      ws.textAll(html_json);

#if defined(SENSOR1)
      html_json = sensor1.html_stat_json();
#if defined(DEBUG_SERIAL_HTML)
      Serial.println(html_json);
#endif
      ws.textAll(html_json);
#endif
#if defined(SENSOR2)
      html_json = sensor2.html_stat_json();
#if defined(DEBUG_SERIAL_HTML)
      Serial.println(html_json);
#endif
      ws.textAll(html_json);
#endif
#if defined(SWITCH1)
      html_json = switch1.html_stat_json();
#if defined(DEBUG_SERIAL_HTML)
      Serial.println(html_json);
#endif
      ws.textAll(html_json);
#endif
#if defined(SWITCH2)
      html_json = switch2.html_stat_json();
#if defined(DEBUG_SERIAL_HTML)
      Serial.println(html_json);
#endif
      ws.textAll(html_json);
#endif
#if defined(SWITCH3)
      html_json = switch3.html_stat_json();
#if defined(DEBUG_SERIAL_HTML)
      Serial.println(html_json);
#endif
      ws.textAll(html_json);
#endif
#if defined(SWITCH4)
      html_json = switch4.html_stat_json();
#if defined(DEBUG_SERIAL_HTML)
      Serial.println(html_json);
#endif
      ws.textAll(html_json);
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

// Kommentiert in main.h
void initWebSocket() {
  ws.onEvent(ws_onEvent);
  httpServer.addHandler(&ws);
}

// Kommentiert in main.h
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
  write2log(log_sys, 4, "Try to connect to ", wifi_ssid.c_str(), " with password ", wifi_pass.c_str());
  WiFi.mode(WIFI_STA);
#ifdef ESP32
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
#else
    //    WiFi.persistent(false);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(wifi_ssid, wifi_pass);
#endif
  write2log(log_sys, 2, "WIFI try to connect to ", wifi_ssid);

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
    write2log(log_sys, 2, " OK connected!", wifi_ssid);
  } else {
    write2log(log_sys, 2, " ERROR not connected!", wifi_ssid);
    retval = false;
  }
  
  return retval;
}

// Kommentiert in main.h
const char *mk_wifiscan() {
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Generiere wifiscan ... ");
#endif
  html_json = "{ \"Wifi\": \"Scan started\" }";
  WiFi.scanNetworks(true, false);
#if defined(DEBUG_SERIAL_HTML)
  Serial.print(" ok (");
  Serial.print(html_json.length());
  Serial.println(" byte)");
#endif
  return html_json.c_str();
}


// Kommentiert in main.h
const char *mk_wifishow() {
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Generiere wifishow ... ");
#endif
  html_json = "{";
  int numberOfNetworks = WiFi.scanComplete();
  if (numberOfNetworks < 0) {
    switch (numberOfNetworks) {
      case -1:
        html_json += "\"Wifi\": \"Scan not finished\"";
      break;
      case -2:
        html_json += "\"Wifi\": \"Scan not started\"";
      break;
    }
  } else {
    for (int i = 0; i < numberOfNetworks; i++) {
      if ( i > 0 ) html_json += ",";
      html_json += "\"Wifi";
      html_json += String(i);
      html_json += "\":\"";
      html_json += WiFi.SSID(i);
      html_json += ", Ch:";
      html_json += String(WiFi.channel(i));
      html_json += " (";
      html_json += WiFi.RSSI(i);
      html_json += " dBm ";
#ifdef ESP32
      switch (WiFi.encryptionType(i)) {
      case  WIFI_AUTH_OPEN:
          html_json += "open";
        break;
      case WIFI_AUTH_WEP:
          html_json += "WEP";
        break;
      case WIFI_AUTH_WPA_PSK:
          html_json += "WPA PSK";
        break;
      case WIFI_AUTH_WPA2_PSK:
          html_json += "WPA2 PSK";
        break;
      case WIFI_AUTH_WPA_WPA2_PSK:
          html_json += "WPA WPA2 PSK";
        break;
      case WIFI_AUTH_WPA2_ENTERPRISE:
          html_json += "WPA2 Enterprise";
        break;
      case WIFI_AUTH_WPA3_PSK:
          html_json += "WPA3 PSK";
        break;
      case WIFI_AUTH_WPA2_WPA3_PSK:
          html_json += "WPA2 WPA3 PSK";
        break;
      case WIFI_AUTH_WAPI_PSK:
          html_json += "WAPI PSK";
        break;
      case WIFI_AUTH_MAX:
          html_json += "MAX";
        break;
      }
#else
      switch (WiFi.encryptionType(i)) {
      case ENC_TYPE_WEP:
          html_json += "WEP";
        break;
      case ENC_TYPE_TKIP:
          html_json += "TKIP";
        break;
      case ENC_TYPE_CCMP:
          html_json += "CCMP";
        break;
      case ENC_TYPE_AUTO:
          html_json += "Auto";
        break;
      case ENC_TYPE_NONE:
          html_json += "None";
        break;
      default:
          html_json += "unknown";
        break;
      }
#endif
      html_json += ")\"";
    }
//#endif
  }
  html_json += "}";
  WiFi.scanDelete();
#if defined(DEBUG_SERIAL_HTML)
  Serial.print(" ok (");
  Serial.print(html_json.length());
  Serial.println(" byte)");
#endif
  return html_json.c_str();
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
  write2log(log_sys,1,info_str.c_str());
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
  write2log(log_sys,1,info_str.c_str());
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
  write2log(log_sys,1,info_str.c_str());
#if defined(DEBUG_SERIAL_HTML)
  Serial.print("Sysinfo3: ");
  Serial.print(info_str.length());
  Serial.println(" byte");
#endif
  return info_str.c_str();
}

// Kommentiert in main.h
const char *mk_cmd(AsyncWebServerRequest *request) {
//  bool prefs_change = false;
  int args = request->args();
  html_json = "";
  cmd_no = 0;
  for (int argNo = 0; argNo < args; argNo++) {
#if defined(DEBUG_SERIAL_HTML)
    Serial.print("prozess_cmd: ");
    Serial.print(request->argName(argNo));
    Serial.print(": ");
    Serial.println(request->arg(argNo));
#endif
    prozess_cmd(request->argName(argNo), request->arg(argNo) );
  }
  if ( cmd_no > 0 ) {
    if ( rebootflag ) { 
      html_json = "Reboot ok"; 
    } else {
      html_json = "ok";
    }
  } else {
    html_json = "No change";
  }
  return html_json.c_str();
}

// Kommentiert in main.h
void prozess_cmd(const String cmd, const String value)  {
  write2log(log_sys,4,"prozess_cmd Cmd:",cmd.c_str(),"Val:",value.c_str());
#if defined(SWITCH1)
  if ( switch1.set( cmd, value ) ) {
    html_json = switch1.html_stat_json();
    write2log(log_sensor,1,html_json.c_str());
    ws.textAll(html_json);
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch1.show_mqtt_name()), switch1.show_value());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(SWITCH2)
  if ( switch2.set( cmd, value ) ) {
    html_json = switch2.html_stat_json();
    write2log(log_sensor,1,html_json.c_str());
    ws.textAll(html_json);
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch2.show_mqtt_name()), switch2.show_value());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(SWITCH3)
  if ( switch3.set( cmd, value ) ) {
    html_json = switch3.html_stat_json();
    write2log(log_sensor,1,html_json.c_str());
    ws.textAll(html_json);
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch3.show_mqtt_name()), switch3.show_value());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(SWITCH4)
  if ( switch4.set( cmd, value ) ) {
    html_json = switch4.html_stat_json();
    write2log(log_sensor,1,html_json.c_str());
    ws.textAll(html_json);
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch4.show_mqtt_name()), switch4.show_value());
#endif
  }
#endif
#if defined(MQTT)
  if ( cmd == "log_mqtt" ) {
    if ( (value == "1") != do_log_mqtt) {
      do_log_mqtt = (value == "1");
      cmd_no++;
    }
    preferences.putBool("log_mqtt", do_log_mqtt);
  }
  if ( cmd == "mqtt" ) {
    if ( (value == "1") != do_mqtt_set ) {
      do_mqtt_set = ( value == "1" );
      do_mqtt = do_mqtt_set;
      preferences.putBool("do_mqtt", do_mqtt);
      rebootflag = true;
      cmd_no++;
    }
  }
  if ( cmd == "mqttclient" ) {
    if ( mqtt_client != value ) {
      mqtt_client = value;
      preferences.putString("mqtt_client", mqtt_client);
      cmd_no++;
    }
  }
  if ( cmd == "mqtttopicp2" ) {
    if ( mqtt_topicP2 != value ) {
      mqtt_topicP2 = value;
      preferences.putString("mqtt_topicP2", mqtt_topicP2);
      cmd_no++;
    }
  }
  if ( cmd == "mqttserver" ) {
    if ( mqtt_server != value ) {
      mqtt_server = value;
      preferences.putString("mqtt_server", mqtt_server);
      cmd_no++;
    }
  }
#endif
#if defined(RF24GW)
  if ( cmd == "rf24gw" ) {
#if defined(DEBUG_SERIAL)
    Serial.print("RF24GW is ");
    Serial.println(do_rf24gw? "1":"0");
#endif
    if ( (value == "1") != do_rf24gw) {
      do_rf24gw = (value == "1");
      preferences.putBool("do_rf24gw", do_rf24gw);
    }
  }
  if ( cmd == "log_rf24" ) {
    if ( (value == "1") != do_log_rf24) {
      do_log_rf24 = (value == "1");
      preferences.putBool("do_log_rf24", do_log_rf24);
    }
  }
  if ( cmd == "rf24hubname" ) {
    rf24gw_hub_server = value;
  }
  if ( cmd == "rf24hubport" ) {
    rf24gw_hub_port = value.toInt();
  }
  if ( cmd == "rf24gwport" ) {
    rf24gw_gw_port = value.toInt();
  }
  if ( cmd == "rf24gwno" ) {
    rf24gw_gw_no = value.toInt();
  }
  if ( cmd == "log_rf24" ) {
    do_log_rf24 = ( value == "1" );
    preferences.putBool("do_log_rf24", do_log_rf24);
  }
#endif
  if ( cmd == "wifi_ssid" ) {
    if ( wifi_ssid != value ) {
      preferences.putString("wifi_ssid", value);
      rebootflag = true;
      cmd_no++;
    }
  }
  if ( cmd == "wifi_pass" ) {
    if ( wifi_pass != value ) {
      preferences.putString("wifi_pass", value);
      rebootflag = true;
      cmd_no++;
    }
  }
  if ( cmd == "log_sensor" ) {
    if ( do_log_sensor != ( value == "1" ) ) {
      do_log_sensor = ( value == "1" );
      preferences.putBool("do_log_sensor", do_log_sensor);
      cmd_no++;
    }
  }
  if ( cmd == "log_web" ) {
    if ( do_log_web != ( value == "1" ) ) {
      do_log_web = ( value == "1" );
      preferences.putBool("do_log_web", do_log_web);
      cmd_no++;
    }
  }
  if ( cmd == "log_sys" ) {
    if ( do_log_sys != ( value == "1" ) ) {
      do_log_sys = ( value == "1" );
      preferences.putBool("do_log_sys", do_log_sys);
      cmd_no++;
    }
  }
  if ( cmd == "log_critical" ) {
    if ( do_log_critical != ( value == "1" ) ) {
      do_log_critical = ( value == "1" );
      preferences.putBool("do_log_critical", do_log_critical);
      cmd_no++;
    }
  }
  if ( cmd == "dellogfile" ) {
    LittleFS.remove(DEBUGFILE);
    File f = LittleFS.open( DEBUGFILE, "a");
    if (f) {
      f.printf("----%d.%d.%d----\n",timeinfo.tm_mday, 1 + timeinfo.tm_mon, 1900 + timeinfo.tm_year);
      f.close();
    }

    cmd_no++;
  }
  if ( cmd == "restart" || cmd == "reboot" ) {
    rebootflag = true;
    cmd_no++;
  }
}

#if defined(MQTT)
// dokumentiert in main.h
const char* mk_topic(const char* part1, const char* part3) {
  mqtt_topic = part1;
  mqtt_topic += "/";
  mqtt_topic += mqtt_topicP2;
  mqtt_topic += "/";
  mqtt_topic += part3;
  return mqtt_topic.c_str();
}

// dokumentiert in main.h
void reconnect_mqtt() {
  // Loop until we're reconnected
  if (do_mqtt) {
    while (!mqttClient.connected()) {
      // Attempt to connect
      // boolean PubSubClient::connect(const char *id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage)
      if (mqttClient.connect(mqtt_client.c_str(), mk_topic(MQTT_TELEMETRIE, "LWT"), MQTT_QOS, MQTT_WILLRETAIN, "offline")) {
        // Once connected, publish an announcement...
        mqttClient.publish(mk_topic(MQTT_TELEMETRIE, "LWT"), "online");
        // ... and resubscribe
        mqttClient.subscribe(mk_topic(MQTT_COMMAND, "#"));
        if (do_log_mqtt) {
          write2log(log_mqtt, 2, mqtt_topic.c_str()," => subcribed");
        }
      } else {
        // Wait 2 seconds before retrying
#if defined(DEBUG_SERIAL)
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 2 seconds");
        Serial.print("Server: ");
        Serial.print(mqtt_server);
        Serial.print("; Client: ");
        Serial.print(mqtt_client);
        Serial.print("; TopicP2: ");
        Serial.println(mqtt_topicP2);
#endif
        delay(2000);
      }
    }
  }
}

// dokumentiert in main.h
void send_mqtt_stat() {
  if (do_mqtt) {
    if (!mqttClient.connected()) {
      reconnect_mqtt();
    }
    mqtt_json = "{";
#if defined(SENSOR1)  
    mqtt_json += sensor1.mqtt_json_part();
#endif
#if defined(SENSOR2)
    if ( mqtt_json.length() > 3 ) mqtt_json += ",";
    mqtt_json += sensor2.mqtt_json_part();
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
}

// dokumentiert in main.h
void send_mqtt_tele() {
  if (do_mqtt) {
#if defined(DEBUG_SERIAL)
    Serial.println("Sending Mqtt Tele");
#endif
    if (!mqttClient.connected()) {
      reconnect_mqtt();
    }
    mk_sysinfo1(mqtt_json); // Hier wird die Variable "info_str" gefüllt!
    mqttClient.publish(mk_topic(MQTT_TELEMETRIE,"info1"), mqtt_json.c_str());
    if (do_log_mqtt) {
      write2log(log_mqtt,2, mqtt_topic.c_str(), mqtt_json.c_str());
    }
    mk_sysinfo2(mqtt_json);
    mqttClient.publish(mk_topic(MQTT_TELEMETRIE, "info2"), mqtt_json.c_str());
    if (do_log_mqtt) {
      write2log(log_mqtt,2, mqtt_topic.c_str(), mqtt_json.c_str());
    }
    mk_sysinfo3(mqtt_json, true);
    mqttClient.publish(mk_topic(MQTT_TELEMETRIE, "info3"), mqtt_json.c_str());
    if (do_log_mqtt) {
      write2log(log_mqtt,2, mqtt_topic.c_str(), mqtt_json.c_str());
    }
  }
}

// dokumentiert in main.h
void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  char delimiter[] = "/";
  char *ptr;
  char part1[TOPIC_PART1_SIZE];
  char part2[TOPIC_PART2_SIZE];
  char part3[TOPIC_PART3_SIZE];
  char* cmd = (char*)malloc(length + 2);
  if (do_mqtt) {
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
}
#endif //MQTT enabled

#if defined(RF24GW)
// dokumentiert in main.h
void writeRf242log(const char* senddir, payload_t pl) {
  if (do_log_rf24) {
      tmp_str = senddir;
      tmp_str += " O:";
      tmp_str += String(pl.orderno);
      tmp_str += " N:";
      tmp_str += String(pl.node_id);
      tmp_str += " M:";
      tmp_str += String(pl.msg_id);
      tmp_str += " MT:";
      tmp_str += String(pl.msg_type);
      tmp_str += " HB:";
      tmp_str += String(pl.heartbeatno);
      write2log(log_rf24, 1, tmp_str.c_str());
   }
}
#endif

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
#if defined(MQTT)
    do_mqtt = MQTT;
    do_mqtt_set = MQTT;
    mqtt_server = MQTT_SERVER;
    mqtt_client = MQTT_CLIENT;
    mqtt_topicP2 = MQTT_TOPICP2;
    do_log_mqtt = LOG_MQTT;
#endif
// Save the defaults for the next start!
    preferences.putInt("magicno", MAGICNO);
    preferences.putString("wifi_ssid", wifi_ssid); 
    preferences.putString("wifi_pass", wifi_pass);
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
    preferences.putBool("log_rf24", log_rf24);
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
#if defined(MQTT)
    do_mqtt = preferences.getBool("do_mqtt");
    do_mqtt_set = do_mqtt;
    mqtt_server = preferences.getString("mqtt_server");
    mqtt_client = preferences.getString("mqtt_client");
    mqtt_topicP2 = preferences.getString("mqtt_topicP2");
    do_log_mqtt = preferences.getBool("log_mqtt");
#endif
#if defined(RF24GW)
    do_rf24gw = preferences.getBool("do_rf24gw");
    rf24gw_hub_server = preferences.getString("rf24gw_hub_server");
    rf24gw_hub_port = preferences.getInt("rf24gw_hub_port");
    rf24gw_gw_port = preferences.getInt("rf24gw_gw_port");
    rf24gw_gw_no = preferences.getInt("rf24gw_gw_no");
    do_log_rf24 = preferences.getBool("log_rf24");
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
    write2log(log_sys,1, "++ Begin Startup: LittleFS mounted ++");
  }

  // Connect to Wi-Fi
  if ( ! do_wifi_con() ) {
    ap_mode = true;
    WiFi.mode(WIFI_AP);
    delay(500);  
    WiFi.softAP("ESPNode");
#if defined(MQTT)    
    do_mqtt = false;
#endif
#if defined(RF24GW)
    do_rf24gw = false;
#endif
#if defined(DEBUG_SERIAL)
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Http Server started on ");
    Serial.println(IP);
#endif
  } else {
#if defined(DEBUG_SERIAL)
    IPAddress IP = WiFi.localIP();
    Serial.print("Node Address is ");
    Serial.println(IP);
#endif
    setupTime();
    getNTPtime(10); 
    lastDay = timeinfo.tm_mday;
    write2log(log_daybreak, 0);
#ifdef ESP32
    //ToDo
#else
    write2log(log_critical, 2, "Reboot: ", ESP.getResetReason().c_str());
#endif
  }
  initWebSocket();
  write2log(log_web,1, "initWebsocket ok");

  httpServer.on("/wifiscan", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "application/json", mk_wifiscan()); });
  httpServer.on("/wifishow", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "application/json", mk_wifishow()); });
  httpServer.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/plain", mk_cmd(request)); });
  httpServer.on("/sysinfo1", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/plain", mk_sysinfo1(html_json)); });
  httpServer.on("/sysinfo2", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/plain", mk_sysinfo2(html_json)); });
  httpServer.on("/sysinfo3", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/plain", mk_sysinfo3(html_json,false)); });
  httpServer.on("/update_fw", HTTP_POST, [&](AsyncWebServerRequest *request) {},
   [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      if (!index) {
#ifdef ESP32
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) { 
#else
        Update.runAsync(true);
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace, U_FLASH, -1, LOW)) {
#endif
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
#ifdef ESP32
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) { 
#else
        Update.runAsync(true);
        size_t fsSize = ((size_t)&_FS_end - (size_t)&_FS_start);
        if (!Update.begin(fsSize, U_FS, -1, LOW)) {
#endif
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
  httpServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });
  httpServer.serveStatic("/", LittleFS, "/");
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
  if (udp.begin(rf24gw_gw_port) == 1) {
    if (do_log_rf24) {
      write2log(log_sys,2, "RF24: Opened UDP Port:", String(rf24gw_hub_port).c_str() );
    }
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
#if defined(SENSOR1)
  sensor1.start_measure();
#endif
#if defined(SENSOR2)
  sensor2.start_measure();
#endif
  measure_started = millis();
#if defined(MQTT)
  if ( do_mqtt ) {
    mqttClient.setServer(mqtt_server.c_str(), 1883);
    mqttClient.setCallback(callback_mqtt);
    mqttClient.setBufferSize(512);
    if (do_log_mqtt) {
      write2log(log_mqtt,3, "MQTT: Connected to Server:", mqtt_server.c_str(), "Port: 1883");
    }
    send_mqtt_tele();
  }
#endif
  write2log(log_sys,1, "Setup Ende");
}

/************************************************
 * Main Loop
*************************************************/
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    write2log(log_critical,1,"WiFi connection lost => reconnect");
    delay(1000);
    int i=0;
    while ( ! do_wifi_con() ) {
      delay(10000);
      i++;
      if ( i>360 ) {
        write2log(log_critical,1,"Wifi already 1 hour offline");
      }
    }
    write2log(log_critical,1,"WiFi reconnected");
  }
  ws.cleanupClients();
#if defined(RF24GW)
  if ( radio.available() ) {
    radio.read(&payload, sizeof(payload));
    udpdata.gw_no = rf24gw_gw_no;
    writeRf242log("N>G", payload);
    memcpy(&udpdata.payload, &payload, sizeof(payload));
    udp.beginPacket(rf24gw_hub_server.c_str(), rf24gw_hub_port);
    udp.write((const unsigned char*)&udpdata, sizeof(udpdata));
    udp.endPacket();
  }
  if (udp.parsePacket() > 0 ) {
    udp.read((char*)&udpdata, sizeof(udpdata));
    memcpy(&payload, &udpdata.payload, sizeof(payload));
    writeRf242log("G>N", payload);
    radio.stopListening();
    radio.write(&payload, sizeof(payload));
    radio.startListening();
  }
#endif
#if defined(MQTT)
  if ( do_mqtt ) {
    if (!mqttClient.connected()) {
      reconnect_mqtt();
    }
    mqttClient.loop();
    delay(0);
  }
#endif
#if defined(SWITCH1)
  switch1.loop();
#endif
#if defined(SWITCH2)
  switch2.loop();
#endif
#if defined(SWITCH3)
  switch3.loop();
#endif
#if defined(SWITCH4)
  switch4.loop();
#endif
#if defined(SENSOR1)
  sensor1.loop();
#endif
#if defined(SENSOR2)
  sensor2.loop();
#endif
#if defined(MQTT)
  if ( do_mqtt ) {
    if ( do_send_mqtt_stat ) {
      send_mqtt_stat();
      do_send_mqtt_stat = false;
    }
  }
#endif  
  if ( rebootflag ) {
    preferences.end();
    write2log(log_critical,1,"Reboot Flag gesetzt => reboot");
    yield();
    delay(2000);
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
    measure_starttime = millis();
    measure_started = true;
    last_stat = millis();
  }
  if ( ((millis() - measure_starttime) > (MESSINTERVAL * 1000)) && measure_started ) {
#if defined(MQTT)
    do_send_mqtt_stat = true;
#endif
    measure_started = false;
#if defined(SENSOR1)  
    html_json = sensor1.html_stat_json();
    write2log(log_sensor,1,html_json.c_str());
    ws.textAll(html_json);
#endif
#if defined(SENSOR2)  
    html_json = sensor2.html_stat_json();
    write2log(log_sens,1,html_json.c_str());
    ws.textAll(html_json);
#endif
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
// Dinge die täglich erledigt werden sollen
  if ( lastDay != timeinfo.tm_mday ) {
    setupTime();
    write2log(log_daybreak,0);
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
    write2log(log_critical,1,tmp_str.c_str());
    uptime.update();
    lastHour = timeinfo.tm_hour;
  }
}