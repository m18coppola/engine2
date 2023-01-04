#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "logger.h"
#include "window.h"
#include "event.h"

#define BUFFER_SIZE 128
#define MAX_EVENTS 1024
#define TARGET_FPS 60


static SDL_Thread *cli_thread;
static SDL_sem *command_ready;
static SDL_sem *command_processed;
static char cli_buffer[BUFFER_SIZE];
static int tick = 0;

int
cli_loop(void *arg)
{
    char c;
    int i;

    i = 0;
    while ((c = getchar()) != EOF) {
        switch (c) {
            case '\n': /* submit */
                cli_buffer[i%BUFFER_SIZE] = '\0';
                SDL_SemPost(command_ready);
                SDL_SemWait(command_processed);
                i = 0;
                break;
            default:
                cli_buffer[i++%BUFFER_SIZE] = c;
        }
    }
    return 0;
}

void
cli_init(void)
{
    setbuf(stdout, NULL); /* turn off stdout buffering to smooth thread printing */
    command_ready = SDL_CreateSemaphore(0);
    command_processed = SDL_CreateSemaphore(0);
	cli_thread = SDL_CreateThread((SDL_ThreadFunction)cli_loop, "CLI", NULL);
}

void
collect_events(int tick)
{
    //MOUSE
    //CONSOLE CHAR (NOP?)
    //NET
    
    /*
     * CONSOLE PARSE
     * dump asynchronus console buffer, push onto evtq
     * one cmd per tick (may have to extend...)
     * maybe dump init-exec cvars onto evtq manually?
     */
    if (SDL_SemTryWait(command_ready) != SDL_MUTEX_TIMEDOUT) {
        int data_len = strlen(cli_buffer) + 1;
        char *data = malloc(sizeof(char) * data_len);
        strncpy(data, cli_buffer, data_len);
        eventqueue_post(tick, CONSOLE, 0, 0, data_len, data);
        SDL_SemPost(command_processed);
    }


	SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                eventqueue_post(tick
                        ,KEY
                        ,e.key.keysym.sym
                        ,(e.type == SDL_KEYDOWN) ? 1 : 0
                        ,0
                        ,NULL);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                eventqueue_post(tick
                        ,KEY
                        ,e.button.button
                        ,(e.button.type == SDL_MOUSEBUTTONDOWN) ? 1 : 0
                        ,0
                        ,NULL);
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
}

int
main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    cli_init();
    init_logger();
    init_window(300, 400);
    unsigned int start_time = SDL_GetTicks();
    unsigned int elapsed_time;
    unsigned int timeout;
    int start_tick = 0;
    int elapsed_ticks;
    int target_fps = 144;

    while (1) {
        timeout = SDL_GetTicks() + (1.0 / (float)target_fps) *1000.0;
        collect_events(tick);
        process_events();
        clear_window();
        while(!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)); // spin quick frames
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
