#pragma once
unsigned int cvar_hash(char *str);
void cvar_set_value(char *name, char *value);
char* cvar_get_value(char *name);
void parse_command(char *command_str);
