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

int rgbfrom565(uint16_t color565)
{
    // Extract 5-bit red, 6-bit green, 5-bcit blue
    uint8_t red = (color565 >> 11) & 0x1F;
    uint8_t green = (color565 >> 5) & 0x3F;
    uint8_t blue = color565 & 0x1F;

    // Convert to 8-bit
    red = (red << 3) | (red >> 2);
    green = (green << 2) | (green >> 4);
    blue = (blue << 3) | (blue >> 2);

    return (red << 16) | (green << 8) | blue;
    // return SDL_MapRGB(screen->format, red, green, blue);
    // return (red << 16) | (green << 8) | blue;
}

void video_callback(void *buffer)
{
    EM_ASM({
        draw_frame($0);
    }, videoBufferGB);
    printf("Frame drawn\n");
}
void audio_callback(void *buffer, size_t length) {}

void loop()
{
    printf("Running\n");
    
    // gnuboy_run(0);
    gnuboy_run(1);

    // EM_ASM({
    //     draw_frame($0);
    // }, videoBufferGB);

    printf("Frame drawn\n");
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


    EM_ASM({
        app.setInterval("_loop()", 10);
    });

    return 0;
}