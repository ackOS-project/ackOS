#include "kernel/psf.h"
#include "kernel/sys/logger.h"

#include <cstdio>
#include <cassert>
#include <climits>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

#define PSF1_MODE_512 0x01
#define PSF1_MODE_HAS_TAB 0x02
#define PSF1_MODE_HAS_SEQ 0x04
#define PSF1_MAX_MODE 0x05

#define PSF1_SEPARATOR 0xFFFF
#define PSF1_START_SEQ 0xFFFE

#define PSF2_MAGIC0 0x72
#define PSF2_MAGIC1 0xb5
#define PSF2_MAGIC2 0x4a
#define PSF2_MAGIC3 0x86

#define PSF2_HAS_UNICODE_TABLE 0x01

#define PSF2_MAXVERSION 0

/* UTF8 separators */
#define PSF2_SEPARATOR 0xff
#define PSF2_STARTSEQ 0xfe

extern char _binary_fonts_zap_light20_psf_start;
extern char _binary_fonts_zap_light20_psf_end;

#define PSF_FONT_START _binary_fonts_zap_light20_psf_start
#define PSF_FONT_END _binary_fonts_zap_light20_psf_end

psf_version_t psf_test_version(psf1_header* font1)
{
    if(font1->magic[0] == PSF1_MAGIC0 && font1->magic[1] == PSF1_MAGIC1)
    {
        return psf_version_t::V1;
    }
    else if(font1->magic[0] == PSF2_MAGIC0 && font1->magic[1] == PSF2_MAGIC1 && font1->magic[2] == PSF2_MAGIC2 && font1->magic[3] == PSF2_MAGIC3)
    {
        return psf_version_t::V2;
    }

    return psf_version_t::UNKNOWN;
}

void psf_initialise()
{
    psf1_font* font = (psf1_font*)&PSF_FONT_START;
    psf_version_t version = psf_test_version(&font->header);

    if(version == psf_version_t::UNKNOWN)
    {
        log_error("psf", "unknown PC Screen Font version");

        return;
    }
}

char* psf1_get_character(char c)
{
    psf1_font* font = (psf1_font*)&PSF_FONT_START;

    return nullptr;
}

char* psf2_get_character(char c)
{
    psf2_font* font = (psf2_font*)&PSF_FONT_START;

    char* glyph = (char*)font->data + c * font->header.bytes_per_glyph;

    return glyph;
}
