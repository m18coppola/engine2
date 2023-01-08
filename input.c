#include <SDL2/SDL.h>

#include "input.h"
#include "event.h"

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

void
handleKey(unsigned int scancode, int down)
{
    if (scancode_command_table[scancode].handle)
        scancode_command_table[scancode].handle(down);
}


static void
parse_sdl_event(SDL_Event e, int tick)
{
    switch (e.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (!e.key.repeat)
                eventqueue_post(tick
                        ,KEY
                        ,e.key.keysym.scancode
                        ,(e.type == SDL_KEYDOWN) ? 1 : 0
                        ,0
                        ,NULL);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_MOUSEMOTION:
            eventqueue_post(tick
                    ,MOUSE
                    ,e.motion.x
                    ,e.motion.y
                    ,0
                    ,NULL);
            break;
        case SDL_QUIT:
            exit(0);
    }

}

void
input_buffer_events(int tick)
{
	SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        parse_sdl_event(e, tick);
    }
}
