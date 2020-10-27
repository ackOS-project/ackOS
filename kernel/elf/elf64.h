#pragma once
#include "kernel/elf/elf.h"

typedef uint64_t elf64_addr;
typedef uint16_t elf64_half;
typedef int16_t	elf64_shalf;
typedef uint64_t elf64_off;
typedef int32_t	elf64_sword;
typedef uint32_t elf64_word;
typedef uint64_t elf64_xword;
typedef int64_t	elf64_sxword;

struct elf64_header
{
    unsigned char e_ident[ELF_EI_NIDENT]; /* Elf magic number (how is it magic?) */
    elf64_half e_type;
    elf64_half e_machine;
    elf64_word e_version;
    elf64_addr e_entry;
    elf64_off e_phoff;
    elf64_off e_shoff;
    elf64_word e_flags;
    elf64_half e_ehsize;
    elf64_half e_phentsize;
    elf64_half e_phnum;
    elf64_half e_shentsize;
    elf64_half e_shnum;
    elf64_half e_shstrndx;
};
