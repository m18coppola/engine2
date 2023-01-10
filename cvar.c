#include "cvar.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct ConVar *cvar_list;
static char logger_msg[512];

struct Token {
	char *string;
	struct Token *next;
};

void cvar_set_cmd(ArgList arg_list);

struct ConVar set_cmd = {FUNC, "set", 0, 0, cvar_set_cmd};

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
    if (new_cvar->type == VALUE) {
        string = new_cvar->string;
        new_cvar->string = malloc(sizeof(char) * (strlen(string) + 1));
        strcpy(new_cvar->string, string);
        new_cvar->value = atof(new_cvar->string);
        new_cvar->callbacks = NULL;
    }
    

    new_cvar->next = cvar_list;
    cvar_list = new_cvar;
}

void
cvar_register_callback(char *name, void (*callback)(void))
{
    struct ConVar *cvar;

    if (!(cvar = cvar_find(name))) {
        sprintf(logger_msg, "cvar: %s not found", name);
        log_append(LOG_ERR, logger_msg);
        return;
    }

    if (FUNC == cvar->type) {
        sprintf(logger_msg, "cvar: %s is a command", name);
        log_append(LOG_ERR, logger_msg);
        return;
    }

    struct CallBackList *new_callback = malloc(sizeof(*new_callback));
    new_callback->callback = callback;
    new_callback->next = cvar->callbacks;
    cvar->callbacks = new_callback;
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

    if (VALUE != cvar->type) {
        sprintf(logger_msg, "cvar: %s is a command", name);
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
        sprintf(logger_msg, "cvar: %s not found", name);
        log_append(LOG_ERR, logger_msg);
        return 0.0;
    }

    if (VALUE != cvar->type) {
        sprintf(logger_msg, "cvar: %s is a command", name);
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
        sprintf(logger_msg, "cvar: %s not found", name);
        log_append(LOG_ERR, logger_msg);
        return;
    }

    if (VALUE != cvar->type) {
        sprintf(logger_msg, "cvar: %s is a command", name);
        log_append(LOG_ERR, logger_msg);
        return;
    }

    free(cvar->string);
    cvar->string = malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(cvar->string, string);
    cvar->value = atof(cvar->string);

    /* trigger callbacks */
    struct CallBackList *cbl;
    for (cbl = cvar->callbacks; cbl; cbl = cbl->next) {
        cbl->callback();
    }
}

void
cvar_set_value(char *name, float value)
{
    char value_string[32];
    snprintf(value_string, 32, "%f", value);
    cvar_set(name, value_string);
}

ArgList
tokenize(char *command_str)
{
    ArgList token_list;
    struct Token *head, *ptr;
    int i;
    int in_token = 0;
    int token_count = 0;

    head = malloc(sizeof(*head));
    head->string = command_str;
    head->next = NULL;
    ptr = head;
    
    while (*command_str != '\0') {
        if (*command_str <= ' ') {
            if (in_token) {
                *command_str = '\0';
                in_token = 0;
            }
        } else if (!in_token) {
            token_count++;
            ptr->next = malloc(sizeof(*ptr->next));
            ptr = ptr->next;
            ptr->string = command_str;
            ptr->next = NULL;
            in_token = 1;
        }
        command_str++;
    }
    token_list = malloc(sizeof(*token_list) * (2 + token_count));
    i = 0;
    while(head != NULL) {
        token_list[i] = head->string;
        ptr = head;
        head = head->next;
        free(ptr);
        i++;
    }
    token_list[i] = NULL;

    return token_list;
}

void
cvar_set_cmd(ArgList arg_list)
{
    if (!arg_list[2] || !arg_list[3] || arg_list[4]) {
        printf("usage: set var_name var_value\n");
        return;
    }
    cvar_set(arg_list[2], arg_list[3]);
    cvar_exec(arg_list[2]);
}

void
free_ArgList(ArgList arg_list)
{
    free(*arg_list);
    free(arg_list);
}

void
cvar_exec(char *command_str)
{
    ArgList arg_list;
    char *command_orig;
    struct ConVar *cvar;

    /* make a copy of command so event can free its own */
    command_orig = command_str;
    command_str = malloc(sizeof(char) * (strlen(command_orig) + 1));
    strcpy(command_str, command_orig);

    arg_list = tokenize(command_str);

    if (!arg_list[1]) {
        free_ArgList(arg_list);
        return;
    }
    
    if (!(cvar = cvar_find(arg_list[1]))) {
        sprintf(logger_msg, "cvar: %s not found", arg_list[1]);
        log_append(LOG_ERR, logger_msg);
        return;
    }

    if (!arg_list[2] && VALUE == cvar->type) {
        printf("] %s %s\n", cvar->name, cvar->string);
    } else {
        if (FUNC == cvar->type) {
            cvar->function(arg_list);
        } else {
            printf("%s is a variable. Use \"set %s new_value\"\n", cvar->name, cvar->name);
        }
    }
    free_ArgList(arg_list);
}

void
cvar_init(void)
{
    cvar_register(&set_cmd);
}
