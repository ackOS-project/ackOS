#pragma once

#include <cstddef>
#include <cstdint>

enum vga_colour
{
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

// A text-based terminal session in VGA mode (not supported on most hardware)
namespace legacy_vga
{
    const size_t get_vga_width();

    const size_t get_vga_height();

    // Enables the cursor in the VGA terminal
    void enable_cursor(unsigned char cursor_start, unsigned char cursor_end);

    // Disables the cursor in the VGA terminal
    void disable_cursor(void);

    // Changes the terminal cursor position
    void set_cursor_pos(int x, int y);

    void update_cursor(void);

    unsigned char vga_colour(enum vga_colour fg, enum vga_colour bg);

    // Setup the VGA output
    void initialize(uint8_t colour);

    static void initialize()
    {
        initialize(vga_colour(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK));
    };

    static void initialize(enum vga_colour fg, enum vga_colour bg)
    {
        initialize(vga_colour(fg, bg));
    }

    void set_colour(enum vga_colour fg, enum vga_colour bg);

    void set_colour(enum vga_colour colour);

    // Clears any text on the screen
    void clear();

    void putc(char c);

    void write(const char *data);
};
