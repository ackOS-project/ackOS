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

static struct gdt_entry create_gdt_entry(bool rw, bool is_exec, bool long_mode, bool data_size_mode, uint8_t priv)
{
    struct gdt_entry entry;

    entry.limit_low = 0;
    entry.limit_high = 0;
    entry.base_low = 0;
    entry.base_mid = 0;
    entry.base_high = 0;

    entry.accessed = 0;
    entry.read_write = rw;
    entry.direction = 0;
    entry.executable = is_exec;
    entry.is_segment = 1;
    entry.privilege_level = priv;
    entry.present = 1;

    entry.available = 0;
    entry.long_mode = long_mode;
    entry.data_size = data_size_mode;
    entry.granualarity = 1;

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

static void print_gdt_entry(uint16_t index)
{
    const struct gdt_entry* entry = &gdt.entries[index];

    kprintf(KERN_DEBUG
            "GDT index %#04hx {\n"
            "    limit_low:   %#hx\n"
            "    limit_high:  %#hx\n"
            "    base_low:    %#hx\n"
            "    base_mid:    %#hhx\n"
            "    base_high:   %#hhx\n"
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
            index * 8,
            entry->limit_low,
            entry->limit_high,
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
    gdt.entries[0] = create_null_gdt_entry();

    gdt.entries[KERNEL_CS / 8] = create_gdt_entry(true, true, true, false, DPL0);
    gdt.entries[KERNEL_DS / 8] = create_gdt_entry(true, false, false, true, DPL0);

    gdt.entries[USER_CS / 8] = create_gdt_entry(true, true, true, false, DPL3);
    gdt.entries[USER_DS / 8] = create_gdt_entry(true, false, false, true, DPL3);

    gdt.tss = create_tss();

    print_gdt_entry(0);
    print_gdt_entry(1);
    print_gdt_entry(2);
    print_gdt_entry(3);
    print_gdt_entry(4);
    print_tss(&gdt.tss);
    print_gdt_desc(gdt_desc);

    gdt_load(&gdt_desc, KERNEL_CS, KERNEL_DS);

    kprintf(KERN_DEBUG "Current segment registers:\n" 
                       "cs=%#04hx\n"
                       "ds=%#04hx\n",
                        reg_get_cs(),
                        reg_get_ds());

    kprintf(KERN_DEBUG "Loaded GDT:\n");
    
    print_gdt_desc(reg_get_gdtr());
}
