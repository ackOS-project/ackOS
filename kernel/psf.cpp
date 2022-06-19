#include "kernel/psf.h"
#include "kernel/sys/logger.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <climits>

#include "kernel/dev/framebuffer.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

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

#define PSF_FONT_START (_binary_fonts_zap_light20_psf_start)
#define PSF_FONT_END (_binary_fonts_zap_light20_psf_end)

bool psf_is_version2(psf_header_t* header)
{
    if(header->magic[0] == PSF1_MAGIC0 && header->magic[1] == PSF1_MAGIC1)
    {
        return false;
    }
    else if(header->magic[0] == PSF2_MAGIC0 && header->magic[1] == PSF2_MAGIC1 && header->magic[2] == PSF2_MAGIC2 && header->magic[3] == PSF2_MAGIC3)
    {
        return true;
    }

    return false;
}

void psf_read_glyph(psf_header_t* header, uint16_t glyph_index, void* pixels, size_t pixel_size, uint32_t fill_colour, uint32_t clear_colour)
{
    uint8_t* glyphs = (uint8_t*)((uintptr_t)header + header->header_size + glyph_index * header->bytes_per_glyph);
    uint8_t glyph_data[header->bytes_per_glyph];
    int pix_index = 0;

    memcpy(glyph_data, glyphs, header->bytes_per_glyph);
    memset(pixels, clear_colour, pixel_size * (header->height * header->width));

    for(int i = 0; i < header->bytes_per_glyph; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if((i % (header->bytes_per_glyph / header->height) * 8) + j < header->width)
            {
                if(glyph_data[i] & 0x80)
                {
                    memcpy((void*)((uintptr_t)pixels + pix_index), &fill_colour, pixel_size);
                }
                else
                {
                    memcpy((void*)((uintptr_t)pixels + pix_index), &clear_colour, pixel_size);
                }

                pix_index += pixel_size;
            }

            glyph_data[i] <<= 1;
        }
    }
}

psf_header_t* psf_load()
{
    psf_header_t* header = (psf_header_t*)&PSF_FONT_START;

    if(!psf_is_version2(header))
    {
        log_error("psf", "unusable PC Screen Font version");

        return nullptr;
    }

    return header;
}

void psf_write_to_fb(const char* s)
{
    psf_header_t* psf_hdr = psf_load();

    if(!psf_hdr)
    {
        log_error("psf", "failed to load PSF font");

        return;
    }

    size_t s_len = strlen(s);
    uint32_t pixels[psf_hdr->width * psf_hdr->height * s_len];

    int fb_fd = open("/dev/fb0", O_RDWR);

    if(errno != 0)
    {
        log_error("psf", "failed to open framebuffer device - errno %d", errno);
    }

    for(int i = 0; i < s_len; i++)
    {
        uint32_t glyph[psf_hdr->width * psf_hdr->height];

        psf_read_glyph(psf_hdr, s[i], glyph, sizeof(uint32_t), 0xffffffff, 0x00000000);

        for(int y = 0; y < psf_hdr->height; y++)
        {
            for(int x = 0; x < psf_hdr->width; x++)
            {
                int index0 = y * (psf_hdr->width * s_len) + (psf_hdr->width * i + x);
                int index1 = y * psf_hdr->width + x;

                pixels[index0] = glyph[index1];
            }
        }
    }

    framebuffer_write_data_info fb_info = 
    {
        .buff = pixels,
        .buff_width = psf_hdr->width * (uint32_t)s_len,
        .buff_height = psf_hdr->height,
        .x = 0,
        .y = 0
    };

    ioctl(fb_fd, FRAMEBUFFER_CALL_WRITE_DATA, &fb_info);

    if(errno != 0)
    {
        log_error("psf", "failed do an iocall on framebuffer device - errno %d", errno);
    }
}
