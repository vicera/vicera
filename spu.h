/*
 * VICERA by h34ting4ppliance
 * 
 * spu.h
 * 
 * Bindings and structs for everything related to the SPU
 */
#ifndef SPU_H
#define SPU_H

#include "cpu.h"

// SPU Memory bindings
// Square
#define S_SQUARE_CHAN 0xfff5
// Triangle
#define S_TRIANGLE_CHAN 0xfff7
// Noise
#define S_NOISE_CHAN 0xfff9
// Master Volume
#define S_MASTERVOL 0xfffb

// One channel of the SPU.
struct SPU_Channel
{
    BYTE frequency;
    BYTE volume;
};

// The whole SPU containing the 3 channels.
struct SPU
{
    // Master Volume
    BYTE master_volume;

    // All 3 channels
    struct SPU_Channel square;
    struct SPU_Channel triangle;
    struct SPU_Channel noise;

    // The CPU struct. Needed for memory access
    struct CPU *cpu;
};

#endif
