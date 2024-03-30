#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Arduino.h>

/**
 * \brief
 * In der Datei "config.h" wird:
 * 1) der zu erzeugende Node ausgewählt
 * 2) grundlegende Konfigurationen festgelegt 
*/
// Hier wird der zu erzeugende Node aktiviert
// Achtung: Es darf nur ein Node ausgewählt werden!
//#define NODESIMPLE
//#define ESP32SIMPLE
#define NODE_WEBRADIO
//#define NODE18B20
//#define NODEBOSCH
//#define NODESLIDER
//#define NODEMATRIX
//#define NODEGWTEST
//#define WITTYNODE1
//#define NODE_GASZAEHLER
// meine produktiven Nodes
//#define NODE_WOHNZIMMER
//#define NODE_TERASSE
//#define NODE_TEICH
//#define NODE_FLUR
//---------------------------

#include "Node_settings.h"

#define SWVERSION   "0.998radio"
#define NTP_SERVER  "de.pool.ntp.org"
#define TZ_INFO     "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"
// enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)

// Hier werden die allgemeinen Parameter für den Einsatz festgelegt
// Alternativ können diese Einstellungen auch in der Konfiguration des Nodes 
// festgelegt werden.
// Achtung: Die Festlegung hier kann für jeden Node überschrieben werden!
#ifndef MQTT_SERVER
#define MQTT_SERVER                    "rpi1.fritz.box"
#endif
#ifndef RF24GW_HUB_SERVER
#define RF24GW_HUB_SERVER              "rpi1.fritz.box"
#endif
#ifndef RF24GW_HUB_UDP_PORTNO
#define RF24GW_HUB_UDP_PORTNO          7004
#endif
#ifndef RF24GW_GW_UDP_PORTNO
#define RF24GW_GW_UDP_PORTNO           7003
#endif

//Settings for I2S
//define I2S_DOUT     25
//#define I2S_BCLK     27
//#define I2S_LRC      26

// Settings for Webradio-Stations definitions
#define MAXSTATION            10
#define STATION_NAME_LENGTH   30
#define STATION_URL_LENGTH    128


//Settings for Logging
#ifndef LOG_WEB
#define LOG_WEB                      false
#endif
#ifndef LOG_SENSOR
#define LOG_SENSOR                   false
#endif
#ifndef LOG_SYS
#define LOG_SYS                      false
#endif
#ifndef LOG_MQTT
#define LOG_MQTT                     false
#endif
#ifndef LOG_RF24
#define LOG_RF24                     false
#endif
#ifndef LOG_CRITICAL
#define LOG_CRITICAL                 false
#endif

///@brief Settings for mqtt topic
///Der Part1 des MQTT Topics nimmt nur einen der 3 vordefinierten Werte auf. Deshalb sind hier 5 Zeichen hinreichend.
#define TOPIC_PART1_SIZE             5
#define MQTT_STATUS                  "stat"
#define MQTT_TELEMETRIE              "tele"
#define MQTT_COMMAND                 "cmnd"

#define TOPIC_PART2_SIZE             30
#define TOPIC_PART3_SIZE             10
//Settings for QoS
#define MQTT_QOS                     0
#define MQTT_WILLRETAIN              true

/// Statusinterval:
/// Definiert den Abstand (in Sekunden) zwischen 2 Messungen mit anschliessendem Versand der Daten über MQTT (falls aktiviert).
/// In diesem Zeitinterval werden auch die Schalterzustände übertragen.
#define STATINTERVAL                 300

/// Messinterval:
/// Definiert den Abstand zwischen dem Start der Messung und dem gesicherten Vorliegen der Ergebnisse in Sekunden.
#define MESSINTERVAL                 1

/// Telemetrieinterval:
/// Definiert den Abstand (in Sekunden) zwischen 2 Telemetrieübertragungen.
/// Hierzu gehören: Serverdaten, Netzwerkdaten, ...
#define TELEINTERVAL                 1200

/// Loop Time Alarm
/// Definiert das Zeitintervall (in Millisekunden) für einen Loop Durchgang das nicht überschritten werden sollte.
/// Bei Überschreitung erfolgt ein Eintrag in die Datei Debugfile.txt (falls aktiviert)
#define LOOP_TIME_ALARM              1000

#define DEBUGFILE                    "/debugfile.txt"

///Anschlusspin für den CE Pin des Funkmodules
#ifndef RF24_RADIO_CE_PIN
#define RF24_RADIO_CE_PIN              15
#endif
///Anschlusspin für den CSN Pin des Funkmodules
#ifndef RF24_RADIO_CSN_PIN
#define RF24_RADIO_CSN_PIN             16
#endif
///Der verwendete RF24 Funkkanal
#ifndef RF24_CHANNEL
#define RF24_CHANNEL        92
#endif
/// Die Übertragungsgeschwindigkeit
#ifndef RF24_SPEED
#define RF24_SPEED          RF24_250KBPS
#endif
/// Der Netzwerkschlüssel Hub zum Node
#ifndef RF24_HUB2NODE
#define RF24_HUB2NODE       { 0xf0, 0xcc, 0xfc, 0xcc, 0xcc}
#endif
/// Der Netzwerkschlüssel Node zum Hub
#ifndef RF24_NODE2HUB
#define RF24_NODE2HUB       { 0x33, 0xcc, 0xfc, 0xcc, 0xcc}
#endif
/// Der Datentyp für die Node_ID. Ist aktuell auf 1...255 festgelegt. Werden mehr Nodes benötigt, kann der Datentyp hier zentral umgestellt werden.
#ifndef NODE_DATTYPE
#define NODE_DATTYPE        uint8_t
#endif
/// Der Datentyp für die Ordernummer. Auch hier eine zentrale Festlegung des Datentyps, der ggf. die Umstellung vereinfacht.
#ifndef ONR_DATTYPE
#define ONR_DATTYPE         uint8_t
#endif


/// @brief Die Datenstruktur des payloads in dem RF24 Netzwerk. Sie wird nur benötigt wenn der RF24Gateway genutzt
/// wird und auch dies nur damit LOginformationen angezeigt werden können.
typedef struct {
/// Die Node_ID ist der eindeutige Identifizierer für einen Node.
/// Aktuell können hier die Nodes 1..255 genutzt werden (8 Bit Begrenzung)
/// Damit der Datentyp einfach gewechselt werden kann ist er nur indirekt festgelegt.   
    NODE_DATTYPE    node_id;         
/// Die MSG_ID ist der eindeutige Identifizierer einer Nachricht.
/// Muss einen Nachricht wiederholt werden, wird sie hochgezählt.
    uint8_t         msg_id;          
/// Art der Nachricht, Definition siehe Nachrichtentyp.
    uint8_t         msg_type;        
/// Nachrichtenflag, Definition siehe Nachrichtenflags.
    uint8_t         msg_flags;   
/// Ordernummern werden im Hub verwaltet und dort nach jeder Order hochgezählt.
/// Auf eine Anfrage vom Hub wird immer mit der selben Ordernummer geantwortet.
/// Nachrichten, die ihren Ursprung im Node haben ( z.B. Heatbeatmessages ) 
/// erhalten die Ordernummer "0", Ordernummern größer 250 diesen zur Messung des PA Levels.    
    ONR_DATTYPE     orderno;         
/// Die heartbeatno wird bei jedem neuen Heartbeat hochgezählt
/// Da es sich um eine 8 Bit Zahl handelt wird der gültige Bereich für normale Heartbeats von 1...200 festgelegt
/// Der Bereich 201...255 gilt für besondere Nachrichten (z.B. Initialisierung )
    uint8_t         heartbeatno;      
/// noch nicht genutzt
    uint8_t         reserved1;      
/// noch nicht genutzt
    uint8_t         reserved2;      
/// Datenpaket 1 (32Bit)
    uint32_t        data1;         
/// Datenpaket 2 (32Bit)
    uint32_t        data2;         
/// Datenpaket 3 (32Bit)
    uint32_t        data3;         
/// Datenpaket 4 (32Bit)
    uint32_t        data4;         
/// Datenpaket 5 (32Bit)
    uint32_t        data5;         
/// Datenpaket 6 (32Bit)
    uint32_t        data6;         
} payload_t;


/// @brief Die Datenstructur zur Übertragung der Daten zwischen Gateway und Hub
///Im Prinzig ebtspricht diese Struktur der payload_t Struktur erweitert um ein Feld zur Aufnahme der Gateway_id.

typedef struct {
/// Die eindeutige Gateway ID
  uint16_t          gw_no;         // the number of the sending gateway
/// Die Payloadstruktur wie unter payload_t definiert.
  payload_t         payload;      // the payload to send forward
} udpdata_t;

/// Definition der LogTypen
#ifndef LOG_RF24
#define  LOG_RF24      0
#endif
#ifndef LOG_SYS
#define  LOG_SYS       1
#endif
#ifndef LOG_MQTT
#define  LOG_MQTT      2
#endif
#ifndef LOG_SENSOR
#define  LOG_SENSOR    3
#endif
#ifndef LOG_WEB
#define  LOG_WEB       4
#endif
#ifndef LOG_CRITICAL
#define  LOG_CRITICAL  5
#endif
#ifndef LOG_DAYBREAK
#define  LOG_DAYBREAK  6
#endif

extern void write2log(uint8_t kat, int count, ...);



#endif