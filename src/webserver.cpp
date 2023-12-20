#include "webserver.h"

// Kommentiert in webserver.h
void initWebSocket() {
  ws.onEvent(ws_onEvent);
  httpServer.addHandler(&ws);
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
    html_json += "\"Wifi\":\"";
    html_json += numberOfNetworks;
    html_json += " Networks\"";
    for (int i = 0; i < numberOfNetworks; i++) {
      html_json += ",\"Wifi";
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
  Serial.println(html_json);
#endif
  return html_json.c_str();
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
#if defined(ESP8266)
      html_json += ",\"platform\":\"esp8266\"";
#endif
#if defined(ESP32)
      html_json += ",\"platform\":\"esp32\"";
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


void setup_webserver() {
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
  // This serves all static web content
  httpServer.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  ElegantOTA.begin(&httpServer);    // Start AsyncElegantOTA

  // Start server
  httpServer.begin();
}