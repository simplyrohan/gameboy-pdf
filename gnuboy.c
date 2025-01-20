#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>
#include "gnuboy/gnuboy.h"

#define PANIC(x) { printf(x); while (1) ;}

uint16_t *framebuffer;


void video_callback(void *buffer) {
    printf("Video\n");
}

void audio_callback(void *buffer, size_t length) {}

void loop() {
    printf("run\n");
    gnuboy_run(0);
    gnuboy_run(1);
}

int main() {
    if (gnuboy_init(16000, GB_AUDIO_STEREO_S16, GB_PIXEL_565_LE, &video_callback, &audio_callback) < 0)
		PANIC("Emulator init failed!");
    printf("Emulator initialized\n");

    framebuffer = (uint16_t *)malloc(GB_WIDTH * GB_HEIGHT * 2);
	gnuboy_set_framebuffer((void *)framebuffer);
    printf("Framebuffer set\n");

	gnuboy_load_rom_file("rom.gbc");
    printf("ROM Loaded\n");


	gnuboy_set_palette(GB_PALETTE_DMG);
	printf("Palette set");

    gnuboy_reset(true);
	printf("Emulator reset");

    emscripten_set_main_loop(loop, 0, 1);

    return 0;
}