#include "kernel/arch/x86_64/features/acpi/rsdt.h"

namespace x86_64
{
    static sdt_t* rsdt = nullptr;

    void rsdt_init(sdt_t* rsdt)
    {
        rsdt = rsdt;
    }

    void rsdt_init(rsdp_t* rsdp)
    {
        //rsdt = (sdt_t*)rsdp->rsdt_addr + PHYSICAL;
    }
};
