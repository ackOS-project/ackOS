#pragma once

#include <cstdint>
#include <cstddef>

// PC screen fonts

struct psf1_header_t
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t char_size;
};

struct psf_header_t
{
    uint8_t magic[4];
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t num_glyph;
    uint32_t bytes_per_glyph;
    uint32_t height;
    uint32_t width; 
};

psf_header_t* psf_load();

void psf_read_glyph(psf_header_t* header, uint16_t glyph_index, void* pixels, size_t pixel_size, uint32_t fill_colour, uint32_t clear_colour);

void psf_write_to_fb(const char* s);
