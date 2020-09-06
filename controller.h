#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>

/*
 * RAM Controller Mapping at &FFF4
 *
 * 0 0 0 0 0 0 0 0
 * | | | | | | | |
 * | | | | | | | B
 * | | | | | | A
 * | | | | | Select
 * | | | | Start
 * | | | Left
 * | | Down
 * | Right
 * Up
 */

struct Controller
{
    unsigned int up;
    unsigned int right;
    unsigned int down;
    unsigned int left;

    unsigned int start;
    unsigned int select;

    unsigned int a;
    unsigned int b;
};

// NOTE: Please configure your keybinds here.
#define D_UP    SDLK_UP
#define D_RIGHT SDLK_RIGHT
#define D_LEFT  SDLK_LEFT
#define D_DOWN  SDLK_DOWN

#define D_START SDLK_RETURN
#define D_SELECT SDLK_BACKSPACE

#define D_A     SDLK_z
#define D_B     SDLK_x

#endif
