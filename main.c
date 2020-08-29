/*
 * VICERA by h34ting4ppliance
 *
 * VICERA is a fantasy console featuring
 * a monochrome 160x160 display and a simple
 * instruction set.
 */
#include <stdio.h>
#include "cpu.h"
#include "logging.h"

#define FNAME "main.c"
struct CPU system;

int main(int argc, char **argv)
{
    logging_log(FNAME, "-- -- --");
    logging_log(FNAME, " Vicera by h34ting4ppliance!");
    logging_log(FNAME, " have fun!");
    logging_log(FNAME, "-- -- --");
    // Running program
    
    init_cpu(&system);

    system.pc = 0x0000;
    for (int i = 0; program[i] != EOF; ++i)
        system.memory[i] = program[i];
    
    run(&system);
}
