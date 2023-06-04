#include <limine/limine.h>

#include "kernel/arch/x86_64/acpi.h"
#include "kernel/arch/x86_64/mem.h"
#include "kernel/lib/log.h"
#include <string.h>
#include <stdlib.h>

static volatile struct limine_rsdp_request rsdp_request =
{
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

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

#define check_signature(SIG_, CORRECT_SIG_) (memcmp(SIG_, CORRECT_SIG_, sizeof(SIG_) / sizeof((SIG_)[0])) == 0)

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

static void print_sdt(const struct acpi_sdt* sdt)
{
    kprintf(KERN_DEBUG "struct acpi_sdt @ %p {\n"
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
                       sdt,
                       sdt->signature[0], sdt->signature[1], sdt->signature[2], sdt->signature[3],
                       sdt->length,
                       sdt->revision,
                       sdt->checksum,
                       sdt->oem_id[0], sdt->oem_id[1], sdt->oem_id[2], sdt->oem_id[3], sdt->oem_id[4], sdt->oem_id[5],
                       sdt->oem_table_id[0], sdt->oem_table_id[1], sdt->oem_table_id[2], sdt->oem_table_id[3], sdt->oem_table_id[4], sdt->oem_table_id[5], sdt->oem_table_id[6], sdt->oem_table_id[7],
                       sdt->oem_revision,
                       sdt->creator_id,
                       sdt->creator_revision);
}

static void print_rsdt(const struct acpi_sdt* sdt)
{
    print_sdt(sdt);

    const uint32_t* entries = (const uint32_t*)(((uintptr_t)sdt) + sizeof(struct acpi_sdt));
    size_t entry_count = (sdt->length - sizeof(struct acpi_sdt)) / sizeof(uint32_t);

    for(size_t i = 0; i < entry_count; i++)
    {
        const struct acpi_sdt* addr = (const struct acpi_sdt*)physical_to_logical((uintptr_t)entries[i]);

        print_sdt(addr);
    }
}

static void print_xsdt(const struct acpi_sdt* sdt)
{
    print_sdt(sdt);

    const uint64_t* entries = (const uint64_t*)(((uintptr_t)sdt) + sizeof(struct acpi_sdt));
    size_t entry_count = (sdt->length - sizeof(struct acpi_sdt)) / sizeof(uint64_t);

    for(size_t i = 0; i < entry_count; i++)
    {
        const struct acpi_sdt* addr = (const struct acpi_sdt*)physical_to_logical(entries[i]);

        print_sdt(addr);
    }
}

static void parse_rsdp(const struct acpi_rsd_ptr* rsdp)
{
    if(rsdp->revision == 0)
    {
        const struct acpi_sdt* sdt = (const struct acpi_sdt*)physical_to_logical(rsdp->rsdt_addr);

        if(!check_signature(sdt->signature, "RSDT") || !check_checksum(sdt, 0, sdt->length))
        {
            kprintf(KERN_PANIC "acpi: invalid RSDT\n");
        }

        print_rsdt(sdt);
    }
    else if(rsdp->revision == 2)
    {
        const struct acpi_sdt* sdt = (const struct acpi_sdt*)physical_to_logical(rsdp->ext.xsdt_addr);

        if(!check_signature(sdt->signature, "XSDT") || !check_checksum(sdt, 0, sdt->length))
        {
            kprintf(KERN_PANIC "acpi: invalid XSDT\n");
        }

        print_xsdt(sdt);
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

    const struct acpi_rsd_ptr* rsdp = rsdp_request.response->address;

    if(!check_signature(rsdp->signature, "RSD PTR ") || !check_checksum(rsdp, 0, sizeof(*rsdp) - sizeof(rsdp->ext)))
    {
        kprintf(KERN_PANIC "acpi: invalid RSDP\n");

        return;
    }
    else if(!check_checksum(rsdp, sizeof(*rsdp) - sizeof(rsdp->ext), sizeof(rsdp->ext)))
    {
        kprintf(KERN_PANIC "acpi: invalid extended RSDP\n");

        return;
    }

    print_rsdp(rsdp);
    parse_rsdp(rsdp);
}
