#include "kernel/psf.h"
#include "kernel/logger.h"

#include <climits>

extern "C" char _binary_fonts_zap_vga16_psf_start;
extern "C" char _binary_fonts_zap_vga16_psf_end;

#define PSF_FONT_START _binary_fonts_zap_vga16_psf_start
#define PSF_FONT_END _binary_fonts_zap_vga16_psf_end

#define PSF_FONT_MAGIC 0x864ab572

psf_version psf_test_version(char& psf_font_start)
{
    psf1_header* font1 = (psf1_header*)&psf_font_start;

    if(font1->magic[0] == PSF1_MAGIC0)
    {
        return psf_version::V1;
    }
    else
    {
        psf2_header* font2 = (psf2_header*)&psf_font_start;

        if(font2->magic[0] == PSF2_MAGIC0)
        {
            return psf_version::V2;
        }
    }

    return psf_version::UNKNOWN;
}

void psf_initialise()
{
    uint16_t glyph = 0;

    psf_version version = psf_test_version(PSF_FONT_START);

    if(version == psf_version::V2)
    {
        log_error(__FILE__, "PSF v2 is not supported yet");
        return;
    }
    else if(version == psf_version::UNKNOWN)
    {
        log_error(__FILE__, "unknown PSF version. Are you livin' in the future?");
        return;
    }
}