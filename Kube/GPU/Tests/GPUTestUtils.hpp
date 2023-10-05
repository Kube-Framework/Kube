/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: GPU testing utilities
 */

#pragma once

#include <SDL2/SDL.h>

#include <Kube/GPU/GPU.hpp>

namespace Tests
{
    struct GPUInstance
    {
        SDL_Window *window;
        kF::GPU::GPU::GlobalInstance gpu;

        inline ~GPUInstance(void) noexcept
        {
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        inline GPUInstance(void) noexcept
            :   window([] {
                    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
                    return SDL_CreateWindow("Test", 0, 0, 400, 400, SDL_WINDOW_VULKAN);
                }()),
                gpu(window, kF::GPU::FrameImageModels {}, {}) {}
    };
}