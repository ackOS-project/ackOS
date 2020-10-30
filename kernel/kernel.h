#pragma once
#include "kernel/io.h"
#include "kernel/utils.h"
#include "kernel/serial.h"

#if defined(__linux__)
#error "You are using a standard gcc/clang compiler. You will need a cross compiler for this to compile successfully."
#endif

#if !defined(__x86_64__)
#error "This OS needs to be compiled with a x86_64-elf compiler."
#endif

extern "C" char _binary_zap_vga16_psf_sta;
extern "C" char _binary_zap_vga16_psf_end;
#define PSF_FONT_START _binary_zap_vga16_psf_sta;
#define PSF_FONT_END _binary_zap_vga16_psf_end;
#define PSF_FONT_MAGIC 0x864ab572