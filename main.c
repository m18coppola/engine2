#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "logger.h"
#include "window.h"
#include "event.h"
#include "console.h"
#include "input.h"
#include "devcon.h"

#define MAX_EVENTS 1024

static int tick = 0;

void
load_config(char *filename)
{
    char *config_string = SDL_LoadFile(filename, NULL);
    char *line;
    line = strtok(config_string, "\n");
    do {
        parse_command(line);
    } while ((line = strtok(NULL, "\n")));
    SDL_free(config_string);
}

int
main(int argc, char **argv)
{
#ifdef SCRATCH
    printf("SDL_NUM_SCANCODES = %d\n", SDL_NUM_SCANCODES);
    return 0;
#endif
    init_logger();
    load_config("init.cfg");
    cli_init();
    SDL_Init(SDL_INIT_VIDEO);
    init_window(atoi(cvar_get_value("width")), atoi(cvar_get_value("height")));
    set_debug_binds();
    //init_gl_test(atoi(cvar_get_value("width")), atoi(cvar_get_value("height")));

    unsigned int start_time = SDL_GetTicks();
    unsigned int elapsed_time;
    unsigned int timeout;
    int start_tick = 0;
    int elapsed_ticks;
    int target_fps = atoi(cvar_get_value("target_fps"));
    while (1) {
        timeout = SDL_GetTicks();
        cli_buffer_event(tick);
        input_buffer_events(tick);
        process_events();

        /*
        if (client) {
            parse_gamestate_packet();
            events_to_cmd_packet();
            send_msg();
        } else {
            parse_cmd_packets();
            game_tick();
            broadcast_state();
        }
        */

        clear_window();
        //render_gl_test();
        SDL_Delay(timeout + ((1.0/(float)target_fps) * 1000) - SDL_GetTicks());
        tick++;

        // determine frame performance
        if (tick%(target_fps * 10) == 0) {
            elapsed_time = SDL_GetTicks() - start_time;
            elapsed_ticks = tick - start_tick;
            float fps;
            fps = (float)elapsed_ticks / (float) elapsed_time;
            fps *= 1000.0;
            printf("fps: %f\n", fps);
            start_time = SDL_GetTicks();
            start_tick = tick;
        }
    }

    return 0;
}
