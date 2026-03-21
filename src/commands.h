#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "common.h"

/// @brief Ein String zur Aufnahme von Statusmeldungen für die Webconsole
String stat_str;

/// @brief Eine laufende Nummer für die JSON Schlüssel
unsigned int stat_no;

/// @brief Erweitert den übergebenen String um den nächsten "stat_XX" Schlüssel
/// @param mystr Der übergebene String zur Aufnahme der Inhalte
void json_stat_header(String& mystr);

/// @brief Ein Schalter der auf "true" gesetzt wird falls ein "set" Befehl abgearbeitet wurde.
bool cmd_valid = false;

/// @brief Der Kommandoprozessor
/// @param cmd Das Kommando
/// @param value Der Wert des Kommandos
bool prozess_cmd(const String cmd, const String value);

/// @brief Überträgt ein JSON mit den Settings an die Wboberfläche.
void show_settings();


#endif