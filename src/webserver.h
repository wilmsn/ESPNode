#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "common.h"
#include "secrets.h"

int cmd_no = 0;

/// @brief Create AsyncWebServer object on port 80
AsyncWebServer httpServer(80);
/// @brief Ein Server für die Websockets
AsyncWebSocket ws("/ws");

void setup_webserver();

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
/// @return Ein Json zur Übergabe an die Webseite
const char *mk_wifiscan(void);

/// @brief Erzeugt ein JSON zur Anzeige der gefundenen Netzwerke
/// @return Ein Json zur Übergabe an die Webseite
const char *mk_wifishow(void);

/// @brief Verarbeitung der Webserveraufrufe "/cmd?[cmd]=[value]"
/// @param request Der übergebene Datensatz
/// @return Rückmeldung für die Webseite
const char *mk_cmd(AsyncWebServerRequest *request);

#endif
