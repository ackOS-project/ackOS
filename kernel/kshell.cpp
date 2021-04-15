#include "kernel/drivers/legacy_vga.h"
#include "kernel/drivers/keyboard.h"
#include "kernel/drivers/serial.h"
#include <cstring>

#define PORT SERIAL_COM1
#define COMMAND_MAX 255
#define ALPHA "abcdefghijklmnopqrsuvwxyzABCDEFGHIJKLMNOPQRSUVWXYZ$"

void keyboard_interrupt_handler()
{
    legacy_vga::write("x");
}

char command_buffer[COMMAND_MAX];

bool parse_is_alpha(char c)
{
    for(int i = 0; i < strlen(ALPHA); i++)
    {
        if(c == ALPHA[i])
        {
            return true;
        }
    }

    return false;
}

void parse_command(const char* command)
{
    size_t size = strlen(command);

    for(int i = 0; i < size; i++)
    {
        if(parse_is_alpha(command[i]))
        {
            
        }
    }
}

void ksh_prompt()
{
    char* command_buffer_ptr = command_buffer;

    legacy_vga::write("$ ");
    serial::print(PORT, "$ ");

    for(;;)
    {
        char c = serial::getc(PORT);

        switch (c)
        {
        case '\r':
            serial::putc(PORT, '\n');
            parse_command(command_buffer);
            command_buffer_ptr = command_buffer;

            break;
        case 127: // backspace
            serial::print(PORT, "\b \b");
            break;

        default:
            *command_buffer_ptr = c;
            command_buffer_ptr++;
            break;
        }

        serial::putc(PORT, c);
    }
}

void ksh_start()
{
    legacy_vga::write("Started kernel shell\n\n");

    ksh_prompt();
}
