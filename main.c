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

const int program[] = {
    MOV_HN, 0xfe,
    MOV_LN, 0xff,

    INC_HL,
    DUMP_R,
    HALT, -1
};

int main()
{
    logging_log(FNAME, "-- -- --");
    logging_log(FNAME, " Vicera by h34ting4ppliance!");
    logging_log(FNAME, " have fun!");
    logging_log(FNAME, "-- -- --");
    // Running program
    
    struct CPU system;
    init_cpu(&system);

    system.pc = 0x0000;
    for (int i = 0; program[i] != EOF; ++i)
        system.memory[i] = program[i];
    
    run(&system);
}
