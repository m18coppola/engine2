#include <epoxy/gl.h>
#include <epoxy/glx.h>
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
    SDL_RenderPresent(render);
}

void
init_gl_test(int width, int height)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(
            "rinse",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            width, height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);


    SDL_GL_CreateContext(window);
}

void
render_gl_test(void)
{
    glClearColor(0.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
}
