#include <stdarg.h>
#include "kernel.h"

char * itoa( int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
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
    // this is kinda broken and i do not know why

    int size = strlen(string);
    char* str = (char*)string;
    int num_format=0;
    for(int i = 0; i < size; i++){
        if(str[i] == '%' && str[i + 1] != ' '){
            num_format++;
        }
    }
    va_list argptr;
    int count;
    void* format[num_format];

    va_start(argptr, num_format);
    while(count < num_format){
        format[count] = va_arg(argptr, void*);
        count++;
    }
    va_end(argptr);
    bool skip = false;
    int c = 0;
    for(int i = 0; i < size; i++){
        if(c <= num_format){
            if(str[i] == '%' && str[i + 1] != ' '){
                if(str[i + 1] == 's'){
                    vga_terminal::write((const char*)format[c]);
                }
                if(str[i + 1] == 'i'){
                    char buf[20];
                    vga_terminal::write(itoa((int)format[c], buf, 10));
                }
                c++;
            }else{ if(!skip){ vga_terminal::write(str[i]);} skip=false; }
            if(str[i] == '%' && str[i + 1] != ' '){ skip = true; }
        }else { vga_terminal::write( str[i]); }

    }
}
void printf(const char* str)
{
    vga_terminal::write(str);
}