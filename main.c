/*
 * VICERA by h34ting4ppliance
 *
 * VICERA is a fantasy console featuring
 * a monochrome 160x160 display and a simple
 * instruction set.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#include "cpu.h"
#include "gpu.h"
#include "logging.h"

#define FNAME "main.c"
struct CPU console;
struct GPU console_gpu;

bool done;

void* gpu_rendering(void *none)
{
    int x, y;

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
    SDL_CreateWindowAndRenderer(SCREEN_X, SCREEN_Y, 0,
                                &win, &renderer);

    while (!done)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Render screen map
        render_screen(&console_gpu);

        // Display on SDL
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        for (x = 0; x < SCREEN_X; ++x)
            for (y = 0; y < SCREEN_Y; ++y)
                if (console_gpu.screen[x][y])
                    SDL_RenderDrawPoint(renderer, x, y);

        if (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                done = true;

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 30);
    }

    pthread_exit(NULL);
}

void print_usage()
{
    logging_error(FNAME, "Usage: vicera ROMFILE");
    exit(1);
}

int main(int argc, char **argv)
{
    logging_log(FNAME, "-- -- -- -- -- -- -- -- -- --");
    logging_log(FNAME, " Vicera by h34ting4ppliance!");
    logging_log(FNAME, "          have fun!");
    logging_log(FNAME, "-- -- -- -- -- -- -- -- -- --");
    // Running program
    
    if (argc != 2)
        print_usage();
    
    FILE *rom = fopen(argv[1], "r");
    if (!rom)
    {
        perror("Unable to open ROM: %s");
        exit(1);
    }

    int c;
    done = false;

    init_cpu(&console);
    init_gpu(&console_gpu, &console);

    console.pc = 0x0000;
    for (int i = 0; (c=fgetc(rom)) != EOF; ++i)
        console.memory[i] = c;
    
    pthread_t ct;
    int rc = pthread_create(&ct, NULL, gpu_rendering, NULL);
    run(&console);
    done = true;

    return 0;
}
