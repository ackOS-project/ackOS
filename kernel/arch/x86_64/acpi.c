#include <limine/limine.h>

#include "kernel/arch/x86_64/acpi.h"
#include "kernel/arch/x86_64/mem.h"
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

    const struct acpi_rsd_ptr* rsdp = rsdp_request.response->address;

    if(!rsdp || !check_signature(rsdp->signature, "RSD PTR ") || !check_checksum(rsdp, 0, sizeof(*rsdp) - sizeof(rsdp->ext)))
    {
        kprintf(KERN_PANIC "acpi: invalid RSDP @ %p\n", rsdp);

        return;
    }
    else if(!check_checksum(rsdp, sizeof(*rsdp) - sizeof(rsdp->ext), sizeof(rsdp->ext)))
    {
        kprintf(KERN_PANIC "acpi: invalid extended RSDP\n");

        return;
    }

    parse_rsdp(rsdp);
}
