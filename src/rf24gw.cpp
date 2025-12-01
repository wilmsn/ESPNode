#include "config.h"
#ifdef RF24GW

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


// 
void writeRf242log(const char* senddir, payload_t pl) {
  String tmp_str;
  tmp_str =  String(senddir);
  tmp_str += String(" O:");
  tmp_str += String(pl.orderno);
  tmp_str += String(" N:");
  tmp_str += String(pl.node_id);
  tmp_str += String(" M:");
  tmp_str += String(pl.msg_id);
  tmp_str += String(" MT:");
  tmp_str += String(pl.msg_type);
  tmp_str += String(" HB:");
  tmp_str += String(pl.heartbeatno);
  write2log(LOG_RF24, 1, tmp_str.c_str());
}

//
void rf24gw_setup() {
  // init rf24
  radio.begin();
  delay(100);
  radio.setChannel(RF24_CHANNEL);
  radio.setDataRate(RF24_SPEED);
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(0, 0);
  radio.setAutoAck(false);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(32);
  radio.setCRCLength(RF24_CRC_16);
  radio.openWritingPipe(rf24_hub2node);
  radio.openReadingPipe(1, rf24_node2hub);
  radio.startListening();
  write2log(LOG_CRITICAL,2, "RF24GW: GW number is:", String(rf24gw_gw_no).c_str() );
  if ( rf24_udp.begin(rf24gw_gw_port) ) {
    if (do_log_rf24) {
      write2log(LOG_CRITICAL,2, "RF24: Opened UDP Port:", String(rf24gw_gw_port).c_str() );
    }
  } else {
    write2log(LOG_CRITICAL,2, "ERROR: Failed to open UDP Port:", String(rf24gw_gw_port).c_str() );
  }
  rf24_payload_size = sizeof(rf24_payload);
  rf24_udpdata_size = sizeof(rf24_udpdata);
}

void rf24gw_loop() {
  if ( radio.available() ) {
    radio.read(&rf24_payload, rf24_payload_size);
    if ( rf24_payload.node_id ) {
      rf24_udpdata.gw_no = rf24gw_gw_no;
      rf24_udpdata.utime = time(0);
      //if (do_log_rf24) 
      writeRf242log("N>G", rf24_payload);
      memcpy(&rf24_udpdata.payload, &rf24_payload, rf24_payload_size);
      rf24_udp.beginPacket(rf24gw_hub_server.c_str(), rf24gw_hub_port);
      rf24_udp.write((const unsigned char *)&rf24_udpdata, rf24_udpdata_size);
      rf24_udp.endPacket();
    }
  }
  int rf24_pack_size = rf24_udp.parsePacket();
  if ( rf24_pack_size ) {
    rf24_udp.read((char*)&rf24_udpdata, rf24_udpdata_size);
    memcpy(&rf24_payload, &rf24_udpdata.payload, rf24_payload_size);
    if (do_log_rf24) writeRf242log("G>N", rf24_payload);
    radio.stopListening();
    radio.write(&rf24_payload, rf24_payload_size);
    radio.startListening();
  }
}

#endif