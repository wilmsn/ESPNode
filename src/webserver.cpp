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
        sendWsMessage(tmpstr);
     break;
      case -2:
        tmpstr = "{\"wifi_network\": \"Scan not started\"}";
        sendWsMessage(tmpstr);
      break;
    }
  } else {
    tmpstr = String("{\"wifi_network\":\"") + String(numberOfNetworks) + String(" Networks:<br>\"}");
    sendWsMessage(tmpstr);
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
      sendWsMessage(tmpstr);
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
      sendWsMessage(tmpstr);

// Teil 2
      tmpstr = "{";
      tmpstr += "\"IP\":\"";
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
      sendWsMessage(tmpstr);
// Teil 3
      tmpstr = "{";
#ifdef USE_SDCARD
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
      tmpstr += RF24GW_HUB_SERVER;
      tmpstr += "\",\"RF24HUB-Port\":";
      tmpstr += String(RF24GW_HUB_UDP_PORTNO);
      tmpstr += ",\"RF24GW-Port\":";
      tmpstr += String(RF24GW_GW_UDP_PORTNO);
      tmpstr += ",\"RF24GW-No\":";
      tmpstr += String(RF24GW_NO);  
#endif
      tmpstr += "}";
      sendWsMessage(tmpstr);
      tmpstr = "{\"x\":0";
#ifdef MODULE1
  tmpstr += module1.html_info();
#endif
#ifdef MODULE2
  tmpstr += module2.html_info();
#endif
#ifdef MODULE3
  tmpstr += module3.html_info();
#endif
#ifdef MODULE4
  tmpstr += module4.html_info();
#endif
#ifdef MODULE5
  tmpstr += module5.html_info();
#endif
#ifdef MODULE6
  tmpstr += module6.html_info();
#endif
  tmpstr += "}";
  sendWsMessage(tmpstr);
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
  tmpstr += String(",\"set_RF24HUB-No\":\"") + String(rf24gw_gw_no) + String("\"");
#else      
  tmpstr += String(",\"set_rf24gw_enable\":0");
#endif
  tmpstr += String("}");
  sendWsMessage(tmpstr);
  tmpstr = "{";
#ifdef MODULE1
      module1.html_create(tmpstr);
#endif
#ifdef MODULE2
      tmpstr += ",";
      module2.html_create(tmpstr);
#endif
#ifdef MODULE3
      tmpstr += ",";
      module3.html_create(tmpstr);
#endif
#ifdef MODULE4
      tmpstr += ",";
      module4.html_create(tmpstr);
#endif
#ifdef MODULE5
      tmpstr += ",";
      module5.html_create(tmpstr);
#endif
#ifdef MODULE6
      tmpstr += ",";
      module6.html_create();
#endif
  tmpstr += "}";
  sendWsMessage(tmpstr);
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