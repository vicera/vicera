/*
 * VICERA by h34ting4ppliance
 *
 * sdl_gpu.h
 *
 * Headers for sdl_gpu.h
 */
#ifndef SDL_GPU_H
#define SDL_GPU_H

#include "gpu.h"
#include "controller.h"
#include <stdbool.h>

void start_gpu_rendering(struct GPU *gpu, struct Controller *ctrl, bool *done);

#endif