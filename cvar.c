#include "cvar.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct ConVar *cvar_list;
static char logger_msg[512];

void
cvar_register(struct ConVar *new_cvar)
{
    char *string;

    if (cvar_find(new_cvar->name)) {
        sprintf(logger_msg, "cvar: %s already in use", new_cvar->name);
        log_append(LOG_ERR, logger_msg);
        return;
    }

    /* cvar manages its own memory */
    string = new_cvar->string;
    new_cvar->string = malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(new_cvar->string, string);
    

    new_cvar->value = atof(new_cvar->string);
    new_cvar->next = cvar_list;
    cvar_list = new_cvar;
}

struct ConVar *
cvar_find(char *name)
{
    struct ConVar **ptr = &cvar_list;
    for(ptr = &cvar_list;
            (*ptr) && strcmp(name, (*ptr)->name);
            ptr = &(*ptr)->next)
        ;
    
    return *ptr;
}

char *
cvar_get(char *name)
{
    struct ConVar *cvar;

    if (!(cvar = cvar_find(name))) {
        sprintf(logger_msg, "cvar: %s not found", name);
        log_append(LOG_ERR, logger_msg);
        return "";
    }

    return cvar->string;
}


float
cvar_get_value(char *name)
{
    struct ConVar *cvar;

    if (!(cvar = cvar_find(name))) {
        sprintf(logger_msg, "cvar: %s not found", cvar->name);
        log_append(LOG_ERR, logger_msg);
        return 0.0;
    }

    return cvar->value;
}

void
cvar_set(char *name, char *string)
{
    struct ConVar *cvar;
    
    if (!(cvar = cvar_find(name))) {
        sprintf(logger_msg, "cvar: %s not found", cvar->name);
        log_append(LOG_ERR, logger_msg);
        return;
    }

    free(cvar->string);
    cvar->string = malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(cvar->string, string);
    cvar->value = atof(cvar->string);
}

void
parse_command(char *command_str)
{
    char token_buffer[128];
    char *token_ptr;

    token_ptr = token_buffer;
    while ((*token_ptr++ = *command_str++) && *command_str != ' ')
        ;
    command_str--;
    *token_ptr = '\0';
    if (!*command_str) {
        printf("] %s %s\n", token_buffer, cvar_get(token_buffer));
    } else {
        cvar_set(token_buffer, ++token_ptr);
    }
}
