#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "event.h"
#include "logger.h"
#include "cvar.h"
#include "input.h"

#define MAX_EVENTS 1024
#define CLI_BUFFER_SIZE 128

struct Event {
    int time;
    enum EventType type;
    int val1, val2;
    int data_size;
    void *data;
};

static int head = -1;
static int tail = -1;
static struct Event queue[MAX_EVENTS];

static SDL_Thread *cli_thread;
static SDL_sem *command_ready;
static SDL_sem *command_processed;
static char cli_buffer[CLI_BUFFER_SIZE];

int
eventqueue_is_empty(void)
{
    return head == -1;
}

int
eventqueue_is_full(void)
{
    return (head == tail + 1) || (head == 0 && tail == MAX_EVENTS - 1);
}

int
eventqueue_post(int time, enum EventType type, int val1, int val2,
                int data_size, char *data)
{
    if (eventqueue_is_full()) {
        log_append(LOG_ERR, "event queue is full!");
        return -1;
    }
    if (head == -1) head = 0;
    tail = (tail + 1) % MAX_EVENTS;
    queue[tail].time = time;
    queue[tail].type = type;
    queue[tail].val1 = val1;
    queue[tail].val2 = val2;
    queue[tail].data_size = data_size;;
    queue[tail].data = data;
    return 0;
}

struct Event *
eventqueue_poll(void)
{
    struct Event *ev;

    if (eventqueue_is_empty()) {
        return NULL;
    }
    ev = queue+head;
    if (head == tail) {
        head = -1;
        tail = -1;
    } else {
        head = (head + 1) % MAX_EVENTS;
    }
    return ev;
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
process_events(void)
{
    int deffered_exit = 0;
    char msg[512];
    while(!eventqueue_is_empty()) {
        struct Event *ev;
        ev = eventqueue_poll();
        switch (ev->type) {
            case CONSOLE:
                parse_command((char *)ev->data);
                free(ev->data);
                break;
            case KEY:
                handleKey(ev->val1, ev->val2);
                break;
            case MOUSE:
                break;
            default:
                sprintf(msg
                        ,"bad event type: %d"
                        ,ev->type);
                log_append(LOG_ERR, msg);
                break;
        }
    }
    if (deffered_exit) exit(0);
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
                cli_buffer[i%CLI_BUFFER_SIZE] = '\0';
                SDL_SemPost(command_ready);
                SDL_SemWait(command_processed);
                i = 0;
                break;
            default:
                cli_buffer[i++%CLI_BUFFER_SIZE] = c;
        }
    }
    exit(0);
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
        parse_sdl_event(e, tick);
    }
}

