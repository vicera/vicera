/*
 * VICERA by h34ting4ppliance
 * 
 * sdl_gpu.c
 * 
 * SDL Front-end for the Vicera GPU.
 */
#include "sdl_gpu.h"

#include "logging.h"
#include "gpu.h"
#include "cpu.h"
#include "controller.h"

#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define FNAME "sdl_gpu.c"

// Inits SDL GPU struct
void init_sdlgpu(struct SDL_GPUState *state)
{
    state->done     = false;
    state->ready    = false;
}

void start_gpu_rendering(struct GPU *gpu, struct Controller *ctrl, struct SDL_GPUState *state)
{
    // Take the CPU from the GPU
    struct CPU* cpu = gpu->cpu;
    if (!cpu)       // Just do nothing if there is no CPU in the GPU struct
        return;

    // Some useful variable definition
    BYTE x, y;
    BYTE sx, sy, controller;
    unsigned int k;
    // Set it at 0 to avoid unexpected behavior
    controller = 0;

    logging_log(FNAME, "Initializing SDL Front-End...");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        // In case of failure.
        logging_error(FNAME, "SDL Init Error");
        exit(1);
    }

    // Init SDL
    SDL_Event event;
    SDL_Window* win;
    SDL_Renderer* renderer;
    // Create window
    SDL_CreateWindowAndRenderer(TSCREEN_X*2, TSCREEN_Y*2, 0,
                                &win, &renderer);
    SDL_SetWindowTitle(win, "VICERA");
    // Stop running if the Windows couldn't be created
    if ((!win) || (!renderer))
    {
        logging_error(FNAME, "[SDL] Window creation failed.");
        perror("SDL Error");

        exit(1);
    }

    // Ready
    state->ready = true;

    // Main loop
    while (!(state->done))
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Render screen map
        render_screen(gpu);

        // Display on SDL
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        sx = gpu->scroll.x;
        sy = gpu->scroll.y;

        for (x = 0; x < TSCREEN_X; ++x)
            for (y = 0; y < TSCREEN_Y; ++y)
                if (gpu->screen[(x+sx)%0x100][(y+sy)%0x100])
                {
                    SDL_RenderDrawPoint(renderer, x*2, y*2);
                    SDL_RenderDrawPoint(renderer, x*2+1, y*2);
                    SDL_RenderDrawPoint(renderer, x*2, y*2+1);
                    SDL_RenderDrawPoint(renderer, x*2+1, y*2+1);
                }
        
        while (SDL_PollEvent(&event))
        {
            // Detects if you quit the app
            if (event.type == SDL_QUIT)
            {
                state->done = true;
                cpu->running = false;
            // Handles the controller emulation
            } else if (event.type == SDL_KEYDOWN)
            {
                k = event.key.keysym.sym;
                if (k == ctrl->up) controller |= 0x80;
                if (k == ctrl->right) controller |= 0x40;
                if (k == ctrl->down) controller |= 0x20;
                if (k == ctrl->left) controller |= 0x10;
                if (k == ctrl->start) controller |= 0x08;
                if (k == ctrl->select) controller |= 0x04;
                if (k == ctrl->a) controller |= 0x02;
                if (k == ctrl->b) controller |= 0x01;
            } else if (event.type == SDL_KEYUP)
            {
                k = event.key.keysym.sym;
                if (k == ctrl->up) controller &= ~0x80;
                if (k == ctrl->right) controller &= ~0x40;
                if (k == ctrl->down) controller &= ~0x20;
                if (k == ctrl->left) controller &= ~0x10;
                if (k == ctrl->start) controller &= ~0x08;
                if (k == ctrl->select) controller &= ~0x04;
                if (k == ctrl->a) controller &= ~0x02;
                if (k == ctrl->b) controller &= ~0x01;
                
            }
        }
        // Transfer the controller data at &FFF4
        cpu->memory[0xfff4] = controller;

        // R e f r e s h
        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 30);
    }

    state->done = true;    
    // Destroy the window and the SDL Renderer once quitted.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

}
