#include "arch/x86_64/features/io.h"
#include "arch/x86_64/features/ps2_keyboard.h"

namespace x86_64
{
    static char keyboard_map[(uint32_t)keyboard_scancode::COUNT] =
    {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
        0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
        0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
    };

    keyboard_scancode kbps2_get_scancode()
    {
        if(inb(0x64) & 0x1)
        {
            return (keyboard_scancode)inb(0x60);
        }

        return keyboard_scancode::INVALID;
    }

    char kbps2_scancode_to_ascii(keyboard_scancode scancode)
    {
        uint32_t sc = (uint32_t)scancode;

        if(kbps2_get_key_state(scancode) == key_state::RELEASED)
        {
            sc -= 0x80; /* convert RELEASED scancode to a normal scancode */
        }

        return keyboard_map[sc];
    }

    key_state kbps2_get_key_state(keyboard_scancode scancode)
    {
        uint32_t sc = (uint32_t)scancode;

        if(sc > 0x58)
        {
            return key_state::RELEASED;
        }
        else
        {
            return key_state::DOWN;
        }
    }
}
