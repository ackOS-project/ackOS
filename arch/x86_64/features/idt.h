#pragma once
#include <stdint.h>
#include <stddef.h>

struct idt64
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t types_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
};

void idt_initialize();
