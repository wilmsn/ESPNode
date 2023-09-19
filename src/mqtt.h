#ifndef _MQTT_H_
#define _MQTT_H_

#include "common.h"


/// @brief Ein Objekt für den MQTT Wifi Client
WiFiClient mqtt_wifi_client;
/// @brief Ein Objekt als MQTT Client
/// @param   Der Wifi Client
PubSubClient mqttClient(mqtt_wifi_client);
/// @brief In diesem String wird der Topic der aktuellen MQTT Message gespeichert
String mqtt_topic;
/// @brief Ein String zur Aufnahme der Informationen die als JSON übertragen werden 
String mqtt_json;
/// @brief Variable zur Zwischenspeicherung der aktuellen JSON Länge
unsigned int mqtt_json_length_old;
/// @brief Ein Flag zur Triggerung der MQTT Status Daten
bool do_send_mqtt_stat = false;
/// @brief Ein Flag zur Triggerung der MQTT Telemetrie Daten
bool do_send_mqtt_tele = false;


bool do_mqtt;
String mqtt_server;
String mqtt_client;
String mqtt_topicP2;
bool do_log_mqtt;


#endif