#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// The current GPU has been designed for SDL.
// But the interface is designed to be portable enough
// to port it to another library.

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

// Width and Height of screen
#define SCREEN_X 256
#define SCREEN_Y 256

// struct containing all required data
struct GPU {
    // The screen itself.
    bool screen[SCREEN_X][SCREEN_Y];
};
