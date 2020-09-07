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

#include "cpu.h"
#include "gpu.h"
#include "controller.h"
#include "logging.h"
#include "sdl_gpu.h"

#define FNAME "main.c"

struct CPU console;
struct GPU console_gpu;
struct Controller console_ctrl;

bool done;

void* gpu_rendering()
{
    start_gpu_rendering(&console_gpu, &console_ctrl, &done);
    
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
    pthread_create(&ct, NULL, gpu_rendering, NULL);
    
    // Running the CPU
    run(&console);
    done = true;

    return 0;
}
