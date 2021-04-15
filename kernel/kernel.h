#pragma once

#include "kernel/io.h"
#include "kernel/drivers/serial.h"
#include "kernel/drivers/legacy_vga.h"
#include "kernel/psf.h"
#include "kernel/mm/memory.h"

extern "C" char _binary_fonts_zap_vga16_psf_start;
extern "C" char _binary_fonts_zap_vga16_psf_end;

#define PSF_FONT_START _binary_fonts_zap_vga16_psf_start
#define PSF_FONT_END   _binary_fonts_zap_vga16_psf_end

#define PSF_FONT_MAGIC 0x864ab572
