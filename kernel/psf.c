#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel/psf.h"
#include "kernel/font.h"
#include "kernel/lib/log.h"

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04
#define PSF1_MODE512 0
//#define PSF1_MODE_HAS_TAB 1

struct psf1_header
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t char_size;
};

struct psf_font_metrics psf_get_font_metrics(void)
{
    struct psf_font_metrics metrics;
    const struct psf1_header* header = (struct psf1_header*)font_psf;

    metrics.glyph_width = 8;
    metrics.glyph_height = header->char_size;
    metrics.bytes_per_glyph = header->char_size;
    metrics.number_of_glyphs = 256;

    return metrics;
}

inline struct psf_glyph_metrics psf_get_glyph_metrics(size_t glyph_index)
{
    struct psf_glyph_metrics metrics;
    const struct psf1_header* header = (struct psf1_header*)font_psf;

    metrics.width = 8;
    metrics.height = header->char_size;
    metrics.size = header->char_size;
    metrics.glyph_data = (const uint8_t*)font_psf + sizeof(struct psf1_header) + glyph_index * header->char_size;

    return metrics;
}

struct psf_string_metrics psf_get_string_metrics(const char* str, size_t str_len)
{
    struct psf_string_metrics metrics;
    const struct psf1_header* header = (struct psf1_header*)font_psf;

    metrics.str = str;
    metrics.str_len = str_len;
    metrics.width = 8 * str_len;
    metrics.height = header->char_size;

    return metrics;
}

void psf_render_glyph(struct psf_glyph_metrics glyph_metrics, uint32_t fill_colour, uint32_t clear_colour, uint32_t* buffer, size_t buffer_width, size_t start_x, size_t start_y)
{
    for (size_t row = 0; row < glyph_metrics.height; row++)
    {
        for (size_t col = 0; col < glyph_metrics.width; col++)
        {
            uint8_t pixel_row = glyph_metrics.glyph_data[row];
            bool pixel = (pixel_row & (1 << (7 - col))) != 0;

            buffer[(start_y + row) * buffer_width + start_x + col] = pixel ? fill_colour : clear_colour;
        }
    }
}

void psf_render_string(struct psf_string_metrics string_metrics, uint32_t fill_colour, uint32_t clear_colour, uint32_t* rendered_string)
{
    for (size_t i = 0, x = 0; i < string_metrics.str_len; i++)
    {
        struct psf_glyph_metrics glyph_metrics = psf_get_glyph_metrics(string_metrics.str[i]);

        psf_render_glyph(glyph_metrics, fill_colour, clear_colour, rendered_string, string_metrics.width, x, 0);

        x += glyph_metrics.width;
    }
}

bool psf_is_text_rendering_available(void)
{
    const struct psf1_header* header = (const struct psf1_header*)font_psf;

    if (header->magic[0] != PSF1_MAGIC0 || header->magic[1] != PSF1_MAGIC1)
    {
        kprintf(KERN_WARN "PSF font was of wrong version (only v1 is supported)\n");

        return false;
    }

    return true;
}
