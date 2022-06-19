#pragma once

#include <cstdint>
#include <cstddef>

enum uniheader_memory_map_entry_type
{
    UNIHEADER_MEMORY_USABLE,
    UNIHEADER_MEMORY_RESERVED,
    UNIHEADER_MEMORY_ACPI_RECLAIMABLE,
    UNIHEADER_MEMORY_NVS,
    UNIHEADER_MEMORY_BAD
};

#define UNIHEADER_MEMORY_MAP_ENTRIES_MAX 32
#define UNIHEADER_MODULE_ENTRIES_MAX 32

#define UNIHEADER_BOOTLOADER_NAME_MAX 64
#define UNIHEADER_CMD_ARGS_MAX 256

struct uniheader_memory_map_entry
{
    uniheader_memory_map_entry_type type;
    void* addr;
    uint64_t length;
};

struct uniheader_memory_map
{
    uniheader_memory_map_entry entries[UNIHEADER_MEMORY_MAP_ENTRIES_MAX];
    size_t entry_count;
};

struct uniheader_framebuffer
{
    void* addr;
    int width;
    int height;
    int pitch;
    int bpp;
};

struct uniheader_module
{
    char command_line_args[UNIHEADER_CMD_ARGS_MAX];
    void* start;
    void* end;
};

struct uniheader
{
    char bootloader_name[UNIHEADER_BOOTLOADER_NAME_MAX];
    char command_line_args[UNIHEADER_CMD_ARGS_MAX];

    uniheader_framebuffer framebuffer;

    void* rsdp;

    uniheader_memory_map memmap;

    void dump();
    void dump_memmap();
    void parse(void* header, uint32_t magic);

    uniheader_module modules[UNIHEADER_MODULE_ENTRIES_MAX];
    size_t module_count;

    uint64_t get_usable_memory();
};
