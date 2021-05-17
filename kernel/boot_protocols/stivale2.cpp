#include <cstdint>
#include <cstddef>
#include <cstring>

#include "kernel/boot_protocols/stivale2.h"
#include "kernel/boot_protocols/uniheader.h"

bool stivale2_check_magic(uint32_t magic)
{
    return magic == 0xC0DE;
}

void stivale2_parse_tags(uniheader* uheader, void* sv_header)
{
    stivale2_struct* sv_struct = reinterpret_cast<stivale2_struct*>(sv_header);

    strlcpy(uheader->bootloader_name, sv_struct->bootloader_brand, UNIHEADER_BOOTLOADER_NAME_MAX);

    stivale2_tag* tag = reinterpret_cast<stivale2_tag*>(sv_struct->tags);

    while(tag)
    {
        switch(tag->identifier)
        {
            case STIVALE2_STRUCT_TAG_CMDLINE_ID:
            {
                stivale2_struct_tag_cmdline* command_line = reinterpret_cast<stivale2_struct_tag_cmdline*>(tag);
                strlcpy(uheader->command_line_args, (const char*)command_line->cmdline, UNIHEADER_CMD_ARGS_MAX);

                break;
            }
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
            {
                stivale2_struct_tag_framebuffer* framebuffer = reinterpret_cast<stivale2_struct_tag_framebuffer*>(tag);

                uheader->framebuffer_addr = (void*)framebuffer->framebuffer_addr;
                uheader->framebuffer_width = framebuffer->framebuffer_width;
                uheader->framebuffer_height = framebuffer->framebuffer_height;
                uheader->framebuffer_pitch = framebuffer->framebuffer_pitch;
                uheader->framebuffer_bpp = framebuffer->framebuffer_bpp;

                break;
            }

            default:
                break;
        }

        tag = reinterpret_cast<stivale2_tag*>(tag->next);
    }
}