#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>

#include "kernel/boot/multiboot2.h"

#define next_tag(__tag) ((multiboot_tag*)((uint8_t*)__tag + ((__tag->size + 7) & ~7)))

bool multiboot2_check_magic(uint32_t magic)
{
    return magic == MULTIBOOT2_BOOTLOADER_MAGIC;
}

void multiboot2_parse_memory_map(uniheader* uheader, multiboot_tag_mmap* mmap_tag)
{
    multiboot_memory_map_t* mmap = reinterpret_cast<multiboot_memory_map_t*>(mmap_tag->entries);

    while((uint8_t*)mmap < (uint8_t*)mmap_tag + mmap_tag->size)
    {
        uniheader_memory_map_entry* entry = &uheader->memmap.entries[uheader->memmap.entry_count];

        switch(mmap->type)
        {
        case MULTIBOOT_MEMORY_AVAILABLE:
        {
            entry->type = UNIHEADER_MEMORY_USABLE;

            break;
        }
        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
        {
            entry->type = UNIHEADER_MEMORY_ACPI_RECLAIMABLE;

            break;
        }
        case MULTIBOOT_MEMORY_NVS:
        {
            entry->type = UNIHEADER_MEMORY_NVS;

            break;
        }
        case MULTIBOOT_MEMORY_RESERVED:
        {
            entry->type = UNIHEADER_MEMORY_RESERVED;

            break;
        }
        case MULTIBOOT_MEMORY_BADRAM:
        {
            entry->type = UNIHEADER_MEMORY_BAD;

            break;
        }
        default:
            entry->type = UNIHEADER_MEMORY_BAD;

            break;
        }

        entry->addr = (void*)mmap->addr;
        entry->length = mmap->len;

        mmap = (multiboot_memory_map_t*)((uintptr_t)mmap + mmap_tag->entry_size);
        uheader->memmap.entry_count++;
    }
}

void multiboot2_parse_tags(uniheader* uheader, void* mb_header)
{
    memset((void*)uheader, 0, sizeof(uniheader));

    strcpy((char*)uheader->bootloader_name, "unknown");
    strcpy((char*)uheader->command_line_args, "");

    multiboot_tag* tag = (multiboot_tag*)((uintptr_t)mb_header + 8);

    while(tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
        {
            multiboot_tag_framebuffer* framebuffer = reinterpret_cast<multiboot_tag_framebuffer*>(tag);

            uheader->framebuffer.addr = (void*)framebuffer->common.framebuffer_addr;
            uheader->framebuffer.width = framebuffer->common.framebuffer_width;
            uheader->framebuffer.height = framebuffer->common.framebuffer_height;
            uheader->framebuffer.pitch = framebuffer->common.framebuffer_pitch;
            uheader->framebuffer.bpp = framebuffer->common.framebuffer_bpp;

            break;
        }
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
        {
            multiboot_tag_string* bootloader = reinterpret_cast<multiboot_tag_string*>(tag);

            strlcpy((char*)uheader->bootloader_name, bootloader->string, UNIHEADER_BOOTLOADER_NAME_MAX);

            break;
        }
        case MULTIBOOT_TAG_TYPE_CMDLINE:
        {
            multiboot_tag_string* cmdline = reinterpret_cast<multiboot_tag_string*>(tag);

            strlcpy((char*)uheader->command_line_args, cmdline->string, UNIHEADER_CMD_ARGS_MAX);

            break;
        }
        case MULTIBOOT_TAG_TYPE_ACPI_NEW:
        {
            multiboot_tag_new_acpi* acpi = reinterpret_cast<multiboot_tag_new_acpi*>(tag);

            uheader->rsdp = (void*)acpi->rsdp;

            break;
        }
        case MULTIBOOT_TAG_TYPE_ACPI_OLD:
        {
            multiboot_tag_old_acpi* acpi = reinterpret_cast<multiboot_tag_old_acpi*>(tag);

            uheader->rsdp = (void*)acpi->rsdp;

            break;
        }
        case MULTIBOOT_TAG_TYPE_MMAP:
        {
            multiboot_tag_mmap* mmap = reinterpret_cast<multiboot_tag_mmap*>(tag);

            multiboot2_parse_memory_map(uheader, mmap);

            break;
        }
        case MULTIBOOT_TAG_TYPE_MODULE:
        {
            multiboot_tag_module* mod = reinterpret_cast<multiboot_tag_module*>(tag);

            uniheader_module* module = &uheader->modules[uheader->module_count];

            strncpy(module->command_line_args, (const char*)mod->cmdline, UNIHEADER_CMD_ARGS_MAX);
            module->start = (void*)(uint64_t)mod->mod_start;
            module->end = (void*)(uint64_t)mod->mod_end;

            uheader->module_count++;

            break;
        }
        default:
            break;
        }

        tag = next_tag(tag);
    }
}
