/*
 * VICERA by h34ting4ppliance
 *
 * VICERA is a fantasy console featuring
 * a monochrome 160x160 display and a simple
 * instruction set.
 */
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "logging.h"

#define FNAME "main.c"
struct CPU console;

void print_usage()
{
    logging_error(FNAME, "Usage: vicera ROMFILE");
    exit(1);
}

int main(int argc, char **argv)
{
    logging_log(FNAME, "-- -- --");
    logging_log(FNAME, " Vicera by h34ting4ppliance!");
    logging_log(FNAME, " have fun!");
    logging_log(FNAME, "-- -- --");
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
    init_cpu(&console);
    
    console.pc = 0x0000;
    for (int i = 0; (c=fgetc(rom)) != EOF; ++i)
        console.memory[i] = c;
    
    run(&console);
}
