#pragma once
// Headers
//#include "utils.h"
#include <cstdint>
#include <cstddef>

// Hardware comunication
unsigned char inb(unsigned short int port);

void outb(unsigned short int port, unsigned char data);

void outw(unsigned short int port, unsigned short int data);

// Power management functions

void reboot();

void shutdown();

// Memory

struct mutex
{
	uint8_t locked;
};

struct alloc_t
{
	uint8_t status;
	uint32_t size;
};

#include "kernel/mm/memory.h"
// Genral input/output stuff


