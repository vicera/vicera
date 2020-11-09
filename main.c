/*
 * VICERA by h34ting4ppliance
 *
 * main.c
 *
 * VICERA is a fantasy console featuring
 * a monochrome 160x160 display and a simple
 * instruction set.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <getopt.h>
#include <pthread.h>

#include "cpu.h"
#include "gpu.h"
#include "controller.h"
#include "logging.h"
#include "sdl_gpu.h"
#include "config.h"
#include "debug.h"

#if FIFOEXT
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fifo.h"
#endif

// Version of the software
#define VERSION "1.0.0 Release"

// Filename for logging
#define FNAME "main.c"

// Usage
#define USAGE   "Usage: vicera [OPTION]..." \
                "\nFantasy Console inspired by the Gameboy." \
                "\n" \
                "\n     -r, --rom           Target ROM file to be run" \
                "\n     -s, --start-at      Begin emulation at 0xXXXX" \
                "\n     -c, --config        Load config file" \
                "\n     -j, --debug-on-jump Disasm on jump instruction" \
                "\n     -d, --debug         Disassemble every ran instr." \
                "\n     -D, --disasm        Disassemble the ROM" \
                "\n     -v, --version       Version of the VICERA" \
                "\n     -h, --help          Print this help and exit" \
                "\n" \
                "\nLicensed under MIT License." \
                "\nThis is free software, you are free and change and redistribute it." \
                "\nWritten and Designed by Matthilde (h34ting4ppliance) Condenseau\n" \

struct CPU console;
struct GPU console_gpu;
struct Controller console_ctrl;
struct SDL_GPUState console_gpu_state;

#define DB_JMP  1   // Debug on jump
#define DB_FULL 2   // Full debug
#define DB_DA   3   // Disassembly
int DEBUGLEVEL = 0;
char* fifoname = NULL;
bool fifo_enabled = false;

// Getopt arguments
static struct option long_options[] = {
    {"rom",      required_argument, NULL, 'r'},
    {"start-at", required_argument, NULL, 's'},
    {"config",   required_argument, NULL, 'c'},
    {"force",    no_argument,       NULL, 'f'},
    {"debug-on-jump", no_argument,  NULL, 'j'},
    {"debug",    no_argument,       NULL, 'd'},
    {"disasm",   no_argument,       NULL, 'D'},
    {"version",  no_argument,       NULL, 'v'},
    {"help",     no_argument,       NULL, 'h'}
};

// ROM filename
char* rom_name;

bool done;

#if FIFOEXT
void fifo_extension(int sig)
{
    receive_from_fifo(&console, fifoname);
    signal(sig, fifo_extension);
}
#endif

void* gpu_rendering()
{
    start_gpu_rendering(&console_gpu, &console_ctrl, &console_gpu_state);
    
    // Finish the thread.
    pthread_exit(NULL);
}

// Prints usge
void print_usage()
{
    printf(USAGE);
}

// Prints version
void print_version()
{
    printf("VICERA version %s\n", VERSION);
}

// Loading configuration
void config_handler(const char* name, const char* value)
{
    #define MATCH(y) strcmp(name, y) == 0
    
    if (MATCH("controller.up"))
        console_ctrl.up = atoi(value);
    else if (MATCH("controller.right"))
        console_ctrl.right = atoi(value);
    else if (MATCH("controller.down"))
        console_ctrl.down = atoi(value);
    else if (MATCH("controller.left"))
        console_ctrl.left = atoi(value);
    else if (MATCH("controller.a"))
        console_ctrl.a = atoi(value);
    else if (MATCH("controller.b"))
        console_ctrl.b = atoi(value);
    else if (MATCH("controller.start"))
        console_ctrl.start = atoi(value);
    else if (MATCH("controller.select"))
        console_ctrl.select = atoi(value);
    else if (MATCH("fifo.enabled"))
    {
        if (strcmp("true", value) == 0)
            fifo_enabled = true;
    }
    else if (MATCH("fifo.filename"))
    {
        fifoname = (char*)malloc(sizeof(char) * (strlen(value) + 1));
        if (!fifoname)
        {
            perror("Unable to malloc");
            exit(1);
        }
        strcpy(fifoname, value);
    }
}

// Passing arguments
void pass_arguments(int argc, char **argv)
{
    int c;

    while ((c = getopt_long(argc, argv, "r:s:c:jDdvh", long_options, NULL)) != -1)
        switch (c)
        {
            case 'r':
                rom_name = malloc(strlen(optarg) * sizeof(char));
                strcpy(rom_name, optarg);
                break;
            case 's':
                console.pc = strtol(optarg, NULL, 16);
                break;
            case 'c':
                parse_config(optarg, &config_handler);
                break;
            case 'v':
                print_version();
                exit(0);
                break;
            case 'h':
                print_usage();
                exit(0);
                break;

            case 'j':
                DEBUGLEVEL = DB_JMP;
                break;
            case 'd':
                DEBUGLEVEL = DB_FULL;
                break;
            case 'D':
                DEBUGLEVEL = DB_DA;
                break;

            default:
            case '?':
                print_usage();
                exit(1);
                break;
        }
}

int main(int argc, char **argv)
{
    // Map controls
    console_ctrl.up     = D_UP;
    console_ctrl.right  = D_RIGHT;
    console_ctrl.left   = D_LEFT;
    console_ctrl.down   = D_DOWN;

    console_ctrl.start  = D_START;
    console_ctrl.select = D_SELECT;
    
    console_ctrl.a      = D_A;
    console_ctrl.b      = D_B;
    
    // Arguments
    rom_name = NULL;
    pass_arguments(argc, argv);
    
    if (!rom_name)
    {
        logging_error(FNAME, "No ROM file specified.");
        exit(1);
    }
    logging_log(FNAME, "-- -- -- -- -- -- -- -- -- --");
    logging_log(FNAME, " Vicera by h34ting4ppliance!");
    logging_log(FNAME, "          have fun!");
    logging_log(FNAME, "-- -- -- -- -- -- -- -- -- --");
    

    // Read the ROM file
    FILE *rom = fopen(rom_name, "r");
    free(rom_name);
    if (!rom)
    {
        perror("Unable to open ROM: %s");
        exit(1);
    }

    int i, c;
    done = false;

    // Init CPU and GPU structs
    init_cpu(&console);
    init_gpu(&console_gpu, &console);
    init_sdlgpu(&console_gpu_state);

    // Transfer the ROM Data into the system memory
    console.pc = 0x0000;
    for (i = 0; (c=fgetc(rom)) != EOF; ++i)
        console.memory[i] = c;
    
    // If debuglevel is Disasm, Disassemble the program then
    // exit.
    if (DEBUGLEVEL == DB_DA)
    {
        logging_log("main.c (DEBUG)", "DEBUG DISASM");
        for (console.pc = 0; console.pc < i; ++console.pc)
            console.pc = disasm_instruction(&console, console.pc);
        exit(0);
    }

    // The CPU and GPU are running seperately
    // in 2 thread. On each screen refresh,
    // the GPU will increment &FFF0 in the RAM
    pthread_t ct;
    pthread_create(&ct, NULL, gpu_rendering, NULL);
    
    // If the fifo extension is enabled on build time,
    // do this things.
    //
    // NOTE: FIFO is not enabled by default because it is
    //       an experimental feature.
    #if FIFOEXT
    if (!mkfifo(fifoname, 0666))
    {
        logging_log(FNAME, "FIFO Enabled!");
        signal(SIGUSR1, fifo_extension);
    }
    #endif
    
    // Wait for SDL to init
    while (!console_gpu_state.ready);

    // Running the CPU
    console.running = true;
    logging_log("main.c (CPU)", "Beginning code execution.");
    bool willjmp = false;
    while (console.running)
    {
        willjmp = false;
        switch (DEBUGLEVEL)
        {
            case DB_JMP:
                if (console.memory[console.pc] >= JP_NN &&
                    console.memory[console.pc] <= CALL_P)
                    willjmp = true;
                break;
            case DB_FULL:
                disasm_instruction(&console, console.pc);
                break;
            default:
                if (console.memory[console.pc] == DBG)
                    disasm_line(&console, ++console.pc, 5);
        }
        
        execute(&console);
        ++console.pc;

        if (willjmp)
            disasm_line(&console, console.pc, 5);
    }
    console.running = false;
    done = true;
    
    #if FIFOEXT
    // Remove the FIFO
    int err = remove(fifoname);
    if (err)
    {
        perror("Error");
        logging_warn(FNAME, "FIFO hasn't been removed properly.");
    }
    #endif
    return 0;
}
