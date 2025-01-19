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

uint16_t *videoBufferGB = NULL;

void video_callback(void *buffer)
{
    printf("Video callback");
}

void audio_callback(void *buffer, size_t length) {}

void loop()
{
    printf("loop");
    gnuboy_run(1);
}

int main(int argc, char **argv)
{
    printf("Setting up GBC\n");

    // Initialize the emulator
    if (gnuboy_init(16000, GB_AUDIO_STEREO_S16, GB_PIXEL_565_LE, &video_callback, &audio_callback) < 0)
        PANIC("Emulator init failed!");

    printf("Emulator initialized\n");

    // // Allocate video and audio buffers
    videoBufferGB = (uint16_t *)malloc(160 * 144 * sizeof(uint16_t));
    if (!videoBufferGB)
        PANIC("Video buffer allocation failed!");
    printf("Video buffer allocated\n");

    gnuboy_set_framebuffer((void *)videoBufferGB);
    printf("Frame buffer set\n");

    EM_ASM({
        move_rom_to_fs();
    });

    if (gnuboy_load_rom_file("rom.gbc") != 0)
    {
        PANIC("ROM Loading failed!");
    }
    printf("ROM opened\n");

    gnuboy_set_palette(GB_PALETTE_DMG);
    printf("Palette set\n");

    // // Hard reset to have a clean slate
    gnuboy_reset(true);
    printf("Emulator reset\n");

    // loop();

    return 0;
}