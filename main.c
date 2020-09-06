/*
 * VICERA by h34ting4ppliance
 *
 * main.c
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
#include "controller.h"
#include "logging.h"

#define TSCREEN_X 160
#define TSCREEN_Y 160

#define FNAME "main.c"

struct CPU console;
struct GPU console_gpu;
struct Controller console_ctrl;

bool done;

void* gpu_rendering(void *none)
{
    // Some useful variable definition
    BYTE x, y;
    BYTE sx, sy, controller;
    unsigned int k;
    // Set it at 0 to avoid unexpected behavior
    controller = 0;

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
    // Stop running if the Windows couldn't be created
    if ((!win) || (!renderer))
    {
        logging_error(FNAME, "[SDL] Window creation failed.");
        perror("SDL Error");

        exit(1);
    }   

    // Main loop
    while (!done)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Render screen map
        render_screen(&console_gpu);

        // Display on SDL
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        sx = console_gpu.scroll.x;
        sy = console_gpu.scroll.y;

        for (x = 0; x < TSCREEN_X; ++x)
            for (y = 0; y < TSCREEN_Y; ++y)
                if (console_gpu.screen[(x+sx)%0x100][(y+sy)%0x100])
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
                done = true;
                console.running = false;
            // Handles the controller emulation
            } else if (event.type == SDL_KEYDOWN)
            {
                k = event.key.keysym.sym;
                if (k == console_ctrl.up) controller |= 0x80;
                if (k == console_ctrl.right) controller |= 0x40;
                if (k == console_ctrl.down) controller |= 0x20;
                if (k == console_ctrl.left) controller |= 0x10;
                if (k == console_ctrl.start) controller |= 0x08;
                if (k == console_ctrl.select) controller |= 0x04;
                if (k == console_ctrl.a) controller |= 0x02;
                if (k == console_ctrl.b) controller |= 0x01;
            } else if (event.type == SDL_KEYUP)
            {
                k = event.key.keysym.sym;
                if (k == console_ctrl.up) controller &= ~0x80;
                if (k == console_ctrl.right) controller &= ~0x40;
                if (k == console_ctrl.down) controller &= ~0x20;
                if (k == console_ctrl.left) controller &= ~0x10;
                if (k == console_ctrl.start) controller &= ~0x08;
                if (k == console_ctrl.select) controller &= ~0x04;
                if (k == console_ctrl.a) controller &= ~0x02;
                if (k == console_ctrl.b) controller &= ~0x01;
                
            }
        }
        // Transfer the controller data at &FFF4
        console.memory[0xfff4] = controller;

        // R e f r e s h
        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 30);
    }
    
    // Destroy the window and the SDL Renderer once quitted.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    // Finish the thread.
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
    
    // Map controls
    console_ctrl.up     = D_UP;
    console_ctrl.right  = D_RIGHT;
    console_ctrl.left   = D_LEFT;
    console_ctrl.down   = D_DOWN;

    console_ctrl.start  = D_START;
    console_ctrl.select = D_SELECT;
    
    console_ctrl.a      = D_A;
    console_ctrl.b      = D_B;

    // Read the ROM file
    FILE *rom = fopen(argv[1], "r");
    if (!rom)
    {
        perror("Unable to open ROM: %s");
        exit(1);
    }

    int c;
    done = false;

    // Init CPU and GPU structs
    init_cpu(&console);
    init_gpu(&console_gpu, &console);

    // Transfer the ROM Data into the system memory
    console.pc = 0x0000;
    for (int i = 0; (c=fgetc(rom)) != EOF; ++i)
        console.memory[i] = c;
    
    // The CPU and GPU are running seperately
    // in 2 thread. On each screen refresh,
    // the GPU will increment &FFF0 in the RAM
    pthread_t ct;
    int rc = pthread_create(&ct, NULL, gpu_rendering, NULL);
    // Running the CPU
    run(&console);
    done = true;

    return 0;
}
