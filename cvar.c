#include "cvar.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>

#define MAX_CMDS 64
#define MAX_VARS 64
#define FNV_OFFSET 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3

struct ConVar {
    unsigned int hash;
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
    unsigned int hash = cvar_hash(name);
    int index = hash % MAX_VARS;
    while (cvar_table[index].hash != hash && cvar_table[index].hash != 0) {
        index++;
    }
    if (cvar_table[index].hash == 0) {
        return NULL;
    }
    return cvar_table[index].value;
}

void
cvar_set_value(char *name, char *value)
{
    unsigned int hash = cvar_hash(name);
    int index = hash % MAX_VARS;
    while (cvar_table[index].hash != hash && cvar_table[index].hash != 0) {
        index++;
    }
    cvar_table[index].hash = hash;
    strcpy(cvar_table[index].value, value);
}
