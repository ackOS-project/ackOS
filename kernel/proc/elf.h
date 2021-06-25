#pragma once

#include <cstddef>
#include <cstdint>

#define ELF_EI_NIDENT 16

typedef uint32_t elf32_addr;
typedef uint16_t elf32_half;
typedef uint32_t elf32_off;
typedef int32_t elf32_sword;
typedef uint32_t elf32_word;

typedef uint64_t elf64_addr;
typedef uint16_t elf64_half;
typedef int16_t	elf64_shalf;
typedef uint64_t elf64_off;
typedef int32_t	elf64_sword;
typedef uint32_t elf64_word;
typedef uint64_t elf64_xword;
typedef int64_t	elf64_sxword;

struct elf32_header
{
    unsigned char e_ident[ELF_EI_NIDENT];
    elf32_half e_type;
    elf32_half e_machine;
    elf32_word e_version;
    elf32_addr e_entry;
    elf32_off e_phoff;
    elf32_off e_shoff;
    elf32_word e_flags;
    elf32_half e_ehsize;
    elf32_half e_phentsize;
    elf32_half e_phnum;
    elf32_half e_shentsize;
    elf32_half e_shnum;
    elf32_half e_shstrndx;
};

struct elf64_header
{
    unsigned char e_ident[ELF_EI_NIDENT];
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
