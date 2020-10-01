/*
 * VICERA by h34ting4ppliance
 *
 * fifo.c
 *
 * FIFO Extension headers.
 */
#ifndef FIFO_H
#define FIFO_H

#include "cpu.h"

// TODO: Eventually add a few more commands in the future...
typedef enum
{
    F_NONE,     // No command
    F_WRITE,    // Write in RAM
    F_READ,     // Read from RAM
    F_JMP,      // Jump in the RAM
    F_REGDMP,   // Dump registers
} FIFOInstruction;

// Parsed FIFO Command
struct FIFOCommand
{
    FIFOInstruction inst;
    WORD            args[2];
};

// Buffer
struct LineBuffer
{
    size_t size;
    size_t ptr;

    char* buff;
};

// External functions
void receive_from_fifo(struct CPU* cpu, const char* filename);

#endif
