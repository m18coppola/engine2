#include <SDL2/SDL.h>
#include "logger.h"

static SDL_RWops *logfile;

char LogTypeNames[4][6] = {
    "EVENT",
    "MESG ",
    "ERROR",
    "*****"
};

int
init_logger(void)
{
    logfile = SDL_RWFromFile("log.txt", "w+");
    if (!logfile) {
        fprintf(stderr, "RUNTIME: Failed to open logfile!\n");
        return -1;
    }
    return 0;
}

void
log_append(enum LogType type, char *mesg)
{
    char entry[128];
    sprintf(entry, "[%s] %s\n", LogTypeNames[type], mesg);
    SDL_RWwrite(logfile, entry, sizeof(char), strlen(entry));
    entry[strlen(entry)-1] = '\0';
    puts(entry);
}
