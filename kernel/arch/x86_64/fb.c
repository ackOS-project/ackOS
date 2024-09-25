#include <limine/limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "kernel/arch/x86_64/fb.h"
#include "kernel/arch/x86_64/simple_alloc.h"

extern volatile struct limine_framebuffer_request fb_request;

size_t framebuffer_count(void)
{
    return fb_request.response->framebuffer_count;
}

struct framebuffer framebuffer_allocate(size_t fb_index)
{
    struct framebuffer framebuffer;
    struct limine_framebuffer* fb = fb_request.response->framebuffers[fb_index];
    size_t fb_size = fb->width * fb->height * (fb->bpp / 8);

    framebuffer.backbuffer = fb->address;
    framebuffer.fb_index = fb_index;
    framebuffer.width = fb->width;
    framebuffer.height = fb->height;
    framebuffer.size = fb_size;
    framebuffer.red_mask_shift = fb->red_mask_shift;
    framebuffer.green_mask_shift = fb->green_mask_shift;
    framebuffer.blue_mask_shift = fb->blue_mask_shift;

    return framebuffer;
}

void framebuffer_composite(struct framebuffer* framebuffer, const uint32_t* pixels, size_t width, size_t height, size_t start_x, size_t start_y)
{
    for (size_t y = 0; y < height; y++)
    {
        for(size_t x = 0; x < width; x++)
        {
            uint32_t pixel = pixels[y * width + x];
            uint8_t blue = pixel & 0xff;
            uint8_t green = (pixel >> 8) & 0xff;
            uint8_t red = (pixel >> 16) & 0xff;
            uint8_t alpha = pixel >> 24;
            uint8_t inverted_alpha = 255 - alpha;

            if (alpha == 0) continue;

            size_t abs_x = start_x + x;
            size_t abs_y = start_y + y;

            if(abs_x >= framebuffer->width || abs_y >= framebuffer->height) return;

            size_t px_pos = (abs_y * framebuffer->width) + abs_x;

            uint32_t fb_pixel = framebuffer->backbuffer[px_pos];
            uint8_t fb_r = (fb_pixel & (0xff << framebuffer->red_mask_shift)) >> framebuffer->red_mask_shift;
            uint8_t fb_g = (fb_pixel & (0xff << framebuffer->green_mask_shift)) >> framebuffer->green_mask_shift; 
            uint8_t fb_b = (fb_pixel & (0xff << framebuffer->blue_mask_shift)) >> framebuffer->blue_mask_shift;

            uint8_t new_r = (fb_r * inverted_alpha + (red * alpha)) / 255;
            uint8_t new_g = (fb_g * inverted_alpha + (green * alpha)) / 255;
            uint8_t new_b = (fb_b * inverted_alpha + (blue * alpha)) / 255;

            framebuffer->backbuffer[px_pos] = new_r << framebuffer->red_mask_shift | new_g << framebuffer->green_mask_shift | new_b << framebuffer->blue_mask_shift;
        }
    }
}

void framebuffer_fill(struct framebuffer* framebuffer, uint32_t pixel, size_t width, size_t height, size_t start_x, size_t start_y)
{
    uint8_t blue = pixel & 0xff;
    uint8_t green = (pixel >> 8) & 0xff;
    uint8_t red = (pixel >> 16) & 0xff;
    uint8_t alpha = pixel >> 24;
    uint8_t inverted_alpha = 255 - alpha;

    if (alpha == 0)
    {
        return;
    }
    else if (alpha == 255)
    {
        for (size_t y = 0; y < height; y++)
        {
            for(size_t x = 0; x < width; x++)
            {
                size_t abs_x = start_x + x;
                size_t abs_y = start_y + y;

                if(abs_x >= framebuffer->width || abs_y >= framebuffer->height) return;

                size_t px_pos = (abs_y * framebuffer->width) + abs_x;

                framebuffer->backbuffer[px_pos] = red << framebuffer->red_mask_shift | green << framebuffer->green_mask_shift | blue << framebuffer->blue_mask_shift;
            }
        }
    }
    else
    {
        for (size_t y = 0; y < height; y++)
        {
            for(size_t x = 0; x < width; x++)
            {
                size_t abs_x = start_x + x;
                size_t abs_y = start_y + y;

                if(abs_x >= framebuffer->width || abs_y >= framebuffer->height) return;

                size_t px_pos = (abs_y * framebuffer->width) + abs_x;

                uint32_t fb_pixel = framebuffer->backbuffer[px_pos];
                uint8_t fb_r = (fb_pixel & (0xff << framebuffer->red_mask_shift)) >> framebuffer->red_mask_shift;
                uint8_t fb_g = (fb_pixel & (0xff << framebuffer->green_mask_shift)) >> framebuffer->green_mask_shift; 
                uint8_t fb_b = (fb_pixel & (0xff << framebuffer->blue_mask_shift)) >> framebuffer->blue_mask_shift;

                uint8_t new_r = (fb_r * inverted_alpha + (red * alpha)) / 255;
                uint8_t new_g = (fb_g * inverted_alpha + (green * alpha)) / 255;
                uint8_t new_b = (fb_b * inverted_alpha + (blue * alpha)) / 255;

                framebuffer->backbuffer[px_pos] = new_r << framebuffer->red_mask_shift | new_g << framebuffer->green_mask_shift | new_b << framebuffer->blue_mask_shift;
            }
        }
    }
}

void framebuffer_flush(struct framebuffer* framebuffer)
{
    struct limine_framebuffer* fb = fb_request.response->framebuffers[framebuffer->fb_index];

    if (framebuffer->backbuffer == fb->address) return;

    memcpy(fb->address, framebuffer->backbuffer, framebuffer->size);
}
