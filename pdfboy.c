#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>

#include "gnuboy/gnuboy.h"

#define PANIC(x)          \
    {                     \
        printf(x); \
        while (1)          \
            ;             \
    }

uint16_t *framebuffer;

void video_callback(void *buffer) {}

void audio_callback(void *buffer, size_t length) {}


void loop() {
    printf("Loop\n");
    gnuboy_run(1);
    printf("Loop done\n");
}

int main() {
    printf("Welcome to PDFBoy\n");

    // ---- Initialize GNUBoy
    if (gnuboy_init(16000, GB_AUDIO_STEREO_S16, GB_PIXEL_565_LE, &video_callback, &audio_callback) < 0)
        PANIC("Emulator init failed!\n");

    printf("GNUBoy Initialized\n");

    // ---- Video Framebuffer setup
    printf("Allocating Framebuffer\n");

    framebuffer = (uint16_t *)malloc(GB_WIDTH * GB_HEIGHT * 2);
    if (!framebuffer)
        PANIC("Framebuffer allocation failed!\n");
	gnuboy_set_framebuffer((void *)framebuffer);

    printf("Framebuffer allocated and set\n");

	gnuboy_set_palette(GB_PALETTE_DMG);
    printf("Palette Set\n");

    // ---- ROM
    printf("Loading ROM\n");
    // EM_ASM({
    //     loadROMToFS();
    // });
    // printf("ROM Moved to FS\n");

    if (gnuboy_load_rom_file("rom.gbc") == -1)
        PANIC("ROM loading failed!");
    
    printf("ROM Loaded!\n");

	gnuboy_reset(true);
    printf("Emulator Reset\n");

    // gnuboy_run(1);

    printf("starting!\n");

    EM_ASM({
        // app.setInterval("_loop()", 0);
        setInterval(() => {
            _loop();
        }, 10);
    });

    return 0;
}