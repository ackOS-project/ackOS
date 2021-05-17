#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include <liback/syscalls.h>

struct FILE
{
    int flags;
    off_t offset;
    char* buff;
    int buff_size;
    int fd;
    int status;
};

FILE _stdin = { .flags = O_RDONLY, .offset = 0, .buff = nullptr, .buff_size = 0, .fd = STDIN_FILENO, .status = 0 };
FILE _stdout = { .flags = O_WRONLY, .offset = 0, .buff = nullptr, .buff_size = 0, .fd = STDOUT_FILENO, .status = 0 };
FILE _stderr = { .flags = O_WRONLY, .offset = 0, .buff = nullptr, .buff_size = 0, .fd = STDERR_FILENO, .status = 0  };

FILE* stdin = &_stdin;
FILE* stdout = &_stdout;
FILE* stderr = &_stderr;

size_t fwrite(const void* buff, size_t size, size_t count, FILE* file)
{
    size_t bytes = count * size;
    size_t bytes_written = write(file->fd, buff, bytes);

    if(bytes == bytes_written)
    {
        return count;
    }

    return 0;
}

int fputc(int c, FILE* file)
{
    fwrite((const void*)&c, 1, 1, file);

    return c;
}

int fputs(const char* str, FILE* file)
{
    fwrite(str, 1, strlen(str), file);

    return 0;
}

int putc(int c, FILE* file)
{
    return fputc(c, file);
}

int putchar(int c)
{
    return fputc(c, stdout);
}

int puts(const char* str)
{
    int r = 0;

    for(r = 0; r < strlen(str); r++)
    {
        putchar(str[r]);
    }

    if(r > 0)
    {
        putchar('\n');

        r++;
    }

    return r;
}

int vsnprintf(char* buff, size_t n, const char* fmt, va_list args)
{
    int length = 0;
    size_t fmt_len = strlen(fmt);

    while(char ch = *fmt++)
    {
        if(n != 0 && length == n)
        {
            break;
        }

        if(ch == '%')
        {
            switch(ch = *fmt++)
            {
                case '%':
                {
                    buff[length] = '%';
                    length++;

                    break;
                }
                case 'c':
                {
                    char c = va_arg(args, int);

                    if(c != 0)
                    {
                        buff[length] = c;
                        length++;
                    }

                    break;
                }
                case 's':
                {
                    char* str = va_arg(args, char*);

                    for(int i = 0; i < strlen(str); i++)
                    {
                        buff[length + i] = str[i];
                    }
                    length += strlen(str);

                    break;
                }
                case 'd':
                {
                    int interger = va_arg(args, int);

                    char itoa_buff[128];
                    itoa(interger, itoa_buff, 10);

                    for(int i = 0; i < strlen(itoa_buff); i++)
                    {
                        buff[length + i] = itoa_buff[i];
                    }
                    length += strlen(itoa_buff);

                    break;
                }
                case 'x':
                {
                    int interger = va_arg(args, uint32_t);

                    char itoa_buff[128];
                    utoa(interger, itoa_buff, 16);

                    for(int i = 0; i < strlen(itoa_buff); i++)
                    {
                        buff[length + i] = itoa_buff[i];
                    }
                    length += strlen(itoa_buff);

                    break;
                }
                case 'l':
                {
                    if((fmt_len - strlen(fmt)) < fmt_len)
                    {
                        ch = *fmt++;

                        switch(ch)
                        {
                            default:
                            {
                                buff[length] = '?';
                                length++;
                                ch = *fmt--;

                                break;
                            }
                        }
                    }
                    else
                    {
                        buff[length] = '?';
                        length++;
                    }

                    break;
                }
                default:
                {
                    buff[length] = '?';
                    length++;
                }
            }

        }
        else
        {
            buff[length] = ch;
            length++;
        }
    }

    buff[length] = '\0';

    return length;
}

int vsprintf(char* buff, const char* fmt, va_list args)
{
    return vsnprintf(buff, 0, fmt, args);
}

int snprintf(char* buff, size_t n, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int length = vsnprintf(buff, 0, fmt, args);

    va_end(args);

    return length;
}

int sprintf(char* buff, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int length = vsprintf(buff, fmt, args);

    va_end(args);

    return length;
}

int vfprintf(FILE* file, const char* fmt, va_list args) 
{
    char buff[512];
    int length = vsnprintf(buff, 512, fmt, args);

    fputs(buff, file);

    return length;
}

int fprintf(FILE* file, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int length = vfprintf(file, fmt, args);

    va_end(args);

    return length;
}

int vprintf(const char* fmt, va_list args) 
{
    return vfprintf(stdout, fmt, args);
}

int printf(const char* fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    int length = vprintf(fmt, args);

    va_end(args);

    return length;
}
