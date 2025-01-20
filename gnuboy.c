#include <stdio.h>
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

void video_callback(void *buffer) {}

void audio_callback(void *buffer, size_t length) {}

void loop()
{
    EM_ASM({
        app.alert("Looping");
    });
    printf("run\n");
    gnuboy_run(1);
    EM_ASM({
        app.alert("Done");
    });
}

int main()
{
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
    printf("Palette set\n");

    gnuboy_reset(true);
    printf("Emulator reset\n");

    EM_ASM({
        app.setInterval("_loop()", 10);
    });


    return 0;
}