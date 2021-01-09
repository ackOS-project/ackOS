#pragma once

#include <cstdint>
#include <cstddef>

struct vbe_header
{
    char magic[4];          /* should be "VESA" */
    uint16_t version;       /* 0x0300 for VBE 3.0 */
    uint16_t oem_string[2];
    uint8_t capabilities[4];
    uint16_t video_mode[2];
    uint16_t memory_capacity;
}
__attribute__((packed));

void vbe_get_modes();
