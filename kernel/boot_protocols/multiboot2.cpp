#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>

#include "kernel/boot_protocols/multiboot2.h"

#define next_tag(tag) ((multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7)))

const char* _multiboot2_tag_names[] =
{
    "END",
    "CMDLINE",
    "BOOT_LOADER_NAME",
    "MODULE",
    "BASIC_MEMINFO",
    "BOOTDEV",
    "MMAP",
    "VBE",
    "FRAMEBUFFER",
    "ELF_SECTIONS",
    "APM",
    "EFI32",
    "EFI64",
    "SMBIOS",
    "ACPI_OLD",
    "ACPI_NEW",
    "NETWORK",
    "EFI_MMAP",
    "EFI_BS",
    "EFI32_IH",
    "EFI64_IH",
    "LOAD_BASE_ADDR"
};

bool multiboot2_check_magic(uint32_t magic)
{
    return magic == MULTIBOOT2_BOOTLOADER_MAGIC;
}

void multiboot2_parse_tags(uniheader* uheader, void* mb_header)
{
    strcpy((char*)uheader->bootloader_name, "unknown");

    multiboot_tag* tag = (multiboot_tag*)((uintptr_t)mb_header + 8);

    while(tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
        {
            multiboot_tag_framebuffer_common* framebuffer = reinterpret_cast<multiboot_tag_framebuffer_common*>(tag);

            uheader->framebuffer_addr = (void*)framebuffer->framebuffer_addr;
            uheader->framebuffer_width = framebuffer->framebuffer_width;
            uheader->framebuffer_height = framebuffer->framebuffer_height;
            uheader->framebuffer_pitch = framebuffer->framebuffer_pitch;
            uheader->framebuffer_bpp = framebuffer->framebuffer_bpp;

            break;
        }
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
        {
            strcpy((char*)uheader->bootloader_name, (reinterpret_cast<multiboot_tag_string*>(tag))->string);

            break;
        }
        case MULTIBOOT_TAG_TYPE_CMDLINE:
        {
            strcpy((char*)uheader->command_line_args, (reinterpret_cast<multiboot_tag_string*>(tag))->string);

            break;
        }
        default:
            printf("unknown tag '%s' (%d)\n", _multiboot2_tag_names[tag->type], tag->type);
            break;
        }

        tag = next_tag(tag);
    }
}
