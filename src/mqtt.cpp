#include "common.h"

/// @brief Ein Objekt für den MQTT Wifi Client
WiFiClient mqtt_wifi_client;
/// @brief Ein Objekt als MQTT Client
/// @param   Der Wifi Client
PubSubClient mqttClient(mqtt_wifi_client);
/// @brief In diesem String wird der Topic der aktuellen MQTT Message gespeichert
String mqtt_topic;
/// @brief Ein Flag zur Triggerung der MQTT Status Daten
bool do_send_mqtt_stat = false;
/// @brief Ein Flag zur Triggerung der MQTT Telemetrie Daten
bool do_send_mqtt_tele = false;

time_t last_mqtt_tele = 0;

time_t last_mqtt_stat = 0;


bool do_mqtt;
String mqtt_server;
String mqtt_client;
String mqtt_topicP2;
bool do_log_mqtt;


const char* mk_topic(const char* part1, const char* part3) {
  mqtt_topic = part1;
  mqtt_topic += "/";
  mqtt_topic += mqtt_topicP2;
  mqtt_topic += "/";
  mqtt_topic += part3;
  return mqtt_topic.c_str();
}

void reconnect_mqtt() {
  // Versuche zu verbinden. Die Funktion wird nur angesteuert wenn die mqtt Verbindung getrennt ist!!!
  // boolean PubSubClient::connect(const char *id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage)
  if (mqttClient.connect(mqtt_client.c_str(), mk_topic(MQTT_TELEMETRIE, "LWT"), MQTT_QOS, MQTT_WILLRETAIN, "offline")) {
    // Once connected, publish an announcement...
    mqttClient.publish(mk_topic(MQTT_TELEMETRIE, "LWT"), "online");
    // ... and resubscribe
    mqttClient.subscribe(mk_topic(MQTT_COMMAND, "#"));
    if (do_log_mqtt) {
      write2log(LOG_MQTT, 2, mqtt_topic.c_str()," => subcribed");
    }
  } else {
    if (do_log_mqtt) {
      write2log(LOG_MQTT, 1, "Mqtt => not connected");
    }
  }
}

void send_mqtt_dev_state() {
#ifdef MODULE1
  if (module1.obj_is_state) {
    mqttClient.publish(mk_topic(MQTT_STATUS, "state"), module1.obj_state.c_str());
    write2log(LOG_MQTT,2, mqtt_topic.c_str(), module1.obj_state.c_str());
  }
#endif
#ifdef MODULE2
  if (module2.obj_is_state) {
    mqttClient.publish(mk_topic(MQTT_STATUS, "state"), module2.obj_state.c_str());
    write2log(LOG_MQTT,2, mqtt_topic.c_str(), module2.obj_state.c_str());
  }
#endif
#ifdef MODULE3
  if (module3.obj_is_state) {
    mqttClient.publish(mk_topic(MQTT_STATUS, "state"), module3.obj_state.c_str());
    write2log(LOG_MQTT,2, mqtt_topic.c_str(), module3.obj_state.c_str());
  }
#endif
#ifdef MODULE4
  if (module4.obj_is_state) {
    mqttClient.publish(mk_topic(MQTT_STATUS, "state"), module4.obj_state.c_str());
    write2log(LOG_MQTT,2, mqtt_topic.c_str(), module4.obj_state.c_str());
  }
#endif
#ifdef MODULE5
  if (module5.obj_is_state) {
    mqttClient.publish(mk_topic(MQTT_STATUS, "state"), module5.obj_state.c_str());
    write2log(LOG_MQTT,2, mqtt_topic.c_str(), module5.obj_state.c_str());
  }
#endif
#ifdef MODULE6
  if (module6.obj_is_state) {
    mqttClient.publish(mk_topic(MQTT_STATUS, "state"), module6.obj_state.c_str());
    write2log(LOG_MQTT,2, mqtt_topic.c_str(), module6.obj_state.c_str());
  }
#endif
}

void send_mqtt_stat() {
  String tmpstr = "{";
#ifdef MODULE1
  tmpstr += module1.mqtt_stat();
#endif      
#ifdef MODULE2
  tmpstr += String(",") + module2.mqtt_stat();
#endif
#ifdef MODULE3
  tmpstr += String(",") + module3.mqtt_stat();
#endif
#ifdef MODULE4
  tmpstr += String(",") + module4.mqtt_stat();
#endif
#ifdef MODULE5
  tmpstr += String(",") + module5.mqtt_stat();
#endif
#ifdef MODULE6
  tmpstr += String(",") + module6.mqtt_stat();
#endif
  tmpstr += "}";
  mqttClient.publish(mk_topic(MQTT_STATUS, "stat"), tmpstr.c_str());
  write2log(LOG_MQTT,2, mqtt_topic.c_str(), tmpstr.c_str());
  send_mqtt_dev_state();
}

void send_mqtt_tele() {
  String tmpstr;
  uint32_t free;
  uint32_t max;
  uint8_t frag;
  bool set_komma = false;
#ifdef ESP32
  free = ESP.getFreeHeap();
  max = ESP.getMaxAllocHeap();
#else
  ESP.getHeapStats(&free, &max, &frag);
#endif
  if (do_mqtt) {
#if defined(DEBUG_SERIAL)
    Serial.println("Sending Mqtt Tele");
#endif
    if (mqttClient.connected()) {
      tmpstr = "{";
      tmpstr += String("\"IP\":\"") + WiFi.localIP().toString() + String("\"");
      tmpstr += String(",\"SSID\":\"") + WiFi.SSID()+String(" (") + String(rssi)+String("dBm / ") + String(rssi_quality) + String("%)\"");
      tmpstr += String(",\"BSSID\":\"") + WiFi.BSSIDstr() + String("\"");
      tmpstr += String(",\"Channel\":") + String(WiFi.channel());
      tmpstr += String(",\"DnsIP\":\"") + WiFi.dnsIP().toString() + String("\"");
      tmpstr += String(",\"GW-IP\":\"") + WiFi.gatewayIP().toString() + String("\"");
      tmpstr += String(",\"Hostname\":\"") +
#ifdef ESP32
                String(HOSTNAME) + String("\"");
#else
                WiFi.hostname() + String("\"");
#endif
      tmpstr += String(",\"MAC\":\"") + WiFi.macAddress() + String("\"");
      tmpstr += String(",\"SubNetMask\":\"") + WiFi.subnetMask().toString() + String("\"");

#if defined(MQTT)  
      tmpstr += String(",\"mqttserver\":\"") + mqtt_server + String("\"");
      tmpstr += String(",\"mqttclient\":\"") + mqtt_client + String("\"");
      tmpstr += String(",\"mqtttopicp2\":\"") + mqtt_topicP2 + String("\"");
#endif
#if defined(RF24GW)  
      tmpstr += String(",\"RF24HUB-Server\":\"") + String(RF24GW_HUB_SERVER) + String("\""); 
      tmpstr += String(",\"RF24HUB-Port\":") + String(RF24GW_HUB_UDP_PORTNO);
      tmpstr += String(",\"RF24GW-Port\":") + String(RF24GW_GW_UDP_PORTNO);
      tmpstr += String(",\"RF24GW-No\":") + String(RF24GW_NO);  
#endif
      tmpstr += "}";
      mqttClient.publish(mk_topic(MQTT_TELEMETRIE,"netinfo"), tmpstr.c_str());
      if (do_log_mqtt) {
        write2log(LOG_MQTT,2, mqtt_topic.c_str(), tmpstr.c_str());
      }

      tmpstr = "{";
      tmpstr += String("\"CpuFreq\":\"") + String((int)(F_CPU / 1000000)) + String(" Mhz\"");
      tmpstr += String(",\"FlashSize\":\"")+String((int)(ESP.getFlashChipSize() / 1024 / 1024))+String(" MB\"");
      tmpstr += String(",\"FlashFreq\":\"")+String((int)(ESP.getFlashChipSpeed() / 1000000))+String(" Mhz\"");
      tmpstr += String(",\"Sketchsize\":\"")+String(ESP.getSketchSize() / 1024.0)+String(" kB\"");
      tmpstr += String(",\"Freespace\":\"")+String((float)ESP.getFreeSketchSpace() / 1024.0)+String(" kB\"");
#ifdef USE_SDCARD
      tmpstr += String(",\"SDCard_size\":\"")+String(sd_cardsize/1024/1024)+String("\"");
      tmpstr += String(",\"SDCard_used\":\"")+String(sd_usedbytes/1024/1024)+String("\"");
#endif
      tmpstr += String(",\"Vcc\":\""); getVcc(tmpstr); tmpstr += String("\"");
      tmpstr += String(",\"Heap_free\":\"")+String((float)free / 1024.0)+String(" kB\"");
      tmpstr += String(",\"Heap_max\":\"")+String((float)max / 1024.0)+String(" kB\"");
      tmpstr += String(",\"Heap_frag\":\"")+
#ifdef ESP32
                String("n.a.\"");
#else
                String((float)frag / 1024.0)+String("%\"");
#endif
      tmpstr += String(",\"ResetReason\":\"");
#ifdef ESP32
      char tmp1[20];
      tmpstr += getResetReason(tmp1);
      tmpstr += String("\"");
#else
      tmpstr += ESP.getResetReason() + String("\"");
#endif
      tmpstr += String(",\"CoreVer\":\"") +
#ifdef ESP32
                String("unknown\"");
#else
                ESP.getCoreVersion() + String("\"");
#endif
#ifdef ESP32
      tmpstr += String(",\"Cores\":") + String(ESP.getChipCores());
      tmpstr += String(",\"PSRamSize\":\"") + String((float)ESP.getPsramSize()/1024.0) + String(" KB\"");
      tmpstr += String(",\"PsRamFree\":\"") + String((float)ESP.getFreePsram()/1024.0) + String(" KB\"");
#else
      tmpstr += String(",\"Cores\":1");
#endif
      tmpstr += String(",\"IdeVer\":\"") + String(ARDUINO) + String("\"");
      tmpstr += String(",\"Platform\":\"") +
#ifdef ESP32
                String(ESP.getChipModel()) + String("\"");
#else
                String("ESP8266\"");
#endif
      tmpstr += String(",\"SdkVer\":\"") + ESP.getSdkVersion() + String("\"");
      tmpstr += String(",\"SW\":\"") + String(SWVERSION) + String(" (") + String(__DATE__) + String(")\"");
      tmpstr += String(",\"UpTime\":\"") + String(uptime.uptimestr()) + String("\"");

      tmpstr += "}";
      mqttClient.publish(mk_topic(MQTT_TELEMETRIE,"sysinfo"), tmpstr.c_str());
      if (do_log_mqtt) {
        write2log(LOG_MQTT,2, mqtt_topic.c_str(), tmpstr.c_str());
      }

      tmpstr = "{";
#ifdef MODULE1
      if (module1.mqtt_has_info()) {
        tmpstr += module1.mqtt_info();
        set_komma = true;
      }
#endif
#ifdef MODULE2  
      if (module2.mqtt_has_info()) {
        if(set_komma) tmpstr += ",";
        tmpstr += module2.mqtt_info();
        set_komma = true;
      }
#endif
#ifdef MODULE3
      if (module3.mqtt_has_info()) {
        if(set_komma) tmpstr += ",";
        tmpstr += module3.mqtt_info();
        set_komma = true;
      }
#endif
#ifdef MODULE4
      if (module4.mqtt_has_info()) {
        if(set_komma) tmpstr += ",";
        tmpstr += module4.mqtt_info();
        set_komma = true;
      }
#endif
#ifdef MODULE5
      if (module5.mqtt_has_info()) {
        if(set_komma) tmpstr += ",";
        tmpstr += module5.mqtt_info();
        set_komma = true;
      }
#endif
#ifdef MODULE6
      if (module6.mqtt_has_info()) {
        if(set_komma) tmpstr += ",";
        tmpstr += module6.mqtt_info();
      }
#endif
      tmpstr += "}";
      mqttClient.publish(mk_topic(MQTT_TELEMETRIE,"modinfo"), tmpstr.c_str());
      if (do_log_mqtt) {
        write2log(LOG_MQTT,2, mqtt_topic.c_str(), tmpstr.c_str());
      }
    } else {
      reconnect_mqtt();
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
    write2log(LOG_MQTT,2, topic, cmd);
    ptr = strtok(topic, delimiter);
    if (ptr != NULL) snprintf(part1, TOPIC_PART1_SIZE, "%s", ptr);
    ptr = strtok(NULL, delimiter);
    if (ptr != NULL) snprintf(part2, TOPIC_PART2_SIZE, "%s", ptr);
    ptr = strtok(NULL, delimiter);
    if (ptr != NULL) snprintf(part3, TOPIC_PART3_SIZE, "%s", ptr);
    if ( strncmp(part1, MQTT_COMMAND, sizeof MQTT_COMMAND) == 0 ) {
      if ( strncmp(part2, mqtt_topicP2.c_str(), sizeof mqtt_topicP2.c_str()) == 0 ) {
        prozess_cmd(part3, cmd);
      }
    }
    // Free the memory
    free(cmd);
    // Send new device state
    send_mqtt_dev_state();
  }
}

void mqtt_setup() {
  if ( do_mqtt ) {
    mqttClient.setServer(mqtt_server.c_str(), 1883);
    mqttClient.setCallback(callback_mqtt);
    mqttClient.setBufferSize(512);
    if (do_log_mqtt) {
      write2log(LOG_MQTT,3, "MQTT: Connected to Server:", mqtt_server.c_str(), "Port: 1883");
    }
    send_mqtt_tele();
  }
}

void mqtt_loop(time_t now) {
  if ( do_mqtt ) {
    if (mqttClient.connected()) {
      mqttClient.loop();
      if ( (now - last_mqtt_stat) > STATINTERVAL ) {
        send_mqtt_stat();
        last_mqtt_stat = now;
      }
      if ( (now - last_mqtt_tele) > TELEINTERVAL ) {
        send_mqtt_tele();
        last_mqtt_tele = now;
      }
      if ( 
#ifdef MODULE1
           module1.mqtt_stat_changed()
#else
           false
#endif           
#ifdef MODULE2
           || module2.mqtt_stat_changed()
#endif           
#ifdef MODULE3
           || module3.mqtt_stat_changed()
#endif           
#ifdef MODULE4
           || module4.mqtt_stat_changed()
#endif           
#ifdef MODULE5
           || module5.mqtt_stat_changed()
#endif           
#ifdef MODULE6
           || module6.mqtt_stat_changed()
#endif                     
               ) {
        send_mqtt_stat();
      }
    } else {
      reconnect_mqtt();
    }
  }
  delay(0);
}
