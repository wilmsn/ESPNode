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

void prozess_wifishow() {
#if defined(DEBUG_SERIAL_WEB)
  Serial.print("Generiere wifishow ... ");
#endif
  int numberOfNetworks = WiFi.scanComplete();
  if (numberOfNetworks < 0) {
    switch (numberOfNetworks) {
      case -1:
        html_json = "{\"wifi_network\": \"Scan not finished\"}";
        ws.textAll(html_json);
     break;
      case -2:
        html_json = "{\"wifi_network\": \"Scan not started\"}";
        ws.textAll(html_json);
      break;
    }
  } else {
    html_json = "{\"wifi_network\":\"";
    html_json += numberOfNetworks;
    html_json += " Networks:<br>\"}";
    ws.textAll(html_json);
    for (int i = 0; i < numberOfNetworks; i++) {
      html_json = "{\"wifi_network\":\"";
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
      html_json += ")\"}";
      ws.textAll(html_json);
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
#if defined(ESP8266)
//      html_json += ",\"platform\":\"esp8266\"";
#endif
#if defined(ESP32)
//      html_json += ",\"platform\":\"esp32\"";
#endif
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
      html_json = "{";
#ifdef ESP32
      html_json +=  "\"Platform\":\"";
      html_json +=  ESP.getChipModel();
      html_json += "\"";
#else
      html_json += "\"Platform\":\"ESP8266\"";
#endif
#ifdef ESP32
      html_json += ",\"Cores\":";
      html_json += String(ESP.getChipCores());
      html_json += ",\"PSRamSize\":\"";
      html_json += String((float)ESP.getPsramSize()/1024.0);
      html_json += " KB\"";
      html_json += ",\"PsRamFree\":\"";
      html_json += String((float)ESP.getFreePsram()/1024.0);
      html_json += " KB\"";
#else
      html_json += ",\"Cores\":\"1\"";
#endif
      html_json += ",\"Hostname\":\"";
#ifdef ESP32
      html_json += HOSTNAME;
#else
      html_json += WiFi.hostname();
#endif
      html_json += "\"";
      html_json += ",\"CpuFreq\":\"";
      html_json += String((int)(F_CPU / 1000000));
      html_json += " Mhz\"";
      html_json += ",\"FlashSize\":\"";
      html_json += String((int)(ESP.getFlashChipSize() / 1024 / 1024));
      html_json += " MB \"";
      html_json += ",\"FlashFreq\":\"";
      html_json += String((int)(ESP.getFlashChipSpeed() / 1000000));
      html_json += " Mhz\"";
      html_json += ",\"Sketchsize\":\"";
      html_json += String(ESP.getSketchSize() / 1024.0);
      html_json += " kB\"";
      html_json += ",\"Freespace\":\"";
      html_json += String(ESP.getFreeSketchSpace() / 1024.0);
      html_json += " kB\"";
      html_json += ",\"Heap_free\":\"";
      html_json += String((float)free / 1024.0);
      html_json += " kB\"";
      html_json += ",\"Heap_max\":\"";
      html_json += String((float)max / 1024.0);
      html_json += " kB\"";
      html_json += ",\"Heap_frag\":\"";
#ifdef ESP32
      html_json += "n.a.\"";
#else
      html_json += String((float)frag / 1024.0);
      html_json += "%\"";
#endif
      html_json += ",\"ResetReason\":\"";
#ifdef ESP32
      char tmp1[20];
      html_json += getResetReason(tmp1);
#else
      html_json += ESP.getResetReason();
#endif
      html_json += "\"";
      html_json += ",\"Vcc\":\"";
      getVcc(html_json);
      html_json += "\"";
      html_json += ",\"UpTime\":\"";
      html_json += uptime.uptimestr();
      html_json += "\"";
      html_json += "}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);

// Teil 2
      html_json = "{";
      html_json += "\"IP\":\"";
      html_json += WiFi.localIP().toString();
      html_json += "\"";
      html_json += ",\"SubNetMask\":\"";
      html_json += WiFi.subnetMask().toString();
      html_json += "\"";
      html_json += ",\"GW-IP\":\"";
      html_json += WiFi.gatewayIP().toString();
      html_json += "\"";
      html_json += ",\"DnsIP\":\"";
      html_json += WiFi.dnsIP().toString();
      html_json += "\"";
      html_json += ",\"SSID\":\"";
      html_json += WiFi.SSID();
      html_json += " (";
      html_json += String(rssi);
      html_json += "dBm / ";
      html_json += String(rssi_quality);
      html_json += "%)\"";
      html_json += ",\"Channel\":\"";
      html_json += String(WiFi.channel());
      html_json += "\"";
      html_json += ",\"BSSID\":\"";
      html_json += WiFi.BSSIDstr();
      html_json += "\"";
      html_json += ",\"MAC\":\"";
      html_json += WiFi.macAddress();
      html_json += "\"";
      html_json += ",\"IdeVer\":\"";
      html_json += String(ARDUINO);
      html_json += "\"";
#ifdef ESP32
      html_json += ",\"CoreVer\":\"unknown\"";
#else
      html_json += ",\"CoreVer\":\"";
      html_json += ESP.getCoreVersion();
      html_json += "\"";
#endif
      html_json += ",\"SdkVer\":\"";
      html_json += ESP.getSdkVersion();
      html_json += "\"";
      html_json += ",\"SW\":\"";
      html_json += SWVERSION;
      html_json += " (";
      html_json += __DATE__;
      html_json += ")\"";
      html_json += "}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
// Teil 3
      html_json = "{";
#ifdef USE_SDCARD
      html_json += "\"sdcard_enable\":1";
      html_json += ",\"sdcard_size\":"+String(sd_cardsize);
      html_json += ",\"sdcard_used\":"+String(sd_usedbytes);
#else
      html_json += "\"sdcard_enable\":0";
#endif
#if defined(MQTT)  
      html_json += ",\"mqttserver\":\"";
      html_json += mqtt_server;
      html_json += "\",\"mqttclient\":\"";
      html_json += mqtt_client;
      html_json += "\",\"mqtttopicp2\":\"";
      html_json += mqtt_topicP2;
      html_json += "\"";
#endif
#if defined(RF24GW)  
      html_json += ",\"RF24HUB-Server\":\"";
      html_json += RF24GW_HUB_SERVER;
      html_json += "\",\"RF24HUB-Port\":";
      html_json += String(RF24GW_HUB_UDP_PORTNO);
      html_json += ",\"RF24GW-Port\":";
      html_json += String(RF24GW_GW_UDP_PORTNO);
      html_json += ",\"RF24GW-No\":";
      html_json += String(RF24GW_NO);  
#endif
      html_json += "}";
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
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
#if defined(MODULE1)
      module1.html_create_json_part(html_json);
#endif
#if defined(MODULE2)
      module2.html_create_json_part(html_json);
#endif
#if defined(MODULE3)
      module3.html_create_json_part(html_json);
#endif
#if defined(MODULE4)
      module4.html_create_json_part(html_json);
#endif
#if defined(MODULE5)
      module5.html_create_json_part(html_json);
#endif
#if defined(MODULE6)
      module6.html_create_json_part(html_json);
#endif
#if defined(MQTT)
      html_json += ",\"set_mqtt_enable\":1";
      html_json += ",\"set_mqtt_active\":";
      html_json += do_mqtt?"1":"0";
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
      html_json += ",\"log_module\":";
      html_json += do_log_module? "1": "0";
      html_json += ",\"log_system\":";
      html_json += do_log_system? "1": "0";
      html_json += ",\"log_critical\":";
      html_json += do_log_critical? "1": "0";
      html_json += ",\"log_web\":";
      html_json += do_log_web? "1": "0";
// RF24 Gateway
#if defined(RF24GW)
      html_json += ",\"set_rf24gw_enable\":1";
      html_json += ",\"set_rf24gw_active\":";
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
      write2log(LOG_WEB,1,html_json.c_str());
      ws.textAll(html_json);
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

/*
void mk_cmd(AsyncWebServerRequest *request) {
//  bool prefs_change = false;
  int args = request->args();
  html_json = "";
  cmd_no = 0;
  for (int argNo = 0; argNo < args; argNo++) {
#if defined(DEBUG_SERIAL_WEB)
    Serial.print("prozess_cmd: ");
    Serial.print(request->argName(argNo));
    Serial.print(": ");
    Serial.println(request->arg(argNo));
#endif
    prozess_cmd(request->argName(argNo), request->arg(argNo) );
  }
  if ( cmd_no > 0 ) {
    if ( rebootflag ) { 
      html_json = "{\"alert\":\"Reboot ok\"}"; 
      ws.textAll(html_json);
    } else {
      html_json = "{\"alert\":\"ok\"}";
      ws.textAll(html_json);
   }
  } else {
    html_json = "{\"alert\":\"No change\"}";
    ws.textAll(html_json);
  }
}
*/

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