#pragma once

#include <cstdint>
#include <cstddef>

uint8_t inb(uint16_t port);

void outb(uint16_t port, uint8_t data);

void outw(uint16_t port, uint16_t data);

void io_wait();
