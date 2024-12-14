#include <limine/limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "kernel/arch/x86_64/fb.h"
#include "kernel/mm/alloc.h"

volatile struct limine_framebuffer_request fb_request =
{
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static struct framebuffer global_framebuffer;
static struct framebuffer* global_framebuffer_addr = NULL;


bool framebuffer_init()
{
    if (fb_request.response->framebuffer_count == 0) return false;

    size_t fb_index = 0;

    while (fb_index < fb_request.response->framebuffer_count && fb_request.response->framebuffers[fb_index]->bpp != 32) fb_index++;

    struct limine_framebuffer* fb = fb_request.response->framebuffers[fb_index];
    size_t fb_size = fb->width * fb->height * sizeof(uint32_t);

    if (fb->bpp != 32) return false;

    global_framebuffer.backbuffer = fb->address;
    global_framebuffer.fb_index = fb_index;
    global_framebuffer.width = fb->width;
    global_framebuffer.height = fb->height;
    global_framebuffer.size = fb_size;
    global_framebuffer.red_mask_shift = fb->red_mask_shift;
    global_framebuffer.green_mask_shift = fb->green_mask_shift;
    global_framebuffer.blue_mask_shift = fb->blue_mask_shift;

    global_framebuffer_addr = &global_framebuffer;

    return true;
}

struct framebuffer* framebuffer_get(void)
{
    return global_framebuffer_addr;
}

bool framebuffer_allocate_backbuffer(struct framebuffer* framebuffer)
{
    if (!framebuffer) return true;

    uint32_t* backbuffer = kmalloc(framebuffer->size, 0);

    if (!backbuffer) return false;

    memcpy(backbuffer, framebuffer->backbuffer, framebuffer->size);

    framebuffer->backbuffer = backbuffer;

    return true;
}

void framebuffer_composite(struct framebuffer* framebuffer, const uint32_t* pixels, size_t width, size_t height, size_t start_x, size_t start_y)
{
    width = MIN(framebuffer->width - start_x, width);
    height = MIN(framebuffer->height - start_y, height);

    for (size_t y = 0; y < height; y++)
    {
        const uint32_t* pixels_row = &pixels[y * width];
        uint32_t* fb_row = &framebuffer->backbuffer[(start_y + y) * framebuffer->width + start_x];

        for(size_t x = 0; x < width; x++)
        {
            uint32_t pixel = pixels_row[x];
            uint8_t alpha = pixel >> 24;

            if (alpha == 0) continue;

            uint8_t blue = pixel & 0xff;
            uint8_t green = (pixel >> 8) & 0xff;
            uint8_t red = (pixel >> 16) & 0xff;
            uint8_t inverted_alpha = 255 - alpha;

            if (alpha == 255)
            {
                fb_row[x] = red << framebuffer->red_mask_shift | green << framebuffer->green_mask_shift | blue << framebuffer->blue_mask_shift;

                continue;
            }

            size_t abs_x = start_x + x;
            size_t abs_y = start_y + y;

            size_t px_pos = (abs_y * framebuffer->width) + abs_x;

            uint32_t fb_pixel = fb_row[x];
            uint8_t fb_r = (fb_pixel >> framebuffer->red_mask_shift) & 0xff;
            uint8_t fb_g = (fb_pixel >> framebuffer->green_mask_shift) & 0xff; 
            uint8_t fb_b = (fb_pixel >> framebuffer->blue_mask_shift) & 0xff;

            uint8_t new_r = (fb_r * inverted_alpha + (red * alpha)) / 255;
            uint8_t new_g = (fb_g * inverted_alpha + (green * alpha)) / 255;
            uint8_t new_b = (fb_b * inverted_alpha + (blue * alpha)) / 255;

            fb_row[x] = new_r << framebuffer->red_mask_shift | new_g << framebuffer->green_mask_shift | new_b << framebuffer->blue_mask_shift;
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

    width = MIN(framebuffer->width - start_x, width);
    height = MIN(framebuffer->height - start_y, height);

    if (alpha == 0)
    {
        return;
    }
    else if (alpha == 255)
    {
        uint32_t new_pixel = red << framebuffer->red_mask_shift | green << framebuffer->green_mask_shift | blue << framebuffer->blue_mask_shift;

        for (size_t y = 0; y < height; y++)
        {
            size_t abs_y = start_y + y;
            uint32_t* row = &framebuffer->backbuffer[(abs_y * framebuffer->width) + start_x];

            for (size_t x = 0; x < width; x++)
            {
                row[x] = new_pixel;
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

                size_t px_pos = (abs_y * framebuffer->width) + abs_x;

                uint32_t fb_pixel = framebuffer->backbuffer[px_pos];
                uint8_t fb_r = (fb_pixel >> framebuffer->red_mask_shift) & 0xff;
                uint8_t fb_g = (fb_pixel >> framebuffer->green_mask_shift) & 0xff; 
                uint8_t fb_b = (fb_pixel >> framebuffer->blue_mask_shift) & 0xff;

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
