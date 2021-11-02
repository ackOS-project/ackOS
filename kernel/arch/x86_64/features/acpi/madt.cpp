#include "kernel/arch/x86_64/features/acpi/madt.h"
#include "kernel/arch/x86_64/features/acpi/rsdt.h"

namespace x86_64
{
    sdt_t* sdt_find_from_signature(sdt_t* root, const char* signature)
    {
        return nullptr;
    }

    bool sdt_is_checksum_valid(sdt_t* table)
    {
        uint8_t checksum = 0;

        for(size_t i = 0; i < table->length; i++)
        {
            checksum += ((uint8_t*)table)[i];
        }

        return checksum == 0;
    }

    madt_info madt_parse(sdt_t* rsdt)
    {
        return {};
    }
}
