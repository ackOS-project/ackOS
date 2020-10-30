#pragma once

#include <stdarg.h>
#include <vector.h>
#include "kernel/kernel.h"

char* itoa(int value, char* str, int base)
{
    char* rc;
    char* ptr;
    char* low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

void printf(const char* string, ...) 
{
    size_t format_length;
    size_t string_length = strlen(string);

    for(int i = 0; i < string_length; i++)
    {
        if(string[i] == '%')
        {
            format_length++;
        }
    }

    va_list vl;
    va_start(vl, format_length);
    
    auto look_ahead = [&](size_t inc = 1) 
    {
        int total = string_length + inc;
        if(total < string_length)
        {
            return string[total];
        }
        else
        {
            return ' ';
        }
        
    };

    std::vector<const char*> tokens;
    int current_format = 0;
    for(int i = 0; i < string_length; i++)
    {
        if(string[i] == '%')
        {
            if(look_ahead() == 's')
            {
                tokens.push_back(va_arg(vl, const char*));
                vga_terminal::write(tokens[current_format]);

                current_format++;
            }
        }
    }

    va_end(vl);
}

void printf(const char* str)
{
    vga_terminal::write(str);
}

int sqrt(int x)
{
    int quotient = 0;
    int i = 0;

    bool resultfound = false;
    while (resultfound == false) {
        if (i*i == x) {
          quotient = i;
          resultfound = true;
        }
        i++;
    }
    return quotient;
}