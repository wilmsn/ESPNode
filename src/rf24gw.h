#ifndef _RF24GW_H_
#define _RF24GW_H_

#include "common.h"
#include <RF24.h>

/// @brief Der Payload vom Typ payload_t
payload_t rf24_payload;

/// @brief Die Größe des Payloads
size_t   rf24_payload_size;

/// @brief Funkkennung Node in Richtung Hub
uint8_t  rf24_node2hub[] = RF24_NODE2HUB;

/// @brief Funkkennung Hub in Richtung Node
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;

/// @brief Ein Objekt zur Verwaltung des RF24 Interfaces
/// @param  Radio CE Pin
/// @param  Radio CSN Pin
RF24 radio(RF24_RADIO_CE_PIN, RF24_RADIO_CSN_PIN);

/// @brief Schalter für den RF24GW
bool         do_rf24gw;

/// @brief String für den Servernamen des RF24 Hubs 
String       rf24gw_hub_server;

/// @brief Der Port des RF24 Hubs
uint16_t     rf24gw_hub_port;

/// @brief Der Port des RF24 Gateways
uint16_t     rf24gw_gw_port;

/// @brief Die eindeutige Nummer dieses Gateways
uint16_t     rf24gw_gw_no;

/// @brief Schalter fürs RF24 Logging
bool         do_log_rf24;


/// @brief Eine Variable zur Aufnahme der UDP Daten Struktur
udpdata_t rf24_udpdata;

/// @brief Die Größe des udp Data arrays
size_t rf24_udpdata_size;

/// @brief Ein Objekt für udp Daten
WiFiUDP rf24_udp;


#endif