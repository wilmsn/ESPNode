#include "webserver.h"

/// @brief Create AsyncWebServer object on port 80
AsyncWebServer httpServer(80);
/// @brief Ein Server für die Websockets
AsyncWebSocket ws("/ws");

int cmd_no = 0;

void initWebSocket() {
  ws.onEvent(ws_onEvent);
  httpServer.addHandler(&ws);
}

void sendWsMessage(String& _myMsg) {
  String myMsg = _myMsg;
  ws.textAll(myMsg);
  write2log(LOG_WEB,2,"[WEB]",myMsg.c_str());
}

void sendWsMessage(String& _myMsg, uint8_t kat) {
  String myMsg = _myMsg;
  ws.textAll(myMsg);
  write2log(kat,1,myMsg.c_str());
}

void prozess_wifishow() {
  String tmpstr;
#if defined(DEBUG_SERIAL_WEB)
  Serial.print("Generiere wifishow ... ");
#endif
  int numberOfNetworks = WiFi.scanComplete();
  if (numberOfNetworks < 0) {
    switch (numberOfNetworks) {
      case -1:
        tmpstr = "{\"wifi_network\": \"Scan not finished\"}";
        sendWsMessage(tmpstr,LOG_WEB);
     break;
      case -2:
        tmpstr = "{\"wifi_network\": \"Scan not started\"}";
        sendWsMessage(tmpstr, LOG_WEB);
      break;
    }
  } else {
    tmpstr = String("{\"wifi_network\":\"") + String(numberOfNetworks) + String(" Networks:<br>\"}");
    sendWsMessage(tmpstr, LOG_WEB);
    for (int i = 0; i < numberOfNetworks; i++) {
      tmpstr = String("{\"wifi_network\":\"") + WiFi.SSID(i) + String(", Ch:") + String(WiFi.channel(i))
               + String(" (") + String(WiFi.RSSI(i)) + String(" dBm ");
#ifdef ESP32
      switch (WiFi.encryptionType(i)) {
      case  WIFI_AUTH_OPEN:
          tmpstr += String("open");
        break;
      case WIFI_AUTH_WEP:
          tmpstr += String("WEP");
        break;
      case WIFI_AUTH_WPA_PSK:
          tmpstr += String("WPA PSK");
        break;
      case WIFI_AUTH_WPA2_PSK:
          tmpstr += String("WPA2 PSK");
        break;
      case WIFI_AUTH_WPA_WPA2_PSK:
          tmpstr += String("WPA WPA2 PSK");
        break;
      case WIFI_AUTH_WPA2_ENTERPRISE:
          tmpstr += String("WPA2 Enterprise");
        break;
      case WIFI_AUTH_WPA3_PSK:
          tmpstr += String("WPA3 PSK");
        break;
      case WIFI_AUTH_WPA2_WPA3_PSK:
          tmpstr += String("WPA2 WPA3 PSK");
        break;
      case WIFI_AUTH_WAPI_PSK:
          tmpstr += String("WAPI PSK");
        break;
      case WIFI_AUTH_MAX:
          tmpstr += String("MAX");
        break;
      }
#else
      switch (WiFi.encryptionType(i)) {
      case ENC_TYPE_WEP:
          tmpstr += String("WEP");
        break;
      case ENC_TYPE_TKIP:
          tmpstr += String("TKIP");
        break;
      case ENC_TYPE_CCMP:
          tmpstr += String("CCMP");
        break;
      case ENC_TYPE_AUTO:
          tmpstr += String("Auto");
        break;
      case ENC_TYPE_NONE:
          tmpstr += String("None");
        break;
      default:
          tmpstr += String("unknown");
        break;
      }
#endif
      tmpstr += ")\"}";
      sendWsMessage(tmpstr, LOG_WEB);
    }
//#endif
  }
  WiFi.scanDelete();
#if defined(DEBUG_SERIAL_WEB)
  Serial.print(" ok (");
#endif
}

void prozess_wifiscan() {
#if defined(DEBUG_SERIAL_WEB)
  Serial.print("Generiere wifiscan ... ");
#endif
  WiFi.scanNetworks(true, false);
#if defined(DEBUG_SERIAL_WEB)
  Serial.print(" ok (");
#endif
}

void prozess_sysinfo() {
  String tmpstr;
  bool set_comma = false;
// Daten für Sysinfo
// Teil 1
      uint32_t free;
      uint32_t max;
      uint8_t frag;
#ifdef ESP32
      free = ESP.getFreeHeap();
      max = ESP.getMaxAllocHeap();
#else
      ESP.getHeapStats(&free, &max, &frag);
#endif
      tmpstr = String("{");
#ifdef ESP32
      tmpstr += String("\"Platform\":\"");
      tmpstr += String(ESP.getChipModel());
      tmpstr += String("\"");
#else
      tmpstr += String("\"Platform\":\"ESP8266\"");
#endif
#ifdef ESP32
      tmpstr += String(",\"Cores\":");
      tmpstr += String(ESP.getChipCores());
      tmpstr += ",\"PSRamSize\":\"";
      tmpstr += String((float)ESP.getPsramSize()/1024.0);
      tmpstr += " KB\"";
      tmpstr += ",\"PsRamFree\":\"";
      tmpstr += String((float)ESP.getFreePsram()/1024.0);
      tmpstr += " KB\"";
#else
      tmpstr += ",\"Cores\":\"1\"";
#endif
      tmpstr += ",\"Hostname\":\"";
#ifdef ESP32
      tmpstr += HOSTNAME;
#else
      tmpstr += WiFi.hostname();
#endif
      tmpstr += "\"";
      tmpstr += ",\"CpuFreq\":\"";
      tmpstr += String((int)(F_CPU / 1000000));
      tmpstr += " Mhz\"";
      tmpstr += ",\"FlashSize\":\"";
      tmpstr += String((int)(ESP.getFlashChipSize() / 1024 / 1024));
      tmpstr += " MB \"";
      tmpstr += ",\"FlashFreq\":\"";
      tmpstr += String((int)(ESP.getFlashChipSpeed() / 1000000));
      tmpstr += " Mhz\"";
      tmpstr += ",\"Sketchsize\":\"";
      tmpstr += String(ESP.getSketchSize() / 1024.0);
      tmpstr += " kB\"";
      tmpstr += ",\"Freespace\":\"";
      tmpstr += String(ESP.getFreeSketchSpace() / 1024.0);
      tmpstr += " kB\"";
      tmpstr += ",\"Heap_free\":\"";
      tmpstr += String((float)free / 1024.0);
      tmpstr += " kB\"";
      tmpstr += ",\"Heap_max\":\"";
      tmpstr += String((float)max / 1024.0);
      tmpstr += " kB\"";
      tmpstr += ",\"Heap_frag\":\"";
#ifdef ESP32
      tmpstr += "n.a.\"";
#else
      tmpstr += String((float)frag / 1024.0);
      tmpstr += "%\"";
#endif
      tmpstr += ",\"ResetReason\":\"";
#ifdef ESP32
      char tmp1[20];
      tmpstr += getResetReason(tmp1);
#else
      tmpstr += ESP.getResetReason();
#endif
      tmpstr += "\"";
      tmpstr += ",\"Vcc\":\"";
      getVcc(tmpstr);
      tmpstr += "\"";
      tmpstr += ",\"UpTime\":\"";
      tmpstr += uptime.uptimestr();
      tmpstr += "\"";
      tmpstr += "}";
      sendWsMessage(tmpstr, LOG_WEB);

// Teil 2
      tmpstr = "{\"IP\":\"";
      tmpstr += WiFi.localIP().toString();
      tmpstr += "\"";
      tmpstr += ",\"SubNetMask\":\"";
      tmpstr += WiFi.subnetMask().toString();
      tmpstr += "\"";
      tmpstr += ",\"GW-IP\":\"";
      tmpstr += WiFi.gatewayIP().toString();
      tmpstr += "\"";
      tmpstr += ",\"DnsIP\":\"";
      tmpstr += WiFi.dnsIP().toString();
      tmpstr += "\"";
      tmpstr += ",\"SSID\":\"";
      tmpstr += WiFi.SSID();
      tmpstr += " (";
      tmpstr += String(rssi);
      tmpstr += "dBm / ";
      tmpstr += String(rssi_quality);
      tmpstr += "%)\"";
      tmpstr += ",\"Channel\":\"";
      tmpstr += String(WiFi.channel());
      tmpstr += "\"";
      tmpstr += ",\"BSSID\":\"";
      tmpstr += WiFi.BSSIDstr();
      tmpstr += "\"";
      tmpstr += ",\"MAC\":\"";
      tmpstr += WiFi.macAddress();
      tmpstr += "\"";
      tmpstr += ",\"IdeVer\":\"";
      tmpstr += String(ARDUINO);
      tmpstr += "\"";
#ifdef ESP32
      tmpstr += ",\"CoreVer\":\"unknown\"";
#else
      tmpstr += ",\"CoreVer\":\"";
      tmpstr += ESP.getCoreVersion();
      tmpstr += "\"";
#endif
      tmpstr += ",\"SdkVer\":\"";
      tmpstr += ESP.getSdkVersion();
      tmpstr += "\"";
      tmpstr += ",\"SW\":\"";
      tmpstr += SWVERSION;
      tmpstr += " (";
      tmpstr += __DATE__;
      tmpstr += ")\"";
      tmpstr += "}";
      sendWsMessage(tmpstr, LOG_WEB);
// Teil 3
      tmpstr = "{\"ws_teil2\":2,";
#ifdef USE_AUDIO_MEDIA
      tmpstr += "\"sdcard_enable\":1";
      tmpstr += ",\"sdcard_size\":"+String(sd_cardsize);
      tmpstr += ",\"sdcard_used\":"+String(sd_usedbytes);
#else
      tmpstr += "\"sdcard_enable\":0";
#endif
#if defined(MQTT)  
      tmpstr += ",\"mqttserver\":\"";
      tmpstr += mqtt_server;
      tmpstr += "\",\"mqttclient\":\"";
      tmpstr += mqtt_client;
      tmpstr += "\",\"mqtttopicp2\":\"";
      tmpstr += mqtt_topicP2;
      tmpstr += "\"";
#endif
#if defined(RF24GW)  
      tmpstr += ",\"RF24HUB-Server\":\"";
      tmpstr += rf24gw_hub_server;
      tmpstr += "\",\"RF24HUB-Port\":";
      tmpstr += String(rf24gw_hub_port);
      tmpstr += ",\"RF24GW-Port\":";
      tmpstr += String(rf24gw_gw_port);
      tmpstr += ",\"RF24GW-No\":";
      tmpstr += String(rf24gw_gw_no);  
#endif
      tmpstr += "}";
      sendWsMessage(tmpstr, LOG_WEB);
      tmpstr = "{";
#ifdef MODULE1
      if (module1.html_has_info) {
        if (module1.html_info.length() > 2) {
          tmpstr += module1.html_info;
          set_comma = true;
        }
      }
#ifdef MODULE2
      if (module2.html_has_info) {
        if (module2.html_info.length() > 2) {
          if (set_comma) tmpstr += ",";
          tmpstr += module2.html_info;
          set_comma = true;
        }
      }
#ifdef MODULE3
      if (module3.html_has_info) {
        if (module3.html_info.length() > 2) {
          if (set_comma) tmpstr += ",";
          tmpstr += module3.html_info;
          set_comma = true;
        }
      }
#ifdef MODULE4
      if (module4.html_has_info) {
        if (module4.html_info.length() > 2) {
          if (set_comma) tmpstr += ",";
          tmpstr += module4.html_info;
          set_comma = true;
        }
      }
#ifdef MODULE5
      if (module5.html_has_info) {
        if (module5.html_info.length() > 2) {
          if (set_comma) tmpstr += ",";
          tmpstr += module5.html_info;
          set_comma = true;
        }
      }
#ifdef MODULE6
      if (module6.html_has_info) {
        if (module6.html_info.length() > 2) {
          if (set_comma) tmpstr += ",";
          tmpstr += module6.html_info;
          set_comma = true;
        }
      }
#endif
#endif
#endif
#endif
#endif
#endif
  tmpstr += "}";
  sendWsMessage(tmpstr, LOG_WEB);
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
#if defined(DEBUG_SERIAL_WEB)
    Serial.print("Websocket cmd: ");
    Serial.print(cmd);
    Serial.print(" value: ");
    Serial.println(value); 
#endif
    if (cmd == String("ws_sysinfo")) prozess_sysinfo();
    else if (cmd == String("wifiscan")) prozess_wifiscan();
    else if (cmd == String("wifishow")) prozess_wifishow();
    else prozess_cmd(cmd,value);
  }
}

void handleWebSocketInit(void *arg, uint8_t *data, size_t len) {
  String tmpstr;
  bool setComma = false;
  tmpstr = String("{\"titel1\":\"") + String(HOSTNAME) + String("\"");
  tmpstr += String(",\"wifi_ssid\":\"") + wifi_ssid + String("\"");
  tmpstr += String(",\"wifi_pass\":\"") + wifi_pass + String("\"");
#if defined(HOST_DISCRIPTION)
  tmpstr += String(",\"titel2\":\"") + String(HOST_DISCRIPTION) + String("\"");
#endif
#if defined(MQTT)
      tmpstr += ",\"set_mqtt_enable\":1";
      tmpstr += ",\"set_mqtt_active\":";
      tmpstr += do_mqtt?"1":"0";
      tmpstr += ",\"set_mqttserver\":\"";
      tmpstr += mqtt_server;
      tmpstr += "\",\"set_mqttclient\":\"";
      tmpstr += mqtt_client;
      tmpstr += "\",\"set_mqtttopicp2\":\"";
      tmpstr += mqtt_topicP2;
      tmpstr += "\"";
#else
  tmpstr += String(",\"set_mqtt_enable\":0");
#endif
// Setzen der Logging Flags 
#if defined(RF24GW)
      tmpstr += String(",\"log_rf24\":") + String(do_log_rf24? "1": "0");
#endif
#if defined(MQTT)
      tmpstr += String(",\"log_mqtt\":") + String(do_log_mqtt? "1": "0");
#endif
  tmpstr += String(",\"log_module\":") + String(do_log_module?"1":"0");
  tmpstr += String(",\"log_system\":") + String(do_log_system?"1":"0");
  tmpstr += String(",\"log_critical\":") + String(do_log_critical?"1":"0");
  tmpstr += String(",\"log_web\":") + String(do_log_web?"1":"0");
// RF24 Gateway
#if defined(RF24GW)
  tmpstr += String(",\"set_rf24gw_enable\":1");
  tmpstr += String(",\"set_rf24gw_active\":") + String(do_rf24gw?"1":"0");
  tmpstr += String(",\"set_RF24HUB-Server\":\"") + rf24gw_hub_server + String("\"");
  tmpstr += String(",\"set_RF24HUB-Port\":\"") + String(rf24gw_hub_port) + String("\"");
  tmpstr += String(",\"set_RF24GW-Port\":\"") + String(rf24gw_gw_port) + String("\"");
  tmpstr += String(",\"set_RF24GW-No\":\"") + String(rf24gw_gw_no) + String("\"");
#else      
  tmpstr += String(",\"set_rf24gw_enable\":0");
#endif
  tmpstr += String("}");
  sendWsMessage(tmpstr, LOG_WEB);
  tmpstr = "{";
#ifdef MODULE1
  if (module1.html_init.length() > 2) {
    tmpstr += module1.html_init;
    setComma = true;
  }
  if (module1.html_stat.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module1.html_stat;
    setComma = true;
  }
#ifdef MODULE2
  if (module2.html_init.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module2.html_init;
  }
  if (module2.html_stat.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module2.html_stat;
    setComma = true;
  }
#ifdef MODULE3
  if (module3.html_init.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module3.html_init;
  }
  if (module3.html_stat.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module3.html_stat;
    setComma = true;
  }
#ifdef MODULE4
  if (module4.html_init.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module4.html_init;
  }
  if (module4.html_stat.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module4.html_stat;
    setComma = true;
  }
#ifdef MODULE5
  if (module5.html_init.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module5.html_init;
  }
  if (module5.html_stat.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module5.html_stat;
    setComma = true;
  }
#ifdef MODULE6
  if (module6.html_init.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module6.html_init;
  }
  if (module6.html_stat.length() > 2) {
    if (setComma) tmpstr += String(",");
    tmpstr += module6.html_stat;
  }
#endif  //module6
#endif  //Module5
#endif  //Module4
#endif  //Module3
#endif  //Module2
#endif  //module1
  tmpstr += String("}");
  sendWsMessage(tmpstr, LOG_WEB);
}

void ws_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      handleWebSocketInit(arg, data, len);
    break;  //    case WS_EVT_CONNECT
    case WS_EVT_DISCONNECT:
//      Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
    break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
    default:
    break;
  }
}

void setup_webserver() {
  initWebSocket();
  write2log(LOG_WEB,1, "initWebsocket ok");
  // This serves all static web content
  httpServer.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  // Start Elegant OTA
  ElegantOTA.begin(&httpServer);
  // Start server
  httpServer.begin();
}