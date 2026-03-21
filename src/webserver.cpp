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
      myjson = String("{\"wifi_network\":\"") + WiFi.SSID(i) + String(" ") + WiFi.BSSIDstr(i) + String(", Ch:") + String(WiFi.channel(i)) +
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
  bool setComma = false;
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
      myjson += String("\"") + String(",\"IdeVer\":\"") + String(ARDUINO) + String("\"");
#ifdef ESP32
      myjson += String(",\"CoreVer\":\"Arduino: ")
              + String(ESP_ARDUINO_VERSION_MAJOR) + String(".") + String(ESP_ARDUINO_VERSION_MINOR) 
              + String(".") + String(ESP_ARDUINO_VERSION_PATCH) + String("\"");
#else
      myjson += String(",\"CoreVer\":\"");
      myjson += ESP.getCoreVersion();
      myjson += String("\"");
#endif
      myjson += String(",\"SdkVer\":\"");
      myjson += String(ESP.getSdkVersion());
      myjson += String("\",\"SW\":\"") + String(SWVERSION) + String(" (") + String(__DATE__) + String(")\"}");
      sendWsMessage(myjson);
// Teil 3
      myjson = String("{\"ws_teil2\":2");
#if defined(MQTT)  
      myjson += String(",\"mqttserver\":\"") + mqtt_server +
                String("\",\"mqttclient\":\"") + mqtt_client +
                String("\",\"mqtttopicp2\":\"") + mqtt_topicP2 + String("\"");
#endif
#if defined(RF24GW)  
      myjson += String(",\"RF24HUB-Server\":\"") + rf24gw_hub_server + String("\"") +
                String(",\"RF24HUB-Port\":") + String(rf24gw_hub_port) +
                String(",\"RF24GW-Port\":") + String(rf24gw_gw_port) +
                String(",\"RF24GW-No\":") + String(rf24gw_gw_no) +
                String(",\"tab_head_nrf24l01\":\"Modul Nrf24L01\"") +
                String(",\"tab_line1_nrf24l01\":\"MOSI:#GPIO: ") + String(RF24_RADIO_MOSI_PIN) + String("\"") +
                String(",\"tab_line2_nrf24l01\":\"MISO:#GPIO: ") + String(RF24_RADIO_MISO_PIN) + String("\"") +
                String(",\"tab_line3_nrf24l01\":\"SCK:#GPIO: ") + String(RF24_RADIO_SCK_PIN) + String("\"") +
                String(",\"tab_line4_nrf24l01\":\"CE:#GPIO: ") + String(RF24_RADIO_CE_PIN) + String("\"") +
                String(",\"tab_line5_nrf24l01\":\"CSN:#GPIO: ") + String(RF24_RADIO_CSN_PIN) + String("\"") +
                String(",\"tab_line6_nrf24l01\":\"Channel:# ") + String(RF24_CHANNEL) + String("\"") +
                String(",\"tab_line7_nrf24l01\":\"Speed:# ") + String(RF24_SPEED_STR) + String("\"");
#endif
      myjson += String("}");
      sendWsMessage(myjson);
      myjson = String("{");
#ifdef MODULE1
      if (module1.html_info_set) {
        module1.html_info(myjson);
        setComma = true;
      }
#ifdef MODULE2
      if (module2.html_info_set) {
        if (setComma) {
          myjson += String(",");
          setComma = false;
        }
        module2.html_info(myjson);
        setComma = true;
      }
#ifdef MODULE3
      if (module3.html_info_set) {
        if (setComma) {
          myjson += String(",");
          setComma = false;
        }
        module3.html_info(myjson);
        setComma = true;
      }
#ifdef MODULE4
      if (module4.html_info_set) {
        if (setComma) {
          myjson += String(",");
          setComma = false;
        }
        module4.html_info(myjson);
        setComma = true;
      }
#ifdef MODULE5
      if (module5.html_info_set) {
        if (setComma) {
          myjson += String(",");
          setComma = false;
        }
        module5.html_info(myjson);
        setComma = true;
      }
#ifdef MODULE6
      if (module6.html_info_set) {
        if (setComma) {
          myjson += String(",");
          setComma = false;
        }
        module6.html_info(myjson);
        setComma = true;
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
  if ( module1.html_init_set) {
    module1.html_init(myjson);
    setComma = true;
  }
#ifdef MODULE2
  if ( setComma) {
    myjson += String(",");
    setComma = false;
  }
  if ( module2.html_init_set) {
    module2.html_init(myjson) ;
    setComma = true;
  }
#ifdef MODULE3
  if ( setComma) {
    myjson += String(",");
    setComma = false;
  }
  if ( module3.html_init_set) {
    module3.html_init(myjson);
    setComma = true;
  }
#ifdef MODULE4
  if ( setComma) {
    myjson += String(",");
    setComma = false;
  }
  if ( module4.html_init_set) {
    module4.html_init(myjson);
    setComma = true;
  }
#ifdef MODULE5
  if ( setComma) {
    myjson += String(",");
    setComma = false;
  }
  if ( module5.html_init_set) {
    module5.html_init(myjson);
    setComma = true;
  }
#ifdef MODULE6
  if ( setComma) {
    myjson += String(",");
    setComma = false;
  }
  if ( module6.html_init_set) {
    module6.html_init(myjson);
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
  bool result = false;
  initWebSocket();
  write2log(LOG_WEB,1, "initWebsocket ok");
  // This serves all static web content
  httpServer.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request) {
    // display params
    size_t count = request->params();
    for (size_t i = 0; i < count; i++) {
      const AsyncWebParameter *p = request->getParam(i);
#ifdef DEBUG_SERIAL_WEB
      Serial.printf("PARAM[%u]: %s = %s\n", i, p->name().c_str(), p->value().c_str());
#endif
      prozess_cmd(p->name(), p->value());
    }
    if ( ! cmd_result) {
      request->send(LittleFS, "/error_cmd.html", "text/html");
    } else {
      request->send(LittleFS, "/ok.html", "text/html");
    }
  });

    httpServer.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=3600");
  // Start Elegant OTA
  ElegantOTA.begin(&httpServer);
  // Start server
  httpServer.begin();
}

void webserver_loop(time_t now) {
#ifdef MODULE1
  if ( module1.html_update_set 
#ifdef MODULE2
       || module2.html_update_set
#ifdef MODULE3
       || module3.html_update_set
#ifdef MODULE4
       || module4.html_update_set
#ifdef MODULE5
       || module5.html_update_set
#ifdef MODULE6
       || module6.html_update_set
#endif  //Module6
#endif  //Module5
#endif  //Module4
#endif  //Module3
#endif  //Module2
       ) {
    String myjson = String("{");
    bool setComma = false;
    if ( module1.html_update_set) {
      module1.html_update(myjson);
      module1.html_update_set = false;
      setComma = true;
    }
#ifdef MODULE2
    if ( module2.html_update_set) {
      if ( setComma) {
        myjson += String(",");
        setComma = false;
      }
      module2.html_update(myjson);
      module2.html_update_set = false;
      setComma = true;
    }
#ifdef MODULE3
    if ( module3.html_update_set) {
      if ( setComma) {
        myjson += String(",");
        setComma = false;
      }
      module3.html_update(myjson);
      module3.html_update_set = false;
      setComma = true;
    }
#ifdef MODULE4
    if ( module4.html_update_set) {
      if ( setComma) {
        myjson += String(",");
        setComma = false;
      }
      module4.html_update(myjson);
      module4.html_update_set = false;
      setComma = true;
    }
#ifdef MODULE5
    if ( module5.html_update_set) {
      if ( setComma) {
        myjson += String(",");
        setComma = false;
      }
      module5.html_update(myjson);
      module5.html_update_set = false;
      setComma = true;
    }
#ifdef MODULE6
    if ( module6.html_update_set) {
      if ( setComma) {
        myjson += String(",");
        setComma = false;
      }
      module6.html_update(myjson);
      module6.html_update_set = false;
      setComma = true;
  }
#endif  //module6
#endif  //Module5
#endif  //Module4
#endif  //Module3
#endif  //Module2
    myjson += String("}");
    sendWsMessage(myjson);
  }
#endif  //module1
} 
