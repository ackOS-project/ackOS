#pragma once

#include <cstdint>
#include <cstddef>

namespace x86_64
{
    struct rsdp_t
    {
        char signature[8];
        uint8_t checksum;
        char oemid[6];
        uint8_t revision;
        uint32_t rsdt_addr;
    };

    struct rsdp_ext_t
    {
        rsdp_t base;

        uint32_t length;
        uint64_t xsdt_addr;
        uint8_t extended_checksum;
        uint8_t reserved[3];
    };

    struct sdt_t
    {
        char signature[4];
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        char oem_id[6];
        char oem_table_id[8];
        uint32_t oem_revision;
        uint32_t creator_id;
        uint32_t creator_revision;
    };

    void rsdt_init(rsdp_t* rsdp);
    void rsdt_init(sdt_t* rsdt);
};
