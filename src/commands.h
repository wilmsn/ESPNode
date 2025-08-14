#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "common.h"


bool cmd_valid = false;

void prozess_cmd(const String cmd, const String value);

void show_settings();

void bootMessage(uint8_t txtcolor, const char* myMsg, bool newline);



#endif