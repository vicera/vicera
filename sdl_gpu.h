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

struct SDL_GPUState
{
    bool ready;
    bool done;
};

void init_sdlgpu(struct SDL_GPUState *state);
void start_gpu_rendering(struct GPU *gpu, struct Controller *ctrl, struct SDL_GPUState *state);

#endif
