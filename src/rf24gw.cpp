#include "rf24gw.h"

// 
void writeRf242log(const char* senddir, payload_t pl) {
  tmp_str = senddir;
  tmp_str += " O:";
  tmp_str += String(pl.orderno);
  tmp_str += " N:";
  tmp_str += String(pl.node_id);
  tmp_str += " M:";
  tmp_str += String(pl.msg_id);
  tmp_str += " MT:";
  tmp_str += String(pl.msg_type);
  tmp_str += " HB:";
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
  radio.printDetails();
  if (udp.begin(rf24gw_gw_port) == 1) {
    if (do_log_rf24) {
      write2log(LOG_SYS,2, "RF24: Opened UDP Port:", String(rf24gw_hub_port).c_str() );
    }
  }
}

//
void rf24gw_loop() {
  if ( radio.available() ) {
    radio.read(&payload, sizeof(payload));
    udpdata.gw_no = rf24gw_gw_no;
    if (do_log_rf24) writeRf242log("N>G", payload);
    memcpy(&udpdata.payload, &payload, sizeof(payload));
    udp.beginPacket(rf24gw_hub_server.c_str(), rf24gw_hub_port);
    udp.write((const unsigned char*)&udpdata, sizeof(udpdata));
    udp.endPacket();
  }
  if (udp.parsePacket() > 0 ) {
    udp.read((char*)&udpdata, sizeof(udpdata));
    memcpy(&payload, &udpdata.payload, sizeof(payload));
    if (do_log_rf24) writeRf242log("G>N", payload);
    radio.stopListening();
    radio.write(&payload, sizeof(payload));
    radio.startListening();
  }
}

