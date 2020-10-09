#pragma once
// Headers
#include "utils.h"

// Hardware comunication
unsigned char inb (unsigned short int port)
{
    	unsigned char rv;
    	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (port));
    	return rv;
}

void outb (unsigned short int port, unsigned char data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

void outw(unsigned short int port, unsigned short int data)
{
	asm volatile("outw %%ax, %%dx": :"d" (port), "a" (data));
}

// System functions

#define bit(n) (1<<(n))
#define check_flag(flags, n) ((flags) & bit(n))
void reboot()
{
    unsigned char temp;
 
    asm volatile ("cli"); /* disable interrupts */
    do
    {
        temp = inb(0x64); /* clear user data */
        if (check_flag(temp, 0) != 0){
            inb(0x60); /* empty keyboard data */
        }
    } while (check_flag(temp, 1) != 0);
 
    outb(0x64, 0xFE); /* send CPU reset */
loop:
    asm volatile ("hlt"); /* Halt the CPU */
    goto loop;
}

void shutdown(){
    // Shutdown registers in qemu and virtual box
    outw(0xB004, 0x2000);
    outw(0x604, 0x2000);
    outw(0x4004, 0x3400);
}

// Memory

typedef struct {
	uint8_t locked;
} mutex;
typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

void* memset (void * ptr, int value, size_t num )
{
	unsigned char* p=(unsigned char*)ptr;
	while(num--)
		*p++ = (unsigned char)value;
	return ptr;
}
uintptr_t current_break;
void *sbrk(intptr_t incr)
{
    uintptr_t old_break = current_break;
    current_break += incr;
    return (void*) old_break;
}
//extern "C" {
#include "mm.h"
//}
// Genral input/output stuff

enum vga_colour {
	VGA_COLOUR_BLACK = 0,
	VGA_COLOUR_BLUE = 1,
	VGA_COLOUR_GREEN = 2,
	VGA_COLOUR_CYAN = 3,
	VGA_COLOUR_RED = 4,
	VGA_COLOUR_MAGENTA = 5,
	VGA_COLOUR_BROWN = 6,
	VGA_COLOUR_LIGHT_GREY = 7,
	VGA_COLOUR_DARK_GREY = 8,
	VGA_COLOUR_LIGHT_BLUE = 9,
	VGA_COLOUR_LIGHT_GREEN = 10,
	VGA_COLOUR_LIGHT_CYAN = 11,
	VGA_COLOUR_LIGHT_RED = 12,
	VGA_COLOUR_LIGHT_MAGENTA = 13,
	VGA_COLOUR_LIGHT_BROWN = 14,
	VGA_COLOUR_WHITE = 15,
};

// A text-based terminal session in VGA mode
namespace vga_terminal {
// Private (i wish cpp had visability in namespaces). So I use static(it's not the same thing)

    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;
    static size_t terminal_row;
    static size_t terminal_column;
    static unsigned char terminal_colour;
    static unsigned short int* terminal_buffer;
    static inline uint16_t vga_entry(unsigned char uc, uint8_t colour) 
    {
		return (uint16_t) uc | (uint16_t) colour << 8;
    }

// Public

    const size_t get_vga_width() { return VGA_WIDTH; }
    const size_t get_vga_height() { return VGA_HEIGHT; }

    // Enables the cursor in the VGA terminal 
    void enable_cursor(unsigned char cursor_start, unsigned char cursor_end)
    {
	    outb(0x3D4, 0x0A);
	    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	    outb(0x3D4, 0x0B);
	    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
    }

    // Disables the cursor in the VGA terminal 
    void disable_cursor(void)
    {
        outb(0x3D4, 0x0A);
    	outb(0x3D5, 0x20);
    }

    // Changes the terminal cursor position
    void set_cursor_pos(int x, int y)
    {
    	uint16_t pos = y * VGA_WIDTH + x;
    
    	outb(0x3D4, 0x0F);
    	outb(0x3D5, (unsigned char) (pos & 0xFF));
    	outb(0x3D4, 0x0E);
    	outb(0x3D5, (unsigned char) ((pos >> 8) & 0xFF));
    }

    void update_cursor(void)
	{
        set_cursor_pos(terminal_column,terminal_row);
    }

    static inline unsigned char vga_colour(enum vga_colour fg, enum vga_colour bg) 
    {
    	return fg | bg << 4;
    }

    // Setup the VGA output
    void initialize(uint8_t colour) 
    {
	    terminal_row = 0;
	    terminal_column = 0;
	    terminal_colour = colour;
	    terminal_buffer = (uint16_t*)0xB8000;
	    for (size_t y = 0; y < VGA_HEIGHT; y++)
		{
	    	for (size_t x = 0; x < VGA_WIDTH; x++)
			{
	    		const size_t index = y * VGA_WIDTH + x;
	    		terminal_buffer[index] = vga_entry(' ', terminal_colour);
	    	}
	    }
    }

    void vga_terminal() { initialize(vga_colour(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK)); };
    void vga_terminal(enum vga_colour fg, enum vga_colour bg) { initialize(vga_colour(fg, bg)); }
    void vga_terminal(unsigned char colour) { initialize(colour); }

    void set_colour(enum vga_colour fg, enum vga_colour bg) { terminal_colour = vga_colour(fg, bg); }
    void set_colour(enum vga_colour colour)                { terminal_colour = colour;             }


    // Clears any text on the screen
    void clear(void){
	    terminal_row = 0;
	    terminal_column = 0;

	    for (size_t y = 0; y < VGA_HEIGHT; y++) {
	    	for (size_t x = 0; x < VGA_WIDTH; x++) {
	    		const size_t index = y * VGA_WIDTH + x;
	    		terminal_buffer[index] = vga_entry(' ', terminal_colour);
	    	}
	    }
    }

    void write(const char* data) 
    {
	    for (size_t i = 0; i < strlen(data); i++){
	    	char c = data[i];
            if(c != '\n'){
                if(terminal_row == VGA_WIDTH){ clear(); }
	            const size_t index = terminal_row * VGA_WIDTH + terminal_column;
	            terminal_buffer[index] = vga_entry(c, terminal_colour);
            }
	        if (++terminal_column == VGA_WIDTH) {
	    	    terminal_column = 0;
	    	if (++terminal_row == VGA_HEIGHT){
	    		terminal_row = 0;
	    	}
	        }
	        if(c == '\n'){
	    	    terminal_row= terminal_row + 1;
	    	    terminal_column = 0;
	        }
	        update_cursor();
        }
    }
	void write(char data) { write(&data); }

};