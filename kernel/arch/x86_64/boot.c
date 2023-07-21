#include <limine/limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <stdio.h>

#include "kernel/lib/log.h"

#include "kernel/arch/x86_64/acpi.h"
#include "kernel/arch/x86_64/com.h"
#include "kernel/arch/x86_64/cpuid.h"
#include "kernel/arch/x86_64/gdt.h"
#include "kernel/arch/x86_64/idt.h"
#include "kernel/arch/x86_64/instr.h"
#include "kernel/arch/x86_64/mem.h"

#include "kernel/logo.h"
#include "lib/liback/util.h"

void kpanic(void)
{
    while(true) halt();
}

volatile struct limine_framebuffer_request fb_request =
{
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// https://github.com/phoboslab/qoi/blob/master/qoi.h
struct ATTR_PACKED qoi_header
{
    char magic[4];
    uint32_t width, height;
    uint8_t channels;    /* 3 = RGB, 4 = RGBA */
    uint8_t colourspace; /* QOI_SRGB, QOI_LINEAR */
};

struct qoi_rgba_t
{
    union {
        struct { uint8_t r, g, b, a; };
        uint32_t value;
    };
};

static const uint8_t qoi_end_bytes[8] = {0, 0, 0, 0, 0, 0, 0, 1};

#define QOI_SRGB 0
#define QOI_LINEAR 1

#define QOI_OP_INDEX 0x00 /* 00xxxxxx */
#define QOI_OP_DIFF 0x40  /* 01xxxxxx */
#define QOI_OP_LUMA 0x80  /* 10xxxxxx */
#define QOI_OP_RUN 0xc0   /* 11xxxxxx */
#define QOI_OP_RGB 0xfe   /* 11111110 */
#define QOI_OP_RGBA 0xff  /* 11111111 */

#define QOI_MASK_2    0xc0 /* 11000000 */

#define QOI_COLOUR_HASH(C) (C.r * 3 + C.g * 5 + C.b * 7 + C.a * 11)

#define QOI_PIXELS_MAX ((uint32_t)400000000)

static uint32_t qoi_convert_be_endian32(uint32_t value)
{
    const uint32_t tmp = 0x00000001;

    if (((const uint8_t*)&tmp)[0] != 0x01) return value;

    return ((value >> 24) & 0x000000FF) |
           ((value >> 8) & 0x0000FF00) |
           ((value << 8) & 0x00FF0000) |
           ((value << 24) & 0xFF000000);
}

void write_image(const void* image, size_t size, struct limine_framebuffer* fb, uint8_t opacity, size_t start_x, size_t start_y)
{
    const struct qoi_header* header = (const struct qoi_header*)image;

    if(!header)
    {
        kprintf(KERN_PANIC "write_image: invalid QOI image - header is NULL\n");
    }
    else if(size < (sizeof(struct qoi_header) + sizeof(qoi_end_bytes)))
    {
        kprintf(KERN_PANIC "write_image: invalid QOI image - size is too small\n");
    }
    else if(strncmp(header->magic, "qoif", 4) != 0)
    {
        kprintf(KERN_PANIC "write_image: invalid QOI image - invalid magic number\n");
    }

    size_t width = qoi_convert_be_endian32(header->width), height = qoi_convert_be_endian32(header->height);

    if(width == 0 || height == 0)
    {
        kprintf(KERN_PANIC "write_image: invalid QOI image - invalid width or height\n");
    }
    else if(height >= (QOI_PIXELS_MAX / width))
    {
        kprintf(KERN_PANIC "write_image: invalid QOI image - invalid height\n");
    }
    else if(header->channels < 3 || header->channels > 4)
    {
        kprintf(KERN_PANIC "write_image: invalid QOI image - invalid number of channels\n");
    }
    else if(header->colourspace != QOI_SRGB && header->colourspace != QOI_LINEAR)
    {
        kprintf(KERN_PANIC "write_image: invalid QOI image - invalid colorspace\n");
    }
    else if(opacity > 255)
    {
        kprintf(KERN_PANIC "write_image: invalid opacity\n");
    }

    const uint8_t* chunk_bytes = (const uint8_t*)(image + sizeof(struct qoi_header));
    size_t chunk_len = size - sizeof(struct qoi_header) - sizeof(qoi_end_bytes);
    size_t pixel_len = width * height * header->channels;
    struct qoi_rgba_t pixel = (struct qoi_rgba_t) {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255
    };

    size_t i = 0;
    size_t run_len = 0;
    struct qoi_rgba_t colour_index[64] = {0};

    for (size_t y = 0; y < height; y++)
    {
        for(size_t x = 0; x < width; x++)
        {
            if (run_len > 0)
            {
                run_len--;
            }
            else if (i < chunk_len)
            {
                uint8_t byte = chunk_bytes[i++];

                if (byte == QOI_OP_RGB)
                {
                    pixel.r = chunk_bytes[i++];
                    pixel.g = chunk_bytes[i++];
                    pixel.b = chunk_bytes[i++];
                }
                else if (byte == QOI_OP_RGBA)
                {
                    pixel.r = chunk_bytes[i++];
                    pixel.g = chunk_bytes[i++];
                    pixel.b = chunk_bytes[i++];
                    pixel.a = chunk_bytes[i++];
                }
                else if ((byte & QOI_MASK_2) == QOI_OP_INDEX)
                {
                    pixel = colour_index[byte];
                }
                else if ((byte & QOI_MASK_2) == QOI_OP_DIFF)
                {
                    pixel.r += ((byte >> 4) & 0x03) - 2;
                    pixel.g += ((byte >> 2) & 0x03) - 2;
                    pixel.b += (byte & 0x03) - 2;
                }
                else if ((byte & QOI_MASK_2) == QOI_OP_LUMA)
                {
                    uint8_t b2 = chunk_bytes[i++];
                    int vg = (byte & 0x3f) - 32;
                    pixel.r += vg - 8 + ((b2 >> 4) & 0x0f);
                    pixel.g += vg;
                    pixel.b += vg - 8 +  (b2 & 0x0f);
                }
                else if ((byte & QOI_MASK_2) == QOI_OP_RUN)
                {
                    run_len = (byte & 0x3f);
                }

                colour_index[QOI_COLOUR_HASH(pixel) % (sizeof(colour_index) / sizeof(*colour_index))] = pixel;
            }

            if(pixel.a == 0) continue;

            size_t abs_x = start_x + x;
            size_t abs_y = start_y + y;

            if(abs_x >= fb->width || abs_y >= fb->height) return;

            size_t px_pos = (abs_y * fb->width) + abs_x;

            uint8_t alpha = pixel.a * opacity / 255;
            uint8_t inverted_alpha = 255 - alpha;

            uint32_t fb_pixel = ((uint32_t*)fb->address)[px_pos];
            uint8_t fb_r = (fb_pixel & (0xff << fb->red_mask_shift)) >> fb->red_mask_shift;
            uint8_t fb_g = (fb_pixel & (0xff << fb->green_mask_shift)) >> fb->green_mask_shift; 
            uint8_t fb_b = (fb_pixel & (0xff << fb->blue_mask_shift)) >> fb->blue_mask_shift;

            uint8_t new_r = (fb_r * inverted_alpha + (pixel.r * alpha)) / 255;
            uint8_t new_g = (fb_g * inverted_alpha + (pixel.g * alpha)) / 255;
            uint8_t new_b = (fb_b * inverted_alpha + (pixel.b * alpha)) / 255;

            ((uint32_t*)fb->address)[px_pos] = new_r << fb->red_mask_shift | new_g << fb->green_mask_shift | new_b << fb->blue_mask_shift;
        }
    }
}

void x86_begin(void)
{
    init_com(COM_PORT1, 115200);

    init_gdt();
    init_idt();

    /* #pragma GCC diagnostic ignored "-Wdiv-by-zero"
        int i = 0 / 0;
        (void)i; */

    init_memory();
    init_acpi();

    char brand_str[49];

    kprintf(KERN_INFO "CPUID brand string: \033[0;33m%s\033[0m\n", cpuid_get_brand_string(brand_str));

#ifdef __ackos__
    kprintf("Hello, ackOS World!\n");
#endif

    if(fb_request.response && fb_request.response->framebuffer_count > 0)
    {
        struct limine_framebuffer* fb = fb_request.response->framebuffers[0]; 

        write_image((const void*)logo_qoi, logo_qoi_len, fb, 200, 0, 0);
    }

    int main(void);

    int res = main();

    if(res != 0)
    {
        kprintf(KERN_PANIC "kernel quitted with error code %d\n", res);
    }

    halt();
}
