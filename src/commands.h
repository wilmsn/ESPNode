#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "common.h"

String stat_str;

unsigned int stat_no;

void json_stat_header(String& mystr);

bool cmd_valid = false;

void prozess_cmd(const String cmd, const String value);

void show_settings();


#endif