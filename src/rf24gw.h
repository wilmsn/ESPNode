#ifndef _RF24GW_H_
#define _RF24GW_H_

#include "common.h"
#include <RF24.h>

/// @brief Der Payload vom Typ payload_t
payload_t payload;
/// @brief Funkkennung Node in Richtung Hub
uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
/// @brief Funkkennung Hub in Richtung Node
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
/// @brief Ein Objekt zur Verwaltung des RF24 Interfaces
/// @param  Radio CE Pin
/// @param  Radio CSN Pin
RF24 radio(RF24_RADIO_CE_PIN, RF24_RADIO_CSN_PIN);
bool         do_rf24gw;
String       rf24gw_hub_server;
uint16_t     rf24gw_hub_port;
uint16_t     rf24gw_gw_port;
uint8_t      rf24gw_gw_no;
bool         do_log_rf24;
/// @brief Eine Variable zur Aufnahme der UDP Daten Struktur
udpdata_t udpdata;
/// @brief Ein Objekt für udp Daten
WiFiUDP udp;


#endif