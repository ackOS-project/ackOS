#pragma once

#include "kernel/lib/util.h"

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10
#define USER_CS 0x18
#define USER_DS 0x20

// Privilege levels are defined using "rings" from 0 to 3. Ring 0 is the most privileged and 3 is the least privileged
// Terminology -
// CPL - current privilege level (the privilege level the processor is currently running in)
// RPL - requested privilege level (the privilege level associated with the segment selector)
// DPL - descriptor privilege level (the privilege level defined in the segment descriptor)
#define DPL0 0 /* ring 0 (kernelspace) */
#define DPL3 3 /* ring 3 (userspace) */

enum
{
    SYS_DESC_LDT = 0x2,
    SYS_DESC_TSS = 0x9,
    SYS_DESC_TSS_BUSY = 0xB
};

// Originally, the GDT was intented for segmentation
// When long mode was introduced, segmentation was replaced by more modern 
// memory management techniques such as paging.
// Some fields in the GDT have been repurposed but most are ignored and should be set to 0
struct ATTR_PACKED gdt_entry
{
    uint16_t limit_low; /*  0 */
    uint16_t base_low; /* 0 */
    uint8_t base_mid; /* 0 */
    union {
        struct {
            uint8_t accessed : 1,
                    read_write : 1,
                    direction : 1, /* 0 */
                    executable : 1,
                    is_segment : 1,
                    privilege_level : 2,
                    present : 1;
        };
        uint8_t access_byte;
    };
    union {
        struct {
            uint8_t limit_high : 4, /* 0 */
                    available : 1, /* 0 */
                    long_mode : 1,
                    data_size : 1, /* must be 0; value 1 reserved for future use */
                    granualarity : 1;
        };
        struct {
            uint8_t : 4,
                    flags_byte : 4;
        };
    };
    uint8_t base_high; /* 0 */
};

struct ATTR_PACKED tss_descriptor
{
    uint32_t reserved0;
    uint64_t rsp[3];
    uint64_t reserved1;
    uint64_t ist[7];
    uint32_t reserved2;
    uint32_t reserved3;
    uint16_t reserved4;
    uint16_t iomap_base;
};

struct ATTR_PACKED tss_entry
{
    uint16_t size;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags0;
    uint8_t flags1;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved0;
};

struct ATTR_PACKED gdt_entries
{
    struct gdt_entry entries[5];
    struct tss_entry tss;
};

struct ATTR_PACKED gdt_descriptor
{
    uint16_t size;
    uint64_t addr;
};

void init_gdt(void);

extern void gdt_load(struct gdt_descriptor* desc, uint16_t code, uint16_t data);
