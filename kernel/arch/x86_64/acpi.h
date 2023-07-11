#pragma once

#include <liback/util.h>

/* Advanced Configuration and Power Interface (ACPI)
    -> Root System Descriptor Pointer (RSDP)
        -> Root System Descriptor Table (RSDT)
            -> Multiple APIC Descriptor Table (MADT)
            -> Fixed ACPI Description Table (FADT)
            etc...
*/
struct ATTR_PACKED acpi_rsd_ptr
{
    uint8_t signature[8];
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t revision;
    uint32_t rsdt_addr;

    struct ATTR_PACKED
    {
        uint32_t length;
        uint64_t xsdt_addr;
        uint8_t extended_checksum;
        uint8_t reserved[3];
    } ext;
};

struct ATTR_PACKED acpi_header
{
    uint8_t signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

enum
{
    ACPI_MADT_PROC_LAPIC_ENTRY,
    ACPI_MADT_IO_APIC_ENTRY,
    ACPI_MADT_IO_APIC_INT_SRC_OVERRIDE_ENTRY,
    ACPI_MADT_IO_APIC_NMI_SRC_ENTRY,
    ACPI_MADT_LAPIC_NMI_INT_ENTRY,
    ACPI_MADT_LAPIC_ADDR_OVERRIDE_ENTRY,
    ACPI_MADT_PROC_X2LAPIC = 9
};

struct ATTR_PACKED acpi_madt
{
    struct acpi_header header;
    uint32_t lapic_addr; /* Local Advanced Programmable Interrupt Controller */
    uint32_t flags;
};

struct ATTR_PACKED acpi_madt_entry
{
    uint8_t entry_type;
    uint8_t record_length;
};

struct ATTR_PACKED acpi_madt_proc_lapic_entry
{
    struct acpi_madt_entry entry_info;
    uint8_t proc_id;
    uint8_t apic_id;
    uint32_t flags;
};

struct ATTR_PACKED acpi_madt_io_apic_entry
{
    struct acpi_madt_entry entry_info;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_addr;
    uint32_t global_sys_int_base;
};


struct ATTR_PACKED acpi_madt_io_apic_src_override_entry
{
    struct acpi_madt_entry entry_info;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_sys_int;
    uint16_t flags;
};


struct ATTR_PACKED acpi_madt_io_apic_nmi_int_src_entry
{
    struct acpi_madt_entry entry_info;
    uint8_t nmi_source;
    uint8_t reserved;
    uint16_t flags;
    uint32_t global_sys_int;
};


struct ATTR_PACKED acpi_madt_lapic_nmi_entry
{
    struct acpi_madt_entry entry_info;
    uint8_t proc_id;
    uint16_t flags;
    uint8_t local_int_num;
};


struct ATTR_PACKED acpi_madt_lapic_addr_override_entry
{
    struct acpi_madt_entry entry_info;
    uint16_t reserved;
    uint64_t lapic_addr; /* 64-bit version of the LAPIC address */
};


struct ATTR_PACKED acpi_madt_proc_x2_lapic_entry
{
    struct acpi_madt_entry entry_info;
    uint16_t reserved;
    uint32_t proc_2x_lapic_id;
    uint32_t flags;
    uint32_t apci_id;
};

struct ATTR_PACKED acpi_generic_addr
{
    uint8_t addr_space;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t addr;
};

struct ATTR_PACKED acpi_fadt
{
    struct acpi_header header;
    uint32_t firmware_control;
    uint32_t dsdt;

    uint8_t reserved0;

    uint8_t power_preferrence;
    uint16_t sci_interrupt;
    uint32_t smi_command_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_control;

    uint32_t pm1a_event_block;
    uint32_t pm1b_event_block;

    uint32_t pm1a_control_block;
    uint32_t pm1b_control_block;

    uint32_t pm2_control_block;
    uint32_t pm_timer_block;
    uint32_t gpe0_block;
    uint32_t gpe1_block;

    uint8_t pm1_event_length;
    uint8_t pm1_control_length;
    uint8_t pm2_control_length;

    uint8_t pm_timer_length;
    uint8_t gpe0_length;
    uint8_t gpe1_length;
    uint8_t gpe1_base;
    uint8_t c_state_control;
    uint16_t worst_c2_latency;
    uint16_t worst_c3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;

    uint16_t boot_arch_flags;

    uint8_t reserved1;
    uint32_t flags;

    struct acpi_generic_addr reset_reg;
    uint8_t reset_value;
    uint8_t reserved2[3];

    uint64_t firmware_control64;
    uint64_t dsdt64;

    struct acpi_generic_addr pm1a_event_block64;
    struct acpi_generic_addr pm1b_event_block64;
    struct acpi_generic_addr pm1a_control_block64;
    struct acpi_generic_addr pm1b_control_block64;
    struct acpi_generic_addr pm2_control_block64;
    struct acpi_generic_addr pm_timer_block64;
    struct acpi_generic_addr gpe0_block64;
    struct acpi_generic_addr gpe1_block64;
};

void init_acpi(void);

// * Finds an ACPI header which contains a particular signature
// * Checks the validity of the checksum
// * Will return null if not found
const struct acpi_header* acpi_find_header(const char* signature);

uint32_t* acpi_get_lapic_address(void);
uint32_t* acpi_get_ioapic_address(void);
