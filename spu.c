/*
 * VICERA by h34ting4ppliance
 *
 * spu.c
 *
 * Sound Processing Unit C file containing all the sound system.
 */
#include "spu.h"
#include "cpu.h"

/*
    The SPU works with 3 channels :
        - Square waveform
        - Triangle waveform
        - Noise

    Each channel uses 2 bytes for sound:
    ef 03
    ^  ^
    |  |
    |  Volume
    Frequency

    &FFF5-&FFF6 -> Square waveform
    &FFF7-&FFF8 -> Triangle waveform
    &FFF9-&FFFA -> Noise waveform

    &FFFB       -> Master volume
*/

// struct *SPU -> None
// Reads memory and updates the SPU struct.
void update_spu(struct SPU *spu)
{
    // Will need to read into memory
    // TODO: Write SDL Front-end
    BYTE *memory = spu->cpu->memory;

    spu->square.frequency   = memory[S_SQUARE_CHAN];    
    spu->triangle.frequency = memory[S_TRIANGLE_CHAN];
    spu->noise.frequency    = memory[S_NOISE_CHAN];

    spu->square.volume      = memory[S_SQUARE_CHAN+1];
    spu->triangle.volume    = memory[S_TRIANGLE_CHAN+1];
    spu->noise.volume       = memory[S_NOISE_CHAN+1];
    
    spu->master_volume      = memory[S_MASTERVOL];
}