#pragma once

struct ConVar {
    char *name;
    char *string;
    float value;
    struct ConVar *next;
};

void parse_command(char *command_str);
void cvar_register(struct ConVar *new_cvar);
struct ConVar * cvar_find(char *name);
char * cvar_get(char *name);
float cvar_get_value(char *name);
void cvar_set(char *name, char *string);
