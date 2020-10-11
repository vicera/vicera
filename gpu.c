/*
 * VICERA by h34ting4ppliance
 *
 * gpu.c
 *
 * This file contains the GPU API to work on the
 * graphical part of the console.
 *
 * Everything has been designed to be easily portable
 * with any other rendering engine.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"
#include "gpu.h"
#include "logging.h"

/*
 * GPU Memory bindings
 * -------------------
 * 4 KiB of VRAM
 *
 * &8000-&80FF  - Sprites index
 * &8100-&84FF  - Tiles index
 * &8500-&86FF  - Sprites memory
 * &8700-&8EFF  - Tiles memory
 *
 * &FFF0        - Refresh (increments on refresh)
 * &FFF1        - SCROLL X
 * &FFF2        - SCROLL Y
 *
 */

/*
 * Sprite memory cell binding
 * --------------------------
 *
 * 01 04 de ad
 * |  |  |  |
 * |  |  |  Y Position of the sprite
 * |  |  X Position of the sprite
 * |  ID of the sprite stored in the Sprites memory
 * (Boolean) is it active?
 */

// struct GPU, struct CPU -> None
// Initialization routine of the struct.
void init_gpu(struct GPU *gpu, struct CPU *cpu)
{
    logging_log("gpu.c", "Initializing GPU...");
    gpu->scroll.x = 0;
    gpu->scroll.y = 0;
    gpu->cpu = cpu;
}

// struct GPU, struct GPU_Point, WORD -> None
// Inserts sprite where it has to.
//
// It reads the sprite from the CPU memory at
// the specified address then insert it at the
// specified position in the screen.
void insert_sprite(struct GPU *gpu, struct GPU_Point pos, WORD addr)
{
    // Reads 8 bytes of memory to make a sprite.
    int line, i;
    BYTE px, py;
    BYTE curbyte;
    
    px = pos.x;
    py = pos.y;

    for (line = 0; line < 8; ++line)
    {
        curbyte = gpu->cpu->memory[addr + line];
        for (i = 0; i < 8; ++i)
        {
            gpu->screen[(px+i) % SCREEN_X][(py+line) % SCREEN_Y] = curbyte & 128;
            curbyte <<= 1;
        }
    }
}

// struct CPU -> None
// Clears the screen
void clear_screen(struct GPU* gpu)
{
    int x, y;
    for (y = 0; y < SCREEN_Y; ++y)
        for (x = 0; x < SCREEN_X; ++x)
            gpu->screen[x][y] = 0;
}

// struct GPU -> None
// Update scrolling from the CPU memory
void update_scrolling(struct GPU *gpu)
{
    gpu->scroll.x = gpu->cpu->memory[M_SCROLLX];
    gpu->scroll.y = gpu->cpu->memory[M_SCROLLY];
}

// struct GPU -> None
// Render the screen
void render_screen(struct GPU *gpu)
{
    int i, t;
    struct GPU_Point pos;

    WORD mempos;

    // Alias to the CPU memory
    BYTE* cpumem = gpu->cpu->memory;
    // Rendering order:
    //  - Clear screen
    //  - Insert tiles
    //  - Inert sprites
    //  - Update scrolling

    // Clear screen.
    clear_screen(gpu);

    // Reads into memory and sets up the tiles.
    for (i = 0; i < 0x400; ++i)
    {
        pos.x = (i % (SCREEN_X / SPRSIZE)) * 8;
        pos.y = (i / (SCREEN_Y / SPRSIZE)) * 8;

        mempos = M_TILMEM + (cpumem[M_TILINDEX + i] * 8);
        insert_sprite(gpu, pos, mempos);
    }

    // Read into memory and sets up the sprites.
    for (i = 0; i < 64; ++i)
    {
        if (!cpumem[M_SPRINDEX + (i * 4)])
            continue;

        pos.x = cpumem[M_SPRINDEX + (i * 4) + 2];
        pos.y = cpumem[M_SPRINDEX + (i * 4) + 3];
        
        t = cpumem[M_SPRINDEX + (i * 4) + 1];
        mempos = M_SPRMEM + (t * 8);
        insert_sprite(gpu, pos, mempos);
    } 

    // Increment refresh
    cpumem[M_REFRESH] += 1;

    // Update scrolling variable
    update_scrolling(gpu);
}
