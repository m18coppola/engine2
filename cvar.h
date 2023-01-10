#pragma once

enum ConVarType {
    VALUE,
    FUNC
};

typedef char** ArgList;

struct ConVar {
    enum ConVarType type;
    char *name;
    char *string;
    float value;
    void (*function)(char **);
    struct ConVar *next;
    struct CallBackList *callbacks;
};

/**
 * \brief Execute a string as a console command.
 *
 * This function takes a string, and parses it as a console command.
 * commands executed through this function are NOT journaled by the
 * event queue.
 *
 * @param [in] command_str The string containing the command to be executed
 */
void cvar_exec(char *command_str);
void cvar_register(struct ConVar *new_cvar);
void cvar_register_callback(char *name, void (*callback)(void));
struct ConVar * cvar_find(char *name);
char * cvar_get(char *name);
float cvar_get_value(char *name);
void cvar_set(char *name, char *string);
void cvar_set_value(char *name, float value);
void free_ArgList(ArgList arg_list);
void cvar_init(void);
