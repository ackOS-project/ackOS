#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct framebuffer
{
    size_t fb_index;
    // by default this is set to the frontbuffer,
    // so double buffering is disabled.
    uint32_t* backbuffer;
    size_t width, height;
    size_t size;
    size_t red_mask_shift;
    size_t green_mask_shift;
    size_t blue_mask_shift;
};

bool framebuffer_init(void);
struct framebuffer* framebuffer_get(void);
bool framebuffer_allocate_backbuffer(struct framebuffer* framebuffer);

// pixels are in ARGB format where each channel is one byte
void framebuffer_composite(struct framebuffer* framebuffer, const uint32_t* pixels, size_t width, size_t height, size_t start_x, size_t start_y);
void framebuffer_fill(struct framebuffer* framebuffer, uint32_t pixel, size_t width, size_t height, size_t start_x, size_t start_y);

void framebuffer_flush(struct framebuffer* framebuffer);
