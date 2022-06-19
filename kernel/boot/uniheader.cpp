#include <cstdio>
#include <liback/utils/macros.h>
#include <liback/utils/storage_size.h>

#include "kernel/boot/uniheader.h"
#include "kernel/boot/multiboot2.h"
#include "kernel/boot/stivale2.h"
#include "kernel/sys/panic.h"
#include "kernel/sys/logger.h"

static const char* memory_type_strings[] =
{
    "Usable",
    "Reserved",
    "ACPI Reclaimable",
    "NVS",
    "Bad RAM"
};

void uniheader::parse(void* header, uint32_t magic)
{
    if(stivale2_check_magic(magic))
    {
        stivale2_parse_tags(this, header);
    }
    else if(multiboot2_check_magic(magic))
    {
        multiboot2_parse_tags(this, header);
    }
    else
    {
        kpanic("no suitable boot protocol was found");
    }
}

void uniheader::dump()
{
    printf("{\n"
            "  .bootloader_name = %s\n"
            "  .command_line = %s\n\n",
            bootloader_name, command_line_args);
    printf("  .framebuffer =\n"
           "  {\n"
           "    .addr = 0x%x\n"
           "    .width = %d\n"
           "    .height = %d\n"
           "    .bpp = %d\n"
           "    .pitch = %d\n"
           "  }\n\n"
           "  .rsdp = 0x%x\n\n"
           "  .usable_memory = %s\n"
           "  .memmap.entry_count = %d\n"
           "  .memmap =\n"
           "  {\n",
           framebuffer.addr, framebuffer.width, framebuffer.height, framebuffer.bpp, framebuffer.pitch, rsdp, utils::format_storage_size(get_usable_memory()).c_str(), memmap.entry_count);
    
    for(int i = 0; i < memmap.entry_count; i++)
    {
        printf("    [\n"
               "      .type = %s\n"
               "      .addr = 0x%x\n"
               "      .length = %d\n"
               "    ]\n",
               memory_type_strings[memmap.entries[i].type], memmap.entries[i].addr, memmap.entries[i].length);
    }

    printf("  }\n"
           "  .module_count = %d\n"
           "  .modules =\n"
           "  {\n",
           module_count);

    for(int i = 0; i < module_count; i++)
    {
        printf("    [\n"
               "      .command_line = %s\n"
               "      .start = 0x%d\n"
               "      .end = 0x%d\n"
               "    ]\n",
               modules[i].command_line_args, modules[i].start, modules[i].end);
    }

    puts("  }\n"
         "}");
}

void uniheader::dump_memmap()
{
    log_info("boot", "memory map:");

    for(int i = 0; i < memmap.entry_count; i++)
    {
        log_info("boot", "  [mem %p - %p] %s", memmap.entries[i].addr, (uintptr_t)memmap.entries[i].addr + memmap.entries[i].length, memory_type_strings[memmap.entries[i].type]);
    }
}

uint64_t uniheader::get_usable_memory()
{
    uint64_t usable_memory = 0;

    for(int index = 0; index < memmap.entry_count; index++)
    {
        uniheader_memory_map_entry* entry = &memmap.entries[index];

        if(entry->type == UNIHEADER_MEMORY_USABLE)
        {
            usable_memory += entry->length;
        }
    }

    return usable_memory;
}
