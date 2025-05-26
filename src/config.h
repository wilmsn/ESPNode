#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Arduino.h>

/**
 * \brief
 * In der Datei "config.h" wird:
 * 1) der zu erzeugende Node ausgewählt
 * 2) grundlegende Konfigurationen festgelegt 
*/
//#if ESP_IDF_VERSION_MAJOR == 5
// Hier wird der zu erzeugende Node aktiviert
// Achtung: Es darf nur ein Node ausgewählt werden!
//#define NODESIMPLE
//#define NODE18B20
//#define NODEBOSCH
//#define WITTYNODE
//#define NODE_AUDIO
//#define NODE_TTGO_T_DISPLAY

// meine produktiven Nodes
//#define NODE_WOHNZIMMER
#define NODE_TERASSE
//#define NODE_TEICH
//#define NODE_FLUR
//#define NODE_KUECHENRADIO
//#define NODE_WOHNZIMMERRADIO
//---------------------------
// Ab hier werden Abhängigkeiten gesetzt. 
// Die folgende Einstellungen müssen an die aktuelle Umgebung angepasst werden.
//---------------------------
// Change below to your requirements and infrastructure.
//---------------------------
#define SWVERSION   "0.999 beta3"
// Network Time Protocoll
#define NTP_SERVER  "de.pool.ntp.org"
#define TZ_INFO     "CET-1CEST,M3.5.0/03,M10.5.0/03"
//#define TZ_INFO     "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"
//#define NTP_SERVER  "th.pool.ntp.org"
//#define TZ_INFO     "WIB-7"
// enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)

// MQTT Default Configuration
#define DEFAULT_MQTT_SERVER                    "rpi1.fritz.box"

// RF24 Default Configuration
#define DEFAULT_RF24GW_HUB_SERVER              "rpi1.fritz.box"
#define DEFAULT_RF24GW_HUB_UDP_PORTNO          7004
#define DEFAULT_RF24GW_GW_UDP_PORTNO           7003

//---------------------------
// Ab hier werden Abhängigkeiten gesetzt. Normalerweise sollte ab hier nichts editiert werden, es sei denn....
// man weiss was man tut !!!!
//---------------------------
// No changes below - exept you know what are you doing !!!
//---------------------------

// Globale Einstellungen für Sensoren, können in den Settings für jeden Node überschrieben werden. 
#ifndef REFRESHTIME
/**
 * Die globale Wiederholdauer für Messungen in Sekunden 
 */
#define REFRESHTIME          300
#endif

#ifndef MEASUREDELAY
/**
 * Die globale Wartezeit nach dem Start einer Messung in Sekunden 
 */
#define MEASUREDELAY         10
#endif

#ifndef RESOLUTION_18B20
/**
 * Globale Festlegung der Messauflösung für den 18B20 Sensor
 */
#define RESOLUTION_18B20     12
#endif

#ifndef ONEWIREBUS
/**
 * Settings for one wire bus
 */
#define ONEWIREBUS           4
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S3
#ifndef ESP32
#define ESP32
#endif
#endif
#ifdef CONFIG_IDF_TARGET_ESP32
#ifndef ESP32
#define ESP32
#endif
#endif

#include "Node_settings.h"

// Hier werden die allgemeinen Parameter für den Einsatz festgelegt
// Alternativ können diese Einstellungen auch in der Konfiguration des Nodes 
// festgelegt werden.
// Achtung: Die Festlegung hier kann für jeden Node überschrieben werden!

#ifndef MAGICNO
/// @brief Die Magic Number
/// Eine Null (0) bewirkt das die Einstellungen nicht gespeichert werden, 
///               Beim Neustart werden wieder die Defaults genutzt.
/// Jede andere Zahl bewirkt das die Defaults einmalig übernommen werden wenn sich die NUmmer geändert hat.
#define MAGICNO               0
#endif

/// Modules
#ifdef MODULE1_DEFINITION
#define MODULE1
#endif
#ifdef MODULE2_DEFINITION
#define MODULE2
#endif
#ifdef MODULE3_DEFINITION
#define MODULE3
#endif
#ifdef MODULE4_DEFINITION
#define MODULE4
#endif
#ifdef MODULE5_DEFINITION
#define MODULE5
#endif
#ifdef MODULE6_DEFINITION
#define MODULE6
#endif

/// Abhängigkeiten für das Debugging
/// Debug Settings
#ifdef DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL
#endif
#ifdef DEBUG_SERIAL_RF24
#define DEBUG_SERIAL
#endif
#ifdef DEBUG_SERIAL_WEB
#define DEBUG_SERIAL
#endif
#ifdef DEBUG_SERIAL_MQTT
#define DEBUG_SERIAL
#endif
#ifdef DEBUG_SERIAL_MODULE
#define DEBUG_SERIAL
#endif

/// Festlegung des MQTT Defaultservers
#ifndef MQTT_SERVER
#define MQTT_SERVER                    DEFAULT_MQTT_SERVER
#endif
#ifdef MQTT_CLIENT
#define MQTT                          true
#ifndef MQTT_TOPICP2
#define MQTT_TOPICP2                  MQTT_CLIENT
#endif
#endif

/// Festlegung der RF24 Defaulteinstellungen
#ifndef RF24GW_HUB_SERVER
#define RF24GW_HUB_SERVER              DEFAULT_RF24GW_HUB_SERVER
#endif
#ifndef RF24GW_HUB_UDP_PORTNO
#define RF24GW_HUB_UDP_PORTNO          DEFAULT_RF24GW_HUB_UDP_PORTNO
#endif
#ifndef RF24GW_GW_UDP_PORTNO
#define RF24GW_GW_UDP_PORTNO           DEFAULT_RF24GW_GW_UDP_PORTNO
#endif
#ifdef RF24GW_NO
#define RF24GW                         true
#endif

//Set a default hostname
#ifndef HOSTNAME
#define HOSTNAME                       "ESPNode"
#endif

//Settings for Logging
#ifndef DO_LOG_WEB
#define DO_LOG_WEB                      false
#endif
#ifndef DO_LOG_MODULE
#define DO_LOG_MODULE                   false
#endif
#ifndef DO_LOG_SYSTEM
#define DO_LOG_SYSTEM                   false
#endif
#ifndef DO_LOG_MQTT
#define DO_LOG_MQTT                     false
#endif
#ifndef DO_LOG_RF24
#define DO_LOG_RF24                     false
#endif
#ifndef DO_LOG_CRITICAL
#define DO_LOG_CRITICAL                 false
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
#ifndef STATINTERVAL
#define STATINTERVAL                 300
#endif

/// Measuredelay:
/// Definiert den Abstand zwischen dem Start der Messung und dem gesicherten Vorliegen der Ergebnisse in Sekunden.
#ifndef MEASUREDELAY
#define MEASUREDELAY                 10
#endif

/// Telemetrieinterval:
/// Definiert den Abstand (in Sekunden) zwischen 2 Telemetrieübertragungen.
/// Hierzu gehören: Serverdaten, Netzwerkdaten, ...
#ifndef TELEINTERVAL
#define TELEINTERVAL                 1200
#endif

/// Loop Time Alarm
/// Definiert das Zeitintervall (in Millisekunden) für einen Loop Durchgang das nicht überschritten werden sollte.
/// Bei Überschreitung erfolgt ein Eintrag in die Datei Debugfile.txt (falls aktiviert)
#define LOOP_TIME_ALARM              1000

#define DEBUGFILE                    "/debugfile.txt"

#ifndef RF24_RADIO_CE_PIN
///Anschlusspin für den CE Pin des Funkmodules
#define RF24_RADIO_CE_PIN              15
#endif

#ifndef RF24_RADIO_CSN_PIN
///Anschlusspin für den CSN Pin des Funkmodules
#define RF24_RADIO_CSN_PIN             16
#endif

#ifndef RF24_CHANNEL
///Der verwendete RF24 Funkkanal
#define RF24_CHANNEL                   92
#endif

#ifndef RF24_SPEED
/// Die Übertragungsgeschwindigkeit
#define RF24_SPEED                     RF24_250KBPS
#endif

#ifndef RF24_HUB2NODE
/// Der Netzwerkschlüssel Hub zum Node
#define RF24_HUB2NODE       { 0xf0, 0xcc, 0xfc, 0xcc, 0xcc}
#endif

#ifndef RF24_NODE2HUB
/// Der Netzwerkschlüssel Node zum Hub
#define RF24_NODE2HUB       { 0x33, 0xcc, 0xfc, 0xcc, 0xcc}
#endif

#ifndef NODE_DATTYPE
/// Der Datentyp für die Node_ID. Ist aktuell auf 1...255 festgelegt. Werden mehr Nodes benötigt, kann der Datentyp hier zentral umgestellt werden.
#define NODE_DATTYPE        uint8_t
#endif

#ifndef ONR_DATTYPE
/// Der Datentyp für die Ordernummer. Auch hier eine zentrale Festlegung des Datentyps, der ggf. die Umstellung vereinfacht.
#define ONR_DATTYPE         uint8_t
#endif


/**
 * @brief Die Datenstruktur des payloads in dem RF24 Netzwerk. Sie wird nur benötigt wenn der RF24Gateway genutzt
 * wird und auch dies nur damit Logging-Informationen angezeigt werden können.
 */
typedef struct {
    /**
     * @brief Der eindeutige Identifizierer für einen Node
     * Die Node_ID ist der eindeutige Identifizierer für einen Node.
     * Aktuell können hier die Nodes 1..255 genutzt werden (8 Bit Begrenzung)
     * Damit der Datentyp einfach gewechselt werden kann ist er nur indirekt festgelegt.
     */
    NODE_DATTYPE    node_id;         
    /**
     * @brief Eindeutige Identifizierer einer Nachricht
     * Die MSG_ID ist der eindeutige Identifizierer einer Nachricht.
     * Muss einen Nachricht wiederholt werden, wird sie hochgezählt.
     */
    uint8_t         msg_id;          
    /**
     * @brief Der Nachrichtentyp
     * Art der Nachricht, Definition siehe Nachrichtentyp.
     */
    uint8_t         msg_type;        
    /**
     * @brief Nachrichtenflags
     * Nachrichtenflag, Definition siehe Nachrichtenflags.
     */
    uint8_t         msg_flags;   
    /**
     * @brief Die Ordernummer
     * Ordernummern werden im Hub verwaltet und dort nach jeder Order hochgezählt.
     * Auf eine Anfrage vom Hub wird immer mit der selben Ordernummer geantwortet.
     * Nachrichten, die ihren Ursprung im Node haben ( z.B. Heatbeatmessages )
     * erhalten die Ordernummer "0", Ordernummern größer 250 diesen zur Messung des PA Levels.
     */
    ONR_DATTYPE     orderno;         
    /**
     * @brief Die Heartbeatnummer
     * Die Heartbeatno wird bei jedem neuen Heartbeat hochgezählt
     * Da es sich um eine 8 Bit Zahl handelt wird der gültige Bereich für normale Heartbeats von 1...200 festgelegt
     * Der Bereich 201...255 gilt für besondere Nachrichten (z.B. Initialisierung )
     */
    uint8_t         heartbeatno;      
    /**
     * noch nicht genutzt
     */
    uint8_t         reserved1;      
    /**
     * noch nicht genutzt
     */
    uint8_t         reserved2;      
    /**
     * Datenpaket 1 (32Bit)
     */
    uint32_t        data1;         
    /**
     * Datenpaket 2 (32Bit)
     */
    uint32_t        data2;         
    /**
     * Datenpaket 3 (32Bit)
     */
    uint32_t        data3;         
    /**
     * Datenpaket 4 (32Bit)
     */
    uint32_t        data4;         
    /**
     * Datenpaket 5 (32Bit)
     */
    uint32_t        data5;         
    /**
     * Datenpaket 6 (32Bit)
     */
    uint32_t        data6;         
} payload_t;


/**
 * @brief Die Datenstructur zur Übertragung der Daten zwischen Gateway und Hub
 * Im Prinzig ebtspricht diese Struktur der payload_t Struktur erweitert um ein Feld zur Aufnahme der Gateway_id.
 */
typedef struct {
/**
 * Die eindeutige Gateway ID
 */
  uint16_t          gw_no;         // the number of the sending gateway
/**
 * Die Payloadstruktur wie unter payload_t definiert.
 */
  payload_t         payload;      // the payload to send forward
/**
 * Der Unix Timestamp
 */
  time_t            utime;
} udpdata_t;

// Definition der LogTypen
#ifndef LOG_RF24
/// 
#define  LOG_RF24      0
#endif
#ifndef LOG_SYSTEM
#define  LOG_SYSTEM    1
#endif
#ifndef LOG_MQTT
#define  LOG_MQTT      2
#endif
#ifndef LOG_MODULE
#define  LOG_MODULE    3
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


#endif
