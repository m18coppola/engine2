#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "lib_datastructure.h"

#define BUFFER_SIZE 128
#define MAX_EVENTS 1024

enum EventType {
    NONE = 0, //timestamp is valid
    KEY, //val1 is keycode, val2 is 1 if down event, 0 if up event
    CHAR, // val1 is a char
    MOUSE, //val1,val2 is x,y respectively
    CONSOLE, //data is *char
    PACKET //TBD
};

struct Event {
    int time;
    enum EventType type;
    int val1, val2;
    int data_size;
    void *data;
};

static SDL_Thread *cli_thread;
static SDL_sem *command_ready;
static SDL_sem *command_processed;
static char cli_buffer[BUFFER_SIZE];
static struct lib_cqueue *event_queue;

void
init_event_system(void)
{
    event_queue = lib_cqueue_init(MAX_EVENTS, sizeof(struct Event));
}

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
    command_ready = SDL_CreateSemaphore(0);
    command_processed = SDL_CreateSemaphore(0);
	cli_thread = SDL_CreateThread((SDL_ThreadFunction)cli_loop, "CLI", NULL);
}

int
main(int argc, char **argv)
{
    setbuf(stdout, NULL); /* turn off stdout buffering to smooth thread printing */
    cli_init();
    while(1) {
        /* cli processing */
        if (SDL_SemTryWait(command_ready) != SDL_MUTEX_TIMEDOUT) {
            /* process command */
            struct Event *new_ev = lib_cqueue_enqueue(event_queue);
            new_ev->type = CONSOLE;
            new_ev->data_size = strlen(cli_buffer) + 1;
            new_ev->data = malloc(new_ev->data_size);
            strncpy((char *)new_ev->data, cli_buffer, new_ev->data_size);

            
            printf("main: %s\n", cli_buffer);
            SDL_SemPost(command_processed);
        }
    }

    return 0;
}
