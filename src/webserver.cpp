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

void sendWsMessage(String& myMsg) {
  ws.textAll(myMsg);
  write2log(LOG_WEB,1,myMsg.c_str());
}

void prozess_wifishow() {
  String myjson;
#if defined(DEBUG_SERIAL_WEB)
  Serial.print("Generiere wifishow ... ");
#endif
  int numberOfNetworks = WiFi.scanComplete();
  if (numberOfNetworks < 0) {
    switch (numberOfNetworks) {
      case -1:
        myjson = String("{\"wifi_network\": \"Scan not finished\"}");
        sendWsMessage(myjson);
     break;
      case -2:
        myjson = String("{\"wifi_network\": \"Scan not started\"}");
        sendWsMessage(myjson);
      break;
    }
  } else {
    myjson = String("{\"wifi_network\":\"") + String(numberOfNetworks) + String(" Networks:<br>\"}");
    sendWsMessage(myjson);
    for (int i = 0; i < numberOfNetworks; i++) {
      myjson = String("{\"wifi_network\":\"") + WiFi.SSID(i) + String(", Ch:") + String(WiFi.channel(i)) +
               String(" (") + String(WiFi.RSSI(i)) + String(" dBm ");
#ifdef ESP32
      switch (WiFi.encryptionType(i)) {
      case  WIFI_AUTH_OPEN:
          myjson += String("open");
        break;
      case WIFI_AUTH_WEP:
          myjson += String("WEP");
        break;
      case WIFI_AUTH_WPA_PSK:
          myjson += String("WPA PSK");
        break;
      case WIFI_AUTH_WPA2_PSK:
          myjson += String("WPA2 PSK");
        break;
      case WIFI_AUTH_WPA_WPA2_PSK:
          myjson += String("WPA WPA2 PSK");
        break;
      case WIFI_AUTH_WPA2_ENTERPRISE:
          myjson += String("WPA2 Enterprise");
        break;
      case WIFI_AUTH_WPA3_PSK:
          myjson += String("WPA3 PSK");
        break;
      case WIFI_AUTH_WPA2_WPA3_PSK:
          myjson += String("WPA2 WPA3 PSK");
        break;
      case WIFI_AUTH_WAPI_PSK:
          myjson += String("WAPI PSK");
        break;
      case WIFI_AUTH_MAX:
          myjson += String("MAX");
        break;
      }
#else
      switch (WiFi.encryptionType(i)) {
      case ENC_TYPE_WEP:
          myjson += String("WEP");
        break;
      case ENC_TYPE_TKIP:
          myjson += String("TKIP");
        break;
      case ENC_TYPE_CCMP:
          myjson += String("CCMP");
        break;
      case ENC_TYPE_AUTO:
          myjson += String("Auto");
        break;
      case ENC_TYPE_NONE:
          myjson += String("None");
        break;
      default:
          myjson += String("unknown");
        break;
      }
#endif
      myjson += String(")\"}");
      sendWsMessage(myjson);
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
  String myjson;
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
      myjson = String("{");
#ifdef ESP32
      myjson += String("\"Platform\":\"") + String(ESP.getChipModel()) + String("\"");
#else
      myjson += String("\"Platform\":\"ESP8266\"");
#endif
#ifdef ESP32
      myjson += String(",\"Cores\":") + String(ESP.getChipCores()) +
                String(",\"PSRamSize\":\"") + String((float)ESP.getPsramSize()/1024.0) + String(" KB\"") +
                String(",\"PsRamFree\":\"") + String((float)ESP.getFreePsram()/1024.0) + String(" KB\"");
#else
      myjson += String(",\"Cores\":\"1\"");
#endif
      myjson += String(",\"Hostname\":\"");
#ifdef ESP32
      myjson += HOSTNAME;
#else
      myjson += WiFi.hostname();
#endif
      myjson += String("\"") + String(",\"CpuFreq\":\"") + String((int)(F_CPU / 1000000)) + String(" Mhz\"") +
                String(",\"FlashSize\":\"") + String((int)(ESP.getFlashChipSize() / 1024 / 1024)) + String(" MB \"") +
                String(",\"FlashFreq\":\"") + String((int)(ESP.getFlashChipSpeed() / 1000000)) + String(" Mhz\"") +
                String(",\"Sketchsize\":\"") + String(ESP.getSketchSize() / 1024.0) + String(" kB\"") +
                String(",\"Freespace\":\"") + String(ESP.getFreeSketchSpace() / 1024.0) + String(" kB\"") +
                String(",\"Heap_free\":\"") + String((float)free / 1024.0) + String(" kB\"") +
                String(",\"Heap_max\":\"") + String((float)max / 1024.0) + String(" kB\"");
#ifdef ESP32
//      myjson += "";
#else
      myjson += String(",\"Heap_frag\":\"") + String((float)frag / 1024.0) + String("%\"");
#endif
      myjson += String(",\"ResetReason\":\"");
#ifdef ESP32
      getResetReason(myjson);
#else
      myjson += ESP.getResetReason();
#endif
      myjson += String("\"") + String(",\"Vcc\":\"");
      getVcc(myjson);
      myjson += String("\"");     
#ifdef ESP32
      myjson += String(",\"MBTemp\":\"");
      myjson += temperatureRead();
      myjson += String("\"");
#endif
      myjson += String(",\"UpTime\":\"");
      myjson += uptime.uptimestr();
      myjson += String("\"}");
      sendWsMessage(myjson);

// Teil 2
      myjson = String("{\"IP\":\"");
      myjson += WiFi.localIP().toString();
      myjson += String("\"") +
                String(",\"SubNetMask\":\"");
      myjson += WiFi.subnetMask().toString();
      myjson += String("\"") +
                String(",\"GW-IP\":\"");
      myjson += WiFi.gatewayIP().toString();
      myjson += String("\"") +
                String(",\"DnsIP\":\"");
      myjson += WiFi.dnsIP().toString();
      myjson += String("\"") +
                String(",\"SSID\":\"");
      myjson += WiFi.SSID();
      myjson += String(" (");
      myjson += String(rssi);
      myjson += String("dBm / ");
      myjson += String(rssi_quality);
      myjson += String("%)\"") +
                String(",\"Channel\":\"");
      myjson += String(WiFi.channel());
      myjson += String("\"") +
                String(",\"BSSID\":\"");
      myjson += WiFi.BSSIDstr();
      myjson += String("\"") +
                String(",\"MAC\":\"");
      myjson += WiFi.macAddress();
      myjson += String("\"") +
                String(",\"IdeVer\":\"") + String(ARDUINO) + String("\"");
#ifdef ESP32
      myjson += String(",\"CoreVer\":\"unknown\"");
#else
      myjson += String(",\"CoreVer\":\"");
      myjson += ESP.getCoreVersion();
      myjson += String("\"");
#endif
      myjson += String(",\"SdkVer\":\"");
      myjson += ESP.getSdkVersion();
      myjson += String("\"") +
                String(",\"SW\":\"") + String(SWVERSION) + String(" (") + String(__DATE__) + String(")\"}");
      sendWsMessage(myjson);
// Teil 3
      myjson = String("{\"ws_teil2\":2");
#if defined(MQTT)  
      myjson += String(",\"mqttserver\":\"") + mqtt_server +
                String("\",\"mqttclient\":\"") + mqtt_client +
                String("\",\"mqtttopicp2\":\"") + mqtt_topicP2 + String("\"");
#endif
#if defined(RF24GW)  
      myjson += String(",\"RF24HUB-Server\":\"") + rf24gw_hub_server +
                String("\",\"RF24HUB-Port\":") + String(rf24gw_hub_port) +
                String(",\"RF24GW-Port\":") + String(rf24gw_gw_port) +
                String(",\"RF24GW-No\":") + String(rf24gw_gw_no);  
#endif
      myjson += String("}");
      sendWsMessage(myjson);
      myjson = String("{");
#ifdef MODULE1
      if (module1.html_has_info) {
        myjson += module1.html_info;
        set_comma = true;
      }
#ifdef MODULE2
      if (module2.html_has_info) {
        if (set_comma) myjson += String(",");
        myjson += module2.html_info;
        set_comma = true;
      }
#ifdef MODULE3
      if (module3.html_has_info) {
        if (set_comma) myjson += String(",");
        myjson += module3.html_info;
        set_comma = true;
      }
#ifdef MODULE4
      if (module4.html_has_info) {
        if (set_comma) myjson += String(",");
        myjson += module4.html_info;
        set_comma = true;
      }
#ifdef MODULE5
      if (module5.html_has_info) {
        if (set_comma) myjson += String(",");
        myjson += module5.html_info;
        set_comma = true;
      }
#ifdef MODULE6
      if (module6.html_has_info) {
        if (set_comma) myjson += String(",");
        myjson += module6.html_info;
        set_comma = true;
      }
#endif
#endif
#endif
#endif
#endif
#endif
  myjson += String("}");
  sendWsMessage(myjson);
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
  String myjson;
  bool setComma = false;
  myjson = String("{\"titel1\":\"") + String(HOSTNAME) + String("\"") +
           String(",\"wifi_ssid\":\"") + wifi_ssid + String("\"") +
           String(",\"wifi_pass\":\"") + wifi_pass + String("\"");
#if defined(HOST_DISCRIPTION)
  myjson += String(",\"titel2\":\"") + String(HOST_DISCRIPTION) + String("\"");
#endif
#if defined(MQTT)
      myjson += String(",\"set_mqtt_enable\":1") +
                String(",\"set_mqtt_active\":") + String(do_mqtt? "1": "0") +
                String(",\"set_mqttserver\":\"") + mqtt_server +
                String("\",\"set_mqttclient\":\"") + mqtt_client +
                String("\",\"set_mqtttopicp2\":\"") + mqtt_topicP2 + String("\"");
#else
  myjson += String(",\"set_mqtt_enable\":0");
#endif
// Setzen der Logging Flags 
#if defined(RF24GW)
      myjson += String(",\"log_rf24\":") + String(do_log_rf24? "1": "0");
#endif
#if defined(MQTT)
      myjson += String(",\"log_mqtt\":") + String(do_log_mqtt? "1": "0");
#endif
  myjson += String(",\"log_module\":") + String(do_log_module?"1":"0") +
            String(",\"log_system\":") + String(do_log_system?"1":"0") +
            String(",\"log_critical\":") + String(do_log_critical?"1":"0") +
            String(",\"log_web\":") + String(do_log_web?"1":"0");
// RF24 Gateway
#if defined(RF24GW)
  myjson += String(",\"set_rf24gw_enable\":1") +
            String(",\"set_rf24gw_active\":") + String(do_rf24gw?"1":"0") +
            String(",\"set_RF24HUB-Server\":\"") + rf24gw_hub_server + String("\"") +
            String(",\"set_RF24HUB-Port\":\"") + String(rf24gw_hub_port) + String("\"") +
            String(",\"set_RF24GW-Port\":\"") + String(rf24gw_gw_port) + String("\"") +
            String(",\"set_RF24GW-No\":\"") + String(rf24gw_gw_no) + String("\"");
#else      
  myjson += String(",\"set_rf24gw_enable\":0");
#endif
  myjson += String("}");
  sendWsMessage(myjson);
  myjson = String("{");
#ifdef MODULE1
  module1.html_init();
  if ( module1.html_json_filled) {
    myjson += module1.html_json;
    setComma = true;
  }
#ifdef MODULE2
  module2.html_init();
  if ( module2.html_json_filled) {
    if (setComma) myjson += String(",");
    myjson += module2.html_json;
    setComma = true;
  }
#ifdef MODULE3
  module3.html_init();
  if ( module3.html_json_filled) {
    if (setComma) myjson += String(",");
    myjson += module3.html_json;
    setComma = true;
  }
#ifdef MODULE4
  module4.html_init();
  if ( module4.html_json_filled) {
    if (setComma) myjson += String(",");
    myjson += module4.html_json;
    setComma = true;
  }
#ifdef MODULE5
  module5.html_init();
  if ( module5.html_json_filled) {
    if (setComma) myjson += String(",");
    myjson += module5.html_json;
    setComma = true;
  }
#ifdef MODULE6
  module6.html_init();
  if ( module6.html_json_filled) {
    if (setComma) myjson += String(",");
    myjson += module6.html_json;
    setComma = true;
  }
#endif  //module6
#endif  //Module5
#endif  //Module4
#endif  //Module3
#endif  //Module2
#endif  //module1
  myjson += String("}");
  sendWsMessage(myjson);
}

void ws_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
#ifdef DEBUG_SERIAL_WEB
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                    client->remoteIP().toString().c_str());
#endif
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
  httpServer.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=3600");
  // Start Elegant OTA
  ElegantOTA.begin(&httpServer);
  // Start server
  httpServer.begin();
}
