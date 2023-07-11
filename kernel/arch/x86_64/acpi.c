#include <limine/limine.h>

#include "kernel/arch/x86_64/acpi.h"
#include "kernel/arch/x86_64/mem.h"
#include "kernel/arch/x86_64/instr.h"
#include "kernel/lib/log.h"
#include "lib/liback/util.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

static volatile struct limine_rsdp_request rsdp_request =
{
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

static struct {
    const void* entries;
    size_t entry_count;
    bool use_long_addresses;
} sdt;

static struct {
    const struct acpi_madt* madt;
    void* lapic_addr;
    void* io_apic_addr;
} madt_info;

extern volatile struct limine_hhdm_request hhdm_request;

static bool check_checksum(const void* data, size_t start, size_t size)
{
    uint8_t sum = 0;

    for(size_t i = start; i < start + size; i++)
    {
        sum += ((const uint8_t*)data)[i];
    }

    return sum == 0;
}

#define check_signature(SIG_, CORRECT_SIG_) (strncmp((const char*)(SIG_), CORRECT_SIG_, STATIC_LEN(SIG_)) == 0)

static void print_rsdp(const struct acpi_rsd_ptr* rsdp)
{
    kprintf(KERN_DEBUG "struct acpi_rsd_ptr {\n"
                       "    signature: \"%c%c%c%c%c%c%c%c\"\n"
                       "    checksum:  %hhu\n"
                       "    oem_id:    \"%c%c%c%c%c%c\"\n"
                       "    revision:  %hhu\n"
                       "    rsdt_addr: %#x\n"
                       "}\n",
                       rsdp->signature[0], rsdp->signature[1], rsdp->signature[2], rsdp->signature[3], rsdp->signature[4], rsdp->signature[5], rsdp->signature[6], rsdp->signature[7],
                       rsdp->checksum,
                       rsdp->oem_id[0], rsdp->oem_id[1], rsdp->oem_id[2], rsdp->oem_id[3], rsdp->oem_id[4], rsdp->oem_id[5],
                       rsdp->revision,
                       rsdp->rsdt_addr);
    
    if(rsdp->revision > 0)
    {
        kprintf(KERN_DEBUG "struct acpi_rsd_ptr.ext {\n"
                           "        length:    %u\n"
                           "        xsdt_addr: %#lx\n"
                           "        xchecksum: %hhu\n"
                           "}\n",
                           rsdp->ext.length,
                           rsdp->ext.xsdt_addr,
                           rsdp->ext.extended_checksum);
    }
}

static void print_header(const struct acpi_header* header)
{
    kprintf(KERN_DEBUG "struct acpi_header @ %p {\n"
                       "    signature:    \"%c%c%c%c\"\n"
                       "    length:       %u\n"
                       "    revision:     %hhu\n"
                       "    checksum:     %hhu\n"
                       "    oem_id:       \"%c%c%c%c%c%c\"\n"
                       "    oem_table_id: \"%c%c%c%c%c%c%c%c\"\n"
                       "    oem_revision: %u\n"
                       "    creator_id:   %u\n"
                       "    creator_rev:  %u\n"
                       "}\n",
                       header,
                       header->signature[0], header->signature[1], header->signature[2], header->signature[3],
                       header->length,
                       header->revision,
                       header->checksum,
                       header->oem_id[0], header->oem_id[1], header->oem_id[2], header->oem_id[3], header->oem_id[4], header->oem_id[5],
                       header->oem_table_id[0], header->oem_table_id[1], header->oem_table_id[2], header->oem_table_id[3], header->oem_table_id[4], header->oem_table_id[5], header->oem_table_id[6], header->oem_table_id[7],
                       header->oem_revision,
                       header->creator_id,
                       header->creator_revision);
}

static void print_madt(const struct acpi_madt* madt)
{
    print_header(&madt->header);

    kprintf(KERN_DEBUG "struct acpi_madt {\n"
                       "    lapic_addr: %#x\n"
                       "    flags:      %u\n"
                       "}\n",
                       madt->lapic_addr,
                       madt->flags);

    static const char* entry_type_strings[] =
    {
        "Processor Local APIC",
        "IO APIC",
        "IO/APIC Interrupt Source Override",
        "IO/APIC Non-maskable interrupt source",
        "Local APIC Non-maskable interrupts",
        "Local APIC Address Override",
        NULL, NULL, NULL,
        "Processor Local x2APIC "
    };

    size_t proc_count = 0;
    uintptr_t lapic_addr = (uintptr_t)madt->lapic_addr;
    uintptr_t io_apic_addr = 0;

    for(size_t offset = sizeof(struct acpi_madt); offset < madt->header.length;)
    {
        const struct acpi_madt_entry* entry = (const struct acpi_madt_entry*)((uintptr_t)madt + offset);
        const char* type_str = entry->entry_type < STATIC_LEN(entry_type_strings) && entry_type_strings[entry->entry_type] ? entry_type_strings[entry->entry_type] : "unknown";

        kprintf(KERN_DEBUG "struct acpi_madt_entry(entry_type: '%s' (%hhu), record_length: %hhu)\n",
                           type_str,
                           entry->entry_type,
                           entry->record_length);

        switch (entry->entry_type)
        {
            case ACPI_MADT_PROC_LAPIC_ENTRY:
            {
                const struct acpi_madt_proc_lapic_entry* proc_lapic_entry = (const struct acpi_madt_proc_lapic_entry*)entry;

                kprintf(KERN_DEBUG "struct acpi_madt_proc_lapic_entry {\n"
                                   "    proc_id: %hhu\n"
                                   "    apic_id: %hhu\n"
                                   "    flags:   %u\n"
                                   "}\n",
                                   proc_lapic_entry->proc_id,
                                   proc_lapic_entry->apic_id,
                                   proc_lapic_entry->flags);

                if(proc_lapic_entry->flags & 1) proc_count++;
                
                break;
            }
            case ACPI_MADT_IO_APIC_ENTRY:
            {
                const struct acpi_madt_io_apic_entry* io_apic_entry = (const struct acpi_madt_io_apic_entry*)entry;

                kprintf(KERN_DEBUG "struct acpi_madt_io_apic_entry {\n"
                                   "    io_apic_id:          %hhu\n"
                                   "    io_apic_addr:        %#x\n"
                                   "    global_sys_int_base: %#x\n"
                                   "}\n",
                                   io_apic_entry->io_apic_id,
                                   io_apic_entry->io_apic_addr,
                                   io_apic_entry->global_sys_int_base);

                io_apic_addr = io_apic_entry->io_apic_addr;

                break;
            }
            case ACPI_MADT_IO_APIC_INT_SRC_OVERRIDE_ENTRY:
            {
                const struct acpi_madt_io_apic_src_override_entry* io_apic_src_override = (const struct acpi_madt_io_apic_src_override_entry*)entry;

                kprintf(KERN_DEBUG "struct acpi_madt_io_apic_src_override_entry {\n"
                                   "    bus_source:     %hhu\n"
                                   "    irq_source:     %hhu\n"
                                   "    global_sys_int: %u\n"
                                   "    flags:          %hu\n"
                                   "}\n",
                                   io_apic_src_override->bus_source,
                                   io_apic_src_override->irq_source,
                                   io_apic_src_override->global_sys_int,
                                   io_apic_src_override->flags);

                break;
            }
            case ACPI_MADT_IO_APIC_NMI_SRC_ENTRY:
            {
                const struct acpi_madt_io_apic_nmi_int_src_entry* io_apic_nmi_src_entry = (const struct acpi_madt_io_apic_nmi_int_src_entry*)entry;

                kprintf(KERN_DEBUG "struct acpi_madt_io_apic_nmi_int_src_entry {"
                                   "    nmi_source:     %hhu\n"
                                   "    flags:          %hu\n"
                                   "    global_sys_int: %u\n"
                                   "}\n",
                                   io_apic_nmi_src_entry->nmi_source,
                                   io_apic_nmi_src_entry->flags,
                                   io_apic_nmi_src_entry->global_sys_int);

                break;
            }
            case ACPI_MADT_LAPIC_NMI_INT_ENTRY:
            {
                const struct acpi_madt_lapic_nmi_entry* nmi_entry = (const struct acpi_madt_lapic_nmi_entry*)entry;

                kprintf(KERN_DEBUG "struct acpi_madt_lapic_nmi_entry {\n"
                                   "    proc_id:  %hhu\n"
                                   "    flags:    %hu\n"
                                   "    lint_num: %hhu\n"
                                   "}\n",
                                   nmi_entry->proc_id,
                                   nmi_entry->flags,
                                   nmi_entry->local_int_num);

                break;
            }
            case ACPI_MADT_LAPIC_ADDR_OVERRIDE_ENTRY:
            {
                const struct acpi_madt_lapic_addr_override_entry* addr_override_entry = (const struct acpi_madt_lapic_addr_override_entry*)entry;

                kprintf(KERN_DEBUG "struct acpi_madt_lapic_addr_override_entry {\n"
                                   "    lapic_addr: %#lx\n"
                                   "}\n",
                                   addr_override_entry->lapic_addr);
                
                lapic_addr = addr_override_entry->lapic_addr;

                break;
            }
            case ACPI_MADT_PROC_X2LAPIC:
            {
                const struct acpi_madt_proc_x2_lapic_entry* proc_x2_lapic_entry = (const struct acpi_madt_proc_x2_lapic_entry*)entry;

                kprintf(KERN_DEBUG "struct acpi_madt_proc_x2_lapic_entry {\n"
                                   "    proc_x2_lapic_id: %u\n"
                                   "    flags:            %u\n"
                                   "    acpi_id:          %u\n"
                                   "}\n",
                                   proc_x2_lapic_entry->proc_2x_lapic_id,
                                   proc_x2_lapic_entry->flags,
                                   proc_x2_lapic_entry->apci_id);
                
                break;
            }
        }

        offset += entry->record_length;
    }

    kprintf(KERN_DEBUG "total of %zu logical processor(s)\n"
                       "IO APIC: %#lx\n"
                       "LAPIC:   %#lx\n",
                       proc_count,
                       io_apic_addr,
                       lapic_addr);
}

static void parse_madt(const struct acpi_madt* madt)
{
    uintptr_t lapic_addr = (uintptr_t)madt->lapic_addr;
    uintptr_t io_apic_addr = 0;

    if(madt->flags & 1 /* Dual 8259 legacy PICS installed */)
    {
        // disable - we don't need it
        outb(0xa1, 0xff);
        outb(0x21, 0xff);
    }

    for(size_t offset = sizeof(struct acpi_madt); offset < madt->header.length;)
    {
        const struct acpi_madt_entry* entry = (const struct acpi_madt_entry*)((uintptr_t)madt + offset);

        switch (entry->entry_type)
        {
            case ACPI_MADT_PROC_LAPIC_ENTRY:
            {
                const struct acpi_madt_proc_lapic_entry* proc_lapic_entry = (const struct acpi_madt_proc_lapic_entry*)entry;
                
                break;
            }
            case ACPI_MADT_IO_APIC_ENTRY:
            {
                const struct acpi_madt_io_apic_entry* io_apic_entry = (const struct acpi_madt_io_apic_entry*)entry;

                io_apic_addr = io_apic_entry->io_apic_addr;

                break;
            }
            case ACPI_MADT_IO_APIC_INT_SRC_OVERRIDE_ENTRY:
            {
                const struct acpi_madt_io_apic_src_override_entry* io_apic_src_override = (const struct acpi_madt_io_apic_src_override_entry*)entry;

                break;
            }
            case ACPI_MADT_IO_APIC_NMI_SRC_ENTRY:
            {
                const struct acpi_madt_io_apic_nmi_int_src_entry* io_apic_nmi_src_entry = (const struct acpi_madt_io_apic_nmi_int_src_entry*)entry;

                break;
            }
            case ACPI_MADT_LAPIC_NMI_INT_ENTRY:
            {
                const struct acpi_madt_lapic_nmi_entry* nmi_entry = (const struct acpi_madt_lapic_nmi_entry*)entry;

                break;
            }
            case ACPI_MADT_LAPIC_ADDR_OVERRIDE_ENTRY:
            {
                const struct acpi_madt_lapic_addr_override_entry* addr_override_entry = (const struct acpi_madt_lapic_addr_override_entry*)entry;
                
                lapic_addr = addr_override_entry->lapic_addr;

                break;
            }
            case ACPI_MADT_PROC_X2LAPIC:
            {
                const struct acpi_madt_proc_x2_lapic_entry* proc_x2_lapic_entry = (const struct acpi_madt_proc_x2_lapic_entry*)entry;
                
                break;
            }
        }

        offset += entry->record_length;
    }

    madt_info.madt = madt;
    madt_info.lapic_addr = physical_to_io(lapic_addr);
    madt_info.io_apic_addr = physical_to_io(io_apic_addr);
}

uint32_t* acpi_get_lapic_address(void)
{
    return (uint32_t*)madt_info.lapic_addr;
}

uint32_t* acpi_get_ioapic_address(void)
{
    return (uint32_t*)madt_info.io_apic_addr;
}

static void print_sdt(void)
{
    if(sdt.use_long_addresses)
    {
        const uint64_t* entries = (const uint64_t*)sdt.entries;

        for(size_t i = 0; i < sdt.entry_count; i++)
        {
            print_header((const struct acpi_header*)physical_to_io(entries[i]));
        }
    }
    else
    {
        const uint32_t* entries = (const uint32_t*)sdt.entries;

        for(size_t i = 0; i < sdt.entry_count; i++)
        {
            print_header((const struct acpi_header*)physical_to_io((uintptr_t)entries[i]));
        }
    }
}


const struct acpi_header* acpi_find_header(const char* signature)
{
    if(sdt.use_long_addresses)
    {
        const uint64_t* entries = (const uint64_t*)sdt.entries;

        for(size_t i = 0; i < sdt.entry_count; i++)
        {
            const struct acpi_header* header = (const struct acpi_header*)physical_to_io(entries[i]);

            if(header && check_signature(header->signature, signature))
            {
                if(!check_checksum(header, 0, header->length))
                {
                    kprintf(KERN_PANIC "invalid %s\n", signature);
                }

                return header;
            }
        }
    }
    else
    {
        const uint32_t* entries = (const uint32_t*)sdt.entries;

        for(size_t i = 0; i < sdt.entry_count; i++)
        {
            const struct acpi_header* header = (const struct acpi_header*)physical_to_io((uintptr_t)entries[i]);

            if(header && check_signature(header->signature, signature))
            {
                if(!check_checksum(header, 0, header->length))
                {
                    kprintf(KERN_PANIC "invalid %s\n", signature);
                }

                return header;
            }
        }
    }

    return NULL;
}

static void parse_rsdp(const struct acpi_rsd_ptr* rsdp)
{
    if(!rsdp || !check_signature(rsdp->signature, "RSD PTR ") || !check_checksum(rsdp, 0, sizeof(*rsdp) - sizeof(rsdp->ext)))
    {
        kprintf(KERN_PANIC "acpi: invalid RSDP @ %p\n", rsdp);

        return;
    }

    print_rsdp(rsdp);

    if(rsdp->revision == 0)
    {
        const struct acpi_header* header = (const struct acpi_header*)physical_to_io(rsdp->rsdt_addr);

        if(!header || !check_signature(header->signature, "RSDT") || !check_checksum(header, 0, header->length))
        {
            kprintf(KERN_PANIC "acpi: invalid RSDT @ %p\n", header);
        }

        print_header(header);

        sdt.entry_count = (header->length - sizeof(struct acpi_header)) / sizeof(uint32_t);
        sdt.entries = (const uint32_t*)(((uintptr_t)header) + sizeof(struct acpi_header));
        sdt.use_long_addresses = false;
    }
    else if(rsdp->revision == 2)
    {
        if(!check_checksum(rsdp, sizeof(*rsdp) - sizeof(rsdp->ext), sizeof(rsdp->ext)))
        {
            kprintf(KERN_PANIC "acpi: invalid extended RSDP\n");
    
            return;
        }

        const struct acpi_header* header = (const struct acpi_header*)physical_to_io(rsdp->ext.xsdt_addr);

        if(!header || !check_signature(header->signature, "XSDT") || !check_checksum(header, 0, header->length))
        {
            kprintf(KERN_PANIC "acpi: invalid XSDT @ %p\n", header);
        }

        print_header(header);

        sdt.entry_count = (header->length - sizeof(struct acpi_header)) / sizeof(uint64_t);
        sdt.entries = (const void*)(((uintptr_t)header) + sizeof(struct acpi_header));
        sdt.use_long_addresses = true;
    }
    else
    {
        kprintf(KERN_PANIC "acpi: unknown RSDP revision number %d\n", rsdp->revision);
    }
}

void init_acpi(void)
{
    if(!rsdp_request.response)
    {
        kprintf(KERN_PANIC "response to `limine_rsdp_request` was null\n");
    }

    parse_rsdp((const struct acpi_rsd_ptr*)rsdp_request.response->address);

    const struct acpi_madt* madt = (const struct acpi_madt*)acpi_find_header("APIC");

    if(!madt)
    {
        kprintf(KERN_PANIC "No MADT!\n");
        
        return;
    }

    //print_madt(madt);
    parse_madt(madt);

    kprintf(KERN_DEBUG "IO APIC: %p\n"
                       "LAPIC:   %p\n",
                       madt_info.io_apic_addr,
                       madt_info.lapic_addr);
}
