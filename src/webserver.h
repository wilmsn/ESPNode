#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "common.h"
#include "secrets.h"


void setup_webserver();

void prozess_sysinfo();

void handleWebSocketInit(void *arg, uint8_t *data, size_t len);

/// @brief Der Handler für Websocket Messages
/// @param arg 
/// @param data 
/// @param len 
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

/// @brief Eventhandler für die Websockets
/// @param server Keine direkte Verwendung
/// @param client Keine direkte Verwendung
/// @param type Der Eventtyp
/// @param arg Wird durchgereicht an handleWebSocketMessage
/// @param data Wird durchgereicht an handleWebSocketMessage
/// @param len Wird durchgereicht an handleWebSocketMessage
void ws_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                void *arg, uint8_t *data, size_t len);

/// @brief Inittialisierung des WebSockets
void initWebSocket();

/// @brief Startet einen Scan nach verfügbaren Wifi Netzwerken
void prozess_wifiscan(void);

/// @brief Erzeugt ein JSON zur Anzeige der gefundenen Netzwerke
void prozess_wifishow(void);

/// @brief Verarbeitung der Webserveraufrufe "/cmd?[cmd]=[value]"
/// @param request Der übergebene Datensatz
void mk_cmd(AsyncWebServerRequest *request);

#endif
