#include "kernel/kernel.h"
#include "kernel/psf.h"
#include "kernel/logger.h"

#include <stdint.h>
#include <stddef.h>
#include <climits>

psf_version psf_test_version(char& psf_font_start)
{
    psf1_font* font1 = (psf1_font*)&psf_font_start;

    if(font1->magic[0] == PSF1_MAGIC0)
    {
        return psf_version::V1;
    }
    else
    {
        psf2_font* font2 = (psf2_font*)&psf_font_start;

        if(font2->magic[0] == PSF2_MAGIC0)
        {
            return psf_version::V2;
        }
    }

    return psf_version::UNKNOWN;
}

void psf_initialize()
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