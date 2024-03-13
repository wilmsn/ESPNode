#include "mqtt.h"

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

// dokumentiert in main.h
void send_mqtt_stat() {
  if (do_mqtt) {
    if (mqttClient.connected()) {
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
      write2log(LOG_MQTT,2, mqtt_topic.c_str(), mqtt_json.c_str());

      mqtt_json = "{";
      mqtt_json_length_old = mqtt_json.length();
#if defined(SWITCH1)  
      mqttClient.publish(mk_topic(MQTT_STATUS, switch1.show_mqtt_name()), switch1.show_value());
      write2log(LOG_MQTT,2, mqtt_topic.c_str(), switch1.show_value());
      mqtt_json += switch1.mqtt_json_part();
#endif
#if defined(SWITCH2)  
      if ( mqtt_json.length() > mqtt_json_length_old ) {
        mqtt_json += ",";
        mqtt_json_length_old = mqtt_json.length();
      }
      mqttClient.publish(mk_topic(MQTT_STATUS, switch2.show_mqtt_name()), switch2.show_value());
      write2log(LOG_MQTT,2, mqtt_topic.c_str(), switch2.show_value());
      mqtt_json += switch2.mqtt_json_part();
#endif
#if defined(SWITCH3)  
      if ( mqtt_json.length() > mqtt_json_length_old ) {
        mqtt_json += ",";
        mqtt_json_length_old = mqtt_json.length();
      }
      mqttClient.publish(mk_topic(MQTT_STATUS, switch3.show_mqtt_name()), switch3.show_value());
      write2log(LOG_MQTT,2, mqtt_topic.c_str(), switch3.show_value());
      mqtt_json += switch3.mqtt_json_part();
#endif
#if defined(SWITCH4)  
      if ( mqtt_json.length() > mqtt_json_length_old ) {
        mqtt_json += ",";
        mqtt_json_length_old = mqtt_json.length();
      }
      mqttClient.publish(mk_topic(MQTT_STATUS, switch4.show_mqtt_name()), switch4.show_value());
      write2log(LOG_MQTT,2, mqtt_topic.c_str(), switch4.show_value());
      mqtt_json += switch4.mqtt_json_part();
#endif
      mqtt_json += "}";
      mqttClient.publish(mk_topic(MQTT_STATUS, "switchdata"), mqtt_json.c_str());
      write2log(LOG_MQTT,2, mqtt_topic.c_str(), mqtt_json.c_str());
    } else {
      reconnect_mqtt();
    }
  }
}

// dokumentiert in main.h
void send_mqtt_tele() {
  if (do_mqtt) {
#if defined(DEBUG_SERIAL)
    Serial.println("Sending Mqtt Tele");
#endif
    if (mqttClient.connected()) {
      mk_sysinfo1(mqtt_json); // Hier wird die Variable "info_str" gefüllt!
      mqttClient.publish(mk_topic(MQTT_TELEMETRIE,"info1"), mqtt_json.c_str());
      if (do_log_mqtt) {
        write2log(LOG_MQTT,2, mqtt_topic.c_str(), mqtt_json.c_str());
      }
      mk_sysinfo2(mqtt_json);
      mqttClient.publish(mk_topic(MQTT_TELEMETRIE, "info2"), mqtt_json.c_str());
      if (do_log_mqtt) {
        write2log(LOG_MQTT,2, mqtt_topic.c_str(), mqtt_json.c_str());
      }
      mk_sysinfo3(mqtt_json, true);
      mqttClient.publish(mk_topic(MQTT_TELEMETRIE, "info3"), mqtt_json.c_str());
      if (do_log_mqtt) {
        write2log(LOG_MQTT,2, mqtt_topic.c_str(), mqtt_json.c_str());
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

void mqtt_loop() {
  if ( do_mqtt ) {
    if (mqttClient.connected()) {
      mqttClient.loop();
      if ( do_send_mqtt_stat ) {
        send_mqtt_stat();
        do_send_mqtt_stat = false;
      }
      if ( do_send_mqtt_tele ) {
        send_mqtt_tele();
        do_send_mqtt_tele = false;
      }
    } else {
      reconnect_mqtt();
    }
  }
  delay(0);
}
