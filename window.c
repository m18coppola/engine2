#include <SDL2/SDL.h>
#include "logger.h"
#include "window.h"

static SDL_Window *window;
static SDL_Renderer *render;

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

void
clear_window(void)
{
    SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
    SDL_RenderClear(render);
    //SDL_UpdateWindowSurface(window);
    SDL_RenderPresent(render);
}
