#include <SDL2/SDL.h>
#include "devcon.h"
#include "event.h"

#define CLI_BUFFER_SIZE 128

static SDL_Thread *cli_thread;
static SDL_sem *command_ready;
static SDL_sem *command_processed;
static char cli_buffer[CLI_BUFFER_SIZE];

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
cli_buffer_event(int tick)
{
    if (SDL_SemTryWait(command_ready) != SDL_MUTEX_TIMEDOUT) {
        int data_len = strlen(cli_buffer) + 1;
        char *data = malloc(sizeof(char) * data_len);
        strncpy(data, cli_buffer, data_len);
        eventqueue_post(tick, CONSOLE, 0, 0, data_len, data);
        SDL_SemPost(command_processed);
    }
}
