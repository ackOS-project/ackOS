#include <stdbool.h>

#include "kernel/lib/log.h"
#include "kernel/arch/x86_64/instr.h"
#include "kernel/arch/x86_64/gdt.h"

// GDT entries should be 8 bytes long; if not this assertion fails
_Static_assert(sizeof(struct gdt_entry) == 8);

static struct gdt_entries gdt;
static struct tss_descriptor tss =
{
    .reserved0 = 0,
    .rsp = {0},
    .reserved1 = 0,
    .ist = {0},
    .reserved2 = 0,
    .reserved3 = 0,
    .reserved4 = 0,
    .iomap_base = 0
};

static struct gdt_descriptor gdt_desc = (struct gdt_descriptor) {
    .size = sizeof(gdt) - 1,
    .addr = (uint64_t)&gdt
};

enum gdt_flags
{
    GDT_READ_WRITE = 1,
    GDT_EXECUTABLE = 1 << 1,
    GDT_LONG_MODE = 1 << 2,
    GDT_DATA32 = 1 << 3,
    GDT_GRANUALARITY4KiB = 1 << 4
};

static struct gdt_entry create_gdt_entry(uint32_t limit, uint32_t base, uint8_t priv, int flags)
{
    struct gdt_entry entry;

    entry.limit_low = (uint16_t)limit;
    entry.limit_high = (uint8_t)(limit >> 16);

    entry.base_low = (uint16_t)base;
    entry.base_mid = (uint8_t)(base >> 16);
    entry.base_high = (uint8_t)(base >> 24);

    entry.accessed = 0;
    entry.read_write = (flags & GDT_READ_WRITE) != 0;
    entry.direction = 0;
    entry.executable = (flags & GDT_EXECUTABLE) != 0;
    entry.is_segment = 1;
    entry.privilege_level = priv;
    entry.present = 1;

    entry.available = 0;
    entry.long_mode = (flags & GDT_LONG_MODE) != 0;
    entry.data_size = (flags & GDT_DATA32) != 0;
    entry.granualarity = (flags & GDT_GRANUALARITY4KiB) != 0;

    return entry;
}

static struct gdt_entry create_null_gdt_entry(void)
{
    struct gdt_entry entry;

    entry.limit_low = 0;
    entry.limit_high = 0;

    entry.base_low = 0;
    entry.base_mid = 0;
    entry.base_high = 0;

    entry.accessed = 0;
    entry.read_write = 0;
    entry.direction = 0;
    entry.executable = 0;
    entry.is_segment = 0;
    entry.privilege_level = 0;
    entry.present = 0;

    entry.available = 0;
    entry.long_mode = 0;
    entry.data_size = 0;
    entry.granualarity = 0;

    return entry;
}

static struct tss_entry create_tss(void)
{
    uintptr_t addr = (uintptr_t)&tss;

    return (struct tss_entry) {
        .size = sizeof(tss),
        .base_low = (uint16_t)addr,
        .base_mid = (uint8_t)(addr >> 16),
        .flags0 = 0b10001001,
        .flags1 = 0,
        .base_high = (uint8_t)(addr >> 24),
        .base_upper = (uint32_t)(addr >> 32),
        .reserved0 = 0
    };
}

static void print_gdt_entry(struct gdt_entries* entries, uint16_t index)
{
    const struct gdt_entry* entry = &entries->entries[index];

    kprintf(KERN_DEBUG
            "GDT index %#04lx {\n"
            "    limit:       %#x\n"
            "        limit_low:    %#hx\n"
            "        limit_high:   %#hx\n"
            "    base:        %#x\n"
            "        base_low:     %#hx\n"
            "        base_mid:     %#hhx\n"
            "        base_high:    %#hhx\n"
            "    access_byte: %#hhx\n"
            "        accessed:     %#hhx\n"
            "        read_write:   %#hhx\n"
            "        direction:    %#hhx\n"
            "        executable:   %#hhx\n"
            "        is_segment:   %#hhx\n"
            "        privilege:    %#hhx\n"
            "        present:      %#hhx\n"
            "    flags:       %#hhx\n"
            "        available:    %#hhx\n"
            "        long_mode:    %#hhx\n"
            "        data_size:    %#hhx\n"
            "        granualarity: %#hhx\n"
            "}\n",
            index * sizeof(struct gdt_entry),
            entry->limit_high << 16 | entry->limit_low,
            entry->limit_low,
            entry->limit_high,
            entry->base_high << 24 | entry->base_mid << 16 | entry->base_low,
            entry->base_low,
            entry->base_mid,
            entry->base_high,
            entry->access_byte,
            entry->accessed,
            entry->read_write,
            entry->direction,
            entry->executable,
            entry->is_segment,
            entry->privilege_level,
            entry->present,
            entry->flags_byte,
            entry->available,
            entry->long_mode,
            entry->data_size,
            entry->granualarity
           );
}

static void print_tss(const struct tss_entry* entry)
{
    kprintf(KERN_DEBUG
            "TSS {\n"
            "    size:   %hu\n"
            "    base:   %#lx\n"
            "       base_low:   %#06hx\n"
            "       base_mid:   %#04hhx\n"
            "       base_high:  %#04hhx\n"
            "       base_upper: %#x\n"
            "    flags0: %#hhx\n"
            "    flags1: %#hhx\n"
            "    res0:   %u\n"
            "}\n",
            entry->size,
            (uintptr_t)entry->base_upper << 32 | (uintptr_t)entry->base_high << 24 | (uintptr_t)entry->base_mid << 16 | (uintptr_t)entry->base_low,
            entry->base_low,
            entry->base_mid,
            entry->base_high,
            entry->base_upper,
            entry->flags0,
            entry->flags1,
            entry->reserved0);
}

static void print_gdt_desc(struct gdt_descriptor desc)
{
    kprintf(KERN_DEBUG "gdt_desc {\n"
                       "    size: %hu\n"
                       "    addr: %#lx\n"
                       "}\n",
                       desc.size,
                       desc.addr);
}

void init_gdt(void)
{
    // The limine protocol requires in addition to 64-bit GDT entries, 16-bit and 32-bit are also defined

    gdt.entries[0] = create_null_gdt_entry();

    gdt.entries[KERNEL16_CS / 8] = create_gdt_entry(0xffff, 0x0, DPL0, GDT_EXECUTABLE | GDT_READ_WRITE);
    gdt.entries[KERNEL16_DS / 8] = create_gdt_entry(0xffff, 0x0, DPL0, GDT_READ_WRITE);

    gdt.entries[KERNEL32_CS / 8] = create_gdt_entry(0xfffff, 0x0, DPL0, GDT_EXECUTABLE | GDT_READ_WRITE | GDT_DATA32 | GDT_GRANUALARITY4KiB);
    gdt.entries[KERNEL32_DS / 8] = create_gdt_entry(0xfffff, 0x0, DPL0, GDT_READ_WRITE | GDT_DATA32 | GDT_GRANUALARITY4KiB);

    gdt.entries[KERNEL64_CS / 8] = create_gdt_entry(0x0, 0x0, DPL0, GDT_READ_WRITE | GDT_EXECUTABLE | GDT_LONG_MODE);
    gdt.entries[KERNEL64_DS / 8] = create_gdt_entry(0x0, 0x0, DPL0, GDT_READ_WRITE);

    gdt.entries[KERNEL_SYSENTER_CS / 8] = create_null_gdt_entry();
    gdt.entries[KERNEL_SYSENTER_DS / 8] = create_null_gdt_entry();

    gdt.entries[USER_CS / 8] = create_gdt_entry(0x0, 0x0, DPL3, GDT_READ_WRITE | GDT_EXECUTABLE | GDT_LONG_MODE);
    gdt.entries[USER_DS / 8] = create_gdt_entry(0x0, 0x0, DPL3, GDT_READ_WRITE);

    gdt.tss = create_tss();

/*
    print_gdt_entry(&gdt, 0);
    print_gdt_entry(&gdt, 1);
    print_gdt_entry(&gdt, 2);
    print_gdt_entry(&gdt, 3);
    print_gdt_entry(&gdt, 4);
    print_gdt_entry(&gdt, 5);
    print_gdt_entry(&gdt, 6);
    print_gdt_entry(&gdt, 7);
    print_gdt_entry(&gdt, 8);
    print_gdt_entry(&gdt, 9);
    print_gdt_entry(&gdt, 10);

    print_tss(&gdt.tss);
    print_gdt_desc(gdt_desc);
*/

    gdt_load(&gdt_desc, KERNEL64_CS, KERNEL64_DS);
}
