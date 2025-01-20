#include <stdio.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <emscripten.h>
#include "gnuboy/gnuboy.h"

#define PANIC(x)   \
    {              \
        printf(x); \
        while (1)  \
            ;      \
    }

uint16_t *framebuffer;

SDL_Surface *screen;

void video_callback(void *buffer)
{
    // printf("Video\n");

    if (SDL_MUSTLOCK(screen))
        SDL_LockSurface(screen);
    for (int i = 0; i < GB_WIDTH; i++)
    {
        for (int j = 0; j < GB_HEIGHT; j++)
        {
            uint16_t color = framebuffer[i * 144 + j];

            *((Uint32*)screen->pixels + i * 144 + j) = (((color >> 11) & 0x1F) << 19) | (((color >> 5) & 0x3F) << 10) | ((color & 0x1F) << 3);
        }
    }
    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);

    SDL_Flip(screen);
}

void audio_callback(void *buffer, size_t length) {}

void loop()
{
    printf("run\n");
    gnuboy_run(0);
    gnuboy_run(1);
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(GB_WIDTH, GB_HEIGHT, 32, SDL_SWSURFACE);

    if (gnuboy_init(16000, GB_AUDIO_STEREO_S16, GB_PIXEL_565_LE, &video_callback, &audio_callback) < 0)
        PANIC("Emulator init failed!");
    printf("Emulator initialized\n");

    framebuffer = (uint16_t *)malloc(GB_WIDTH * GB_HEIGHT * 2);
    gnuboy_set_framebuffer((void *)framebuffer);
    printf("Framebuffer set\n");

    EM_ASM({
        loadROMToFS();
    });

    gnuboy_load_rom_file("rom.gbc");
    printf("ROM Loaded\n");

    gnuboy_set_palette(GB_PALETTE_DMG);
    printf("Palette set");

    gnuboy_reset(true);
    printf("Emulator reset");

    emscripten_set_main_loop(loop, 0, 1);

    SDL_Quit();

    return 0;
}