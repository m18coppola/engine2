#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "logger.h"
#include "event.h"

#define BUFFER_SIZE 128
#define MAX_EVENTS 1024


static SDL_Thread *cli_thread;
static SDL_sem *command_ready;
static SDL_sem *command_processed;
static char cli_buffer[BUFFER_SIZE];
static SDL_Window *window;
static SDL_Renderer *render;
static int evt_id = 0;

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
collect_events(void)
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
        eventqueue_post(evt_id++, CONSOLE, 0, 0, data_len, data);
        SDL_SemPost(command_processed);
    }


	SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                eventqueue_post(evt_id++
                        ,KEY
                        ,e.key.keysym.sym
                        ,(e.type == SDL_KEYDOWN) ? 1 : 0
                        ,0
                        ,NULL);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                eventqueue_post(evt_id++
                        ,KEY
                        ,e.button.button
                        ,(e.button.type == SDL_MOUSEBUTTONDOWN) ? 1 : 0
                        ,0
                        ,NULL);
                break;
            case SDL_MOUSEMOTION:
                eventqueue_post(evt_id++
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
init_window(int width, int height)
{
    window = SDL_CreateWindow("rinse"
            ,SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED
            ,width, height
            ,SDL_WINDOW_SHOWN);
            //,SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        log_append(LOG_ERR, "Failed to get SDL window");
        return -1;
    }
    render = SDL_CreateRenderer(window, -1, 0);
    return 0;
}

int
main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    cli_init();
    init_logger();
    init_event_system(1024);
    init_window(300, 400);
    while(1) {
        collect_events();
        process_events();
        SDL_RenderClear(render);
    }

    return 0;
}
