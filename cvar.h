#pragma once

enum ConVarType {
    VALUE,
    FUNC
};

typedef char** ArgList;

struct CallBackList {
    void (*callback)(void);
    struct CallBackList *next;
};

struct ConVar {
    enum ConVarType type;
    char *name;
    char *string;
    float value;
    void (*function)(char **);
    struct ConVar *next;
    struct CallBackList *callbacks;
};

void parse_command(char *command_str);
void cvar_register(struct ConVar *new_cvar);
void cvar_register_callback(char *name, void (*callback)(void));
struct ConVar * cvar_find(char *name);
char * cvar_get(char *name);
float cvar_get_value(char *name);
void cvar_set(char *name, char *string);
void cvar_set_value(char *name, float value);
void free_ArgList(ArgList arg_list);
void cvar_exec(char *command_str);
void cvar_init(void);
