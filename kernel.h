#pragma once
#include "kernel-core/io.h"
#include "kernel-core/utils.h"
#include "kernel-core/serial.h"

extern "C" char _binary_zap_vga16_psf_sta;
extern "C" char _binary_zap_vga16_psf_end;
#define PSF_FONT_START _binary_zap_vga16_psf_sta;
#define PSF_FONT_END _binary_zap_vga16_psf_end;
#define PSF_FONT_MAGIC 0x864ab572