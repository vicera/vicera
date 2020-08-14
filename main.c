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

const BYTE program[] = {
    // TODO: Make a program to test all the opcodes.
    MOV_AN, 0x42,
    MOV_BA, MOV_CB,
    MOV_BN, 0x01,
    ADD_B,
    DUMP_M, 0x0000,
    DUMP_R,
    MOV_AN, 0x00,
    MOV_BA, MOV_CA, MOV_DA, MOV_EA, MOV_HA, MOV_LA,
    DUMP_R,
    HALT,
    0xff
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
    for (int i = 0; program[i] != 0xff; ++i)
        system.memory[i] = program[i];
    
    run(&system);
}
