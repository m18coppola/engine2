#include "cvar.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_CMDS 64
#define MAX_VARS 64
#define FNV_OFFSET 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3

struct ConVar {
    unsigned int hash;
    char name[32];
    char value[128];
};

static struct ConVar cvar_table[MAX_VARS] = {0};

unsigned int
cvar_hash(char *str)
{
    long hash = FNV_OFFSET;
    while (*str != '\0') {
        hash *= FNV_PRIME;
        hash ^= *(str++);
    }
    return hash;
}

char*
cvar_get_value(char *name)
{
    struct ConVar *cvar;

    for (cvar = &cvar_table[cvar_hash(name) % MAX_VARS];
            strcmp(name, cvar->name) && cvar->hash != 0;
            cvar++)
        ;
    return cvar->value;
}

void
cvar_set_value(char *name, char *value)
{
    struct ConVar *cvar;
    for (cvar = &cvar_table[cvar_hash(name) % MAX_VARS];
            strcmp(name, cvar->name) && cvar->hash != 0; cvar++)
        ;
    strcpy(cvar->value, value);
}

void
parse_command(char *command_str)
{
    char token_buffer[128];
    char *token_ptr;
    char *value_dest;

    token_ptr = token_buffer;
    while ((*token_ptr++ = *command_str++) && *command_str != ' ')
        ;
    *token_ptr = '\0';
    value_dest = cvar_get_value(token_buffer);
    if (!*command_str) {
        log_append(LOG_MESG, value_dest);
    } else {
        strcpy(value_dest, ++command_str);
    }
}
