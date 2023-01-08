#include "input.h"
#include <SDL2/SDL.h>

typedef void (*InputHandle)(int);

enum PlayerCommandName {
    WALK_F,
    WALK_B,
    WALK_L,
    WALK_R,
    NUM_PLAYERCOMMANDS
};

struct PlayerCommand {
    enum PlayerCommandName name;
    void (*handle)(int);
};

static struct PlayerCommand scancode_command_table[SDL_NUM_SCANCODES];

void
bind_key(unsigned int scancode, enum PlayerCommandName name, InputHandle handle)
{
    scancode_command_table[scancode].name = name;
    scancode_command_table[scancode].handle = handle;
}


void wf(int down) {
    printf("F %d\n", down);
}

void wb(int down) {
    printf("B %d\n", down);
}

void wl(int down) {
    printf("L %d\n", down);
}

void wr(int down) {
    printf("R %d\n", down);
}

void
set_debug_binds(void) {
    bind_key(SDL_SCANCODE_W, WALK_F, wf);
    bind_key(SDL_SCANCODE_A, WALK_L, wl);
    bind_key(SDL_SCANCODE_S, WALK_B, wb);
    bind_key(SDL_SCANCODE_D, WALK_R, wr);
}

void
handleKey(unsigned int scancode, int down)
{
    if (scancode_command_table[scancode].handle)
        scancode_command_table[scancode].handle(down);
}
