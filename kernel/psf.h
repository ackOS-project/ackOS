#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct psf_font_metrics
{
    size_t glyph_width, glyph_height;
    size_t bytes_per_glyph;
    size_t number_of_glyphs;
};

struct psf_glyph_metrics
{
    size_t width, height;
    size_t size;
    const uint8_t* glyph_data;
};

struct psf_string_metrics
{
    const char* str;
    size_t str_len;
    size_t width, height;
};

struct psf_font_metrics psf_get_font_metrics(void);
struct psf_glyph_metrics psf_get_glyph_metrics(size_t glyph_index);
struct psf_string_metrics psf_get_string_metrics(const char* str, size_t str_len);

void psf_render_glyph(struct psf_glyph_metrics glyph_metrics, uint32_t fill_colour, uint32_t clear_colour, uint32_t* buffer, size_t buffer_width, size_t start_x, size_t start_y);
void psf_render_string(struct psf_string_metrics string_metrics, uint32_t fill_colour, uint32_t clear_colour, uint32_t* rendered_string);

bool psf_is_text_rendering_available(void);
