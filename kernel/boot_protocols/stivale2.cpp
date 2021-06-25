#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>

#include "kernel/boot_protocols/stivale2.h"
#include "kernel/boot_protocols/uniheader.h"

bool stivale2_check_magic(uint32_t magic)
{
    return magic == 0xC0DE;
}

void stivale2_parse_memory_map(uniheader* uheader, stivale2_struct_tag_memmap* memmap_tag)
{
    for(int i = 0; i < memmap_tag->entries; i++)
    {
       stivale2_mmap_entry* entry = &memmap_tag->memmap[i];

        switch(entry->type)
        {
        case STIVALE2_MMAP_USABLE:
        {
            entry->type = UNIHEADER_MEMORY_USABLE;

            break;
        }
        case STIVALE2_MMAP_KERNEL_AND_MODULES:
        {
            entry->type = UNIHEADER_MEMORY_USABLE;

            break;
        }
        case STIVALE2_MMAP_ACPI_RECLAIMABLE:
        {
            entry->type = UNIHEADER_MEMORY_ACPI_RECLAIMABLE;

            break;
        }
        case STIVALE2_MMAP_ACPI_NVS:
        {
            entry->type = UNIHEADER_MEMORY_NVS;

            break;
        }
        case STIVALE2_MMAP_RESERVED:
        {
            entry->type = UNIHEADER_MEMORY_RESERVED;

            break;
        }
        case STIVALE2_MMAP_BAD_MEMORY:
        {
            entry->type = UNIHEADER_MEMORY_BAD;

            break;
        }
        default:
            break;
        }

        uheader->memmap.entries[i].addr = (void*)entry->base;
        uheader->memmap.entries[i].length = entry->length;

        uheader->memmap.entry_count++;
    }
}

void stivale2_parse_tags(uniheader* uheader, void* sv_header)
{
    memset((void*)uheader, 0, sizeof(uniheader));

    stivale2_struct* sv_struct = reinterpret_cast<stivale2_struct*>(sv_header);

    strlcpy(uheader->bootloader_name, sv_struct->bootloader_brand, UNIHEADER_BOOTLOADER_NAME_MAX);
    strcpy(uheader->command_line_args, "");

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

                uheader->framebuffer.addr = (void*)framebuffer->framebuffer_addr;
                uheader->framebuffer.width = framebuffer->framebuffer_width;
                uheader->framebuffer.height = framebuffer->framebuffer_height;
                uheader->framebuffer.pitch = framebuffer->framebuffer_pitch;
                uheader->framebuffer.bpp = framebuffer->framebuffer_bpp;

                break;
            }
            case STIVALE2_STRUCT_TAG_RSDP_ID:
            {
                stivale2_struct_tag_rsdp* rsdp = reinterpret_cast<stivale2_struct_tag_rsdp*>(tag);
                uheader->rsdp = (void*)rsdp->rsdp;

                break;
            }
            case STIVALE2_STRUCT_TAG_MEMMAP_ID:
            {
                stivale2_struct_tag_memmap* map = reinterpret_cast<stivale2_struct_tag_memmap*>(tag);
                stivale2_parse_memory_map(uheader, map);

                break;
            }
            case STIVALE2_STRUCT_TAG_MODULES_ID:
            {
                stivale2_struct_tag_modules* mods = reinterpret_cast<stivale2_struct_tag_modules*>(tag);

                for(int i = 0; i < mods->module_count; i++)
                {
                    uniheader_module* module = &uheader->modules[i];

                    strncpy(module->command_line_args, (const char*)mods->modules[i].string, UNIHEADER_CMD_ARGS_MAX);
                    module->start = (void*)mods->modules[i].begin;
                    module->end = (void*)mods->modules[i].end;

                    uheader->module_count++;
                }
            }

            default:
                break;
        }

        tag = reinterpret_cast<stivale2_tag*>(tag->next);
    }
}