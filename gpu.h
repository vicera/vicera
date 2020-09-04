#ifndef GPU_H
#define GPU_H

#include "cpu.h"

/*
 * Small definitions
 */
// Screen size X
#define SCREEN_X    256
// Screen size Y
#define SCREEN_Y    256

// Sprites index
#define M_SPRINDEX  0x8000
// Tiles index
#define M_TILINDEX  0x8100
// Sprites memory
#define M_SPRMEM    0x8500
// Tiles memory
#define M_TILMEM    0x8700

// Refresh counter
#define M_REFRESH   0xfff0
// SCROLL X
#define M_SCROLLX   0xfff1
// SCROLL Y
#define M_SCROLLY   0xfff2

// Sprite size
#define SPRSIZE     8

// Point struct
struct GPU_Point
{
    char x;
    char y;
};

// GPU struct
struct GPU
{
    // The screen itself
    char screen[SCREEN_X][SCREEN_Y];
    // Scrolling
    struct GPU_Point scroll;
    // Pointer to CPU struct
    struct CPU *cpu;
};

// Function prototypes
void init_gpu(struct GPU *gpu, struct CPU *cpu);
void clear_screen(struct GPU *gpu);
void render_screen(struct GPU *gpu);

#endif
