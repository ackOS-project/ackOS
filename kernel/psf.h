#pragma once

#include <cstdint>
#include <cstddef>

// PC screen fonts

enum class psf_version_t
{
    V1,
    V2,
    UNKNOWN
};

struct psf2_header
{
    uint32_t magic[4];
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t num_glyph;
    uint32_t bytes_per_glyph;
    uint32_t height;
    uint32_t width; 
};

struct psf2_font
{
    psf2_header header;
    uint8_t data[];
};

struct psf1_header
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t char_size;
};

struct psf1_font
{
    psf1_header header;
    uint8_t data[];
};


void psf_initialise();

char* psf1_get_character(char c);

char* psf2_get_character(char c);
