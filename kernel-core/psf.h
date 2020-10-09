// PC screen fonts
#include "../kernel.h"

struct psf_font
{
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width; 
}

void psf_initialize()
{
    uint16_t glyph = 0;

    psf_font* font = (psf_font*)&PSF_FONT_START;

    if(font->flags)
    {
        char* s = (char*)(
            (unsigned char*)&PSF_FONT_START +
            font->headersize +
            font->numglyph * font->bytesperglyph
        );
        // W.I.P
    }

}