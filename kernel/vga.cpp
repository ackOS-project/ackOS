#include "kernel/vga.h"
#include "kernel/io.h"
#include <cstring>

// A text-based terminal session in VGA mode (not supported on most hardware)
namespace vga_terminal
{
	uint16_t vga_entry(unsigned char uc, uint8_t colour)
	{
		return (uint16_t)uc | (uint16_t)colour << 8;
	}

	void enable_cursor(unsigned char cursor_start, unsigned char cursor_end)
	{
		outb(0x3D4, 0x0A);
		outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

		outb(0x3D4, 0x0B);
		outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
	}

	void disable_cursor(void)
	{
		outb(0x3D4, 0x0A);
		outb(0x3D5, 0x20);
	}

	void set_cursor_pos(int x, int y)
	{
		uint16_t pos = y * VGA_WIDTH + x;

		outb(0x3D4, 0x0F);
		outb(0x3D5, (unsigned char)(pos & 0xFF));
		outb(0x3D4, 0x0E);
		outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
	}

	void update_cursor(void)
	{
		set_cursor_pos(terminal_column, terminal_row);
	}

	unsigned char vga_colour(enum vga_colour fg, enum vga_colour bg)
	{
		return fg | bg << 4;
	}

	void initialize(uint8_t colour)
	{
		terminal_row = 0;
		terminal_column = 0;
		terminal_colour = colour;

		for (size_t y = 0; y < VGA_HEIGHT; y++)
		{
			for (size_t x = 0; x < VGA_WIDTH; x++)
			{
				const size_t index = y * VGA_WIDTH + x;
				vidmem[index] = vga_entry(' ', terminal_colour);
			}
		}
	}

	void clear(void)
	{
		terminal_row = 0;
		terminal_column = 0;

		for (size_t y = 0; y < VGA_HEIGHT; y++)
		{
			for (size_t x = 0; x < VGA_WIDTH; x++)
			{
				const size_t index = y * VGA_WIDTH + x;
				vidmem[index] = vga_entry(' ', terminal_colour);
			}
		}
	}

	void putc(char c)
	{
		const size_t index = terminal_row * VGA_WIDTH + terminal_column;
		vidmem[index] = vga_entry(c, terminal_colour);

		if (++terminal_column == VGA_WIDTH)
		{
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT)
			{
				terminal_row = 0;
			}
		}
	}

	void write(const char *data)
	{
		for (int i = 0; i < strlen(data); i++)
		{
			char c = data[i];

			if(c == '\n')
			{
				terminal_row = 0; // x -
				terminal_column++;  // y |
			}
			else
			{
				putc(c);
			}
		}
	}
};