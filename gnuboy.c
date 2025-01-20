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

int pad = 0;

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

            int r = (color >> 11) & 0x1F;
            int g = (color >> 5) & 0x3F;
            int b = color & 0x1F;

            *((Uint32 *)screen->pixels + i * 144 + j) = SDL_MapRGB(screen->format, r << 3, g << 2, b << 3);

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

    // Get keys SDL
    SDL_Event event;
    int pad = 0;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                pad |= GB_PAD_UP;
                break;
            case SDLK_DOWN:
                pad |= GB_PAD_DOWN;
                break;
            case SDLK_LEFT: 
                pad |= GB_PAD_LEFT;
                break;
            case SDLK_RIGHT:
                pad |= GB_PAD_RIGHT;
                break;
            case SDLK_z:
                pad |= GB_PAD_A;
                break;
            case SDLK_x:
                pad |= GB_PAD_B;
                break;
            case SDLK_RETURN:
                pad |= GB_PAD_START;
                break;
            case SDLK_RSHIFT:
                pad |= GB_PAD_SELECT;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                pad &= ~GB_PAD_UP;
                break;
            case SDLK_DOWN:
                pad &= ~GB_PAD_DOWN;
                break;
            case SDLK_LEFT:
                pad &= ~GB_PAD_LEFT;
                break;
            case SDLK_RIGHT:
                pad &= ~GB_PAD_RIGHT;
                break;
            case SDLK_z:
                pad &= ~GB_PAD_A;
                break;
            case SDLK_x:
                pad &= ~GB_PAD_B;
                break;
            case SDLK_RETURN:
                pad &= ~GB_PAD_START;
                break;
            case SDLK_RSHIFT:
                pad &= ~GB_PAD_SELECT;
                break;
            }
            break;
        }
    }
    gnuboy_set_pad(pad);
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