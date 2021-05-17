#pragma once

#include <cstdint>
#include <cstddef>

#define UNIHEADER_BOOTLOADER_NAME_MAX 64
#define UNIHEADER_CMD_ARGS_MAX 256

struct uniheader
{
    char bootloader_name[UNIHEADER_BOOTLOADER_NAME_MAX];
    char command_line_args[UNIHEADER_CMD_ARGS_MAX];

    void* framebuffer_addr;
    int framebuffer_width;
    int framebuffer_height;
    int framebuffer_pitch;
    int framebuffer_bpp;
};

void uniheader_parse(uniheader& uheader, void* header, uint32_t magic);
