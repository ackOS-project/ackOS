#pragma once

// PC screen fonts

// version 1
#define PSF1_MAGIC0     0x36
#define PSF1_MAGIC1     0x04

#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE    0x05

#define PSF1_SEPARATOR  0xFFFF
#define PSF1_STARTSEQ   0xFFFE

// version 2
#define PSF2_MAGIC0     0x72
#define PSF2_MAGIC1     0xb5
#define PSF2_MAGIC2     0x4a
#define PSF2_MAGIC3     0x86

#define PSF2_HAS_UNICODE_TABLE 0x01

#define PSF2_MAXVERSION 0

/* UTF8 separators */
#define PSF2_SEPARATOR  0xFF
#define PSF2_STARTSEQ   0xFE

enum class psf_version
{
    V1,
    V2,
    UNKNOWN
};

struct psf2_font
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

struct psf1_font
{
    unsigned char magic[2];     /* magic number */
    unsigned char mode;         /* font mode */
    unsigned char char_size;    /* character size */
};

void psf_initialize();
