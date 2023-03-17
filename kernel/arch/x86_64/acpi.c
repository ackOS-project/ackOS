#include <limine/limine.h>

#include "kernel/arch/x86_64/acpi.h"
#include "kernel/lib/log.h"
#include <string.h>
#include <stdlib.h>

static volatile struct limine_rsdp_request rsdp_request =
{
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

static bool check_checksum(const void* data, size_t start, size_t size)
{
    uint8_t sum = 0;

    for(size_t i = start; i < start + size; i++)
    {
        sum += ((const uint8_t*)data)[i];
    }

    return sum == 0;
}

void init_acpi(void)
{
    if(!rsdp_request.response)
    {
        kprintf(KERN_PANIC "response to `limine_rsdp_request` was null\n");
    }

    struct acpi_rsd_ptr* rsdp = rsdp_request.response->address;

    if(!check_checksum(rsdp, 0, sizeof(*rsdp) - sizeof(rsdp->ext)))
    {
        kprintf(KERN_PANIC "acpi: invalid RSD pointer\n");
    }
    else if(!check_checksum(rsdp, sizeof(*rsdp) - sizeof(rsdp->ext), sizeof(rsdp->ext)))
    {
        kprintf(KERN_PANIC "acpi: invalid extended RSD pointer\n");
    }
}
