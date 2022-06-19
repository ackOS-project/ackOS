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

static FILE _stdin = { .flags = O_RDONLY, .offset = 0, .buff = nullptr, .buff_size = 0, .fd = STDIN_FILENO, .status = 0 };
static FILE _stdout = { .flags = O_WRONLY, .offset = 0, .buff = nullptr, .buff_size = 0, .fd = STDOUT_FILENO, .status = 0 };
static FILE _stderr = { .flags = O_WRONLY, .offset = 0, .buff = nullptr, .buff_size = 0, .fd = STDERR_FILENO, .status = 0  };

FILE* stdin = &_stdin;
FILE* stdout = &_stdout;
FILE* stderr = &_stderr;

FILE* fdopen(int fd, const char* mode)
{
    FILE* file = (FILE*)malloc(sizeof(FILE));

    file->flags = O_RDWR; // TODO: implement mode parsing
    file->offset = 0;
    file->buff = nullptr;
    file->buff_size = 0;
    file->fd = fd;
    file->status = 0;

    return file;
}

FILE* fopen(const char *filename, const char *mode)
{
    int fd = open(filename, O_RDWR);

    if(fd < 0) return nullptr;

    return fdopen(fd, mode);
}

size_t fread(void* buff, size_t size, size_t count, FILE* file)
{
    if(size == 0 || count == 0 || file == nullptr)
    {
        return 0;
    }

    size_t bytes = count * size;
    ssize_t bytes_read = read(file->fd, buff, bytes);

    if(bytes_read < 0)
    {
        return 0;
    }

    return bytes_read;
}

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

int fgetc(FILE* file)
{
    char c;

    return (fread(&c, 1, 1, file) == 1) ? (int)c : EOF;
}

int fputs(const char* str, FILE* file)
{
    fwrite(str, sizeof(char), strlen(str), file);

    return 0;
}

int putc(int c, FILE* file)
{
    return fputc(c, file);
}

int getc(FILE* file)
{
    return fgetc(file);
}

int putchar(int c)
{
    return fputc(c, stdout);
}

int getchar()
{
    return fgetc(stdin);
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
/*
int vsnprintf(char* buff, size_t n, const char* fmt, va_list args)
{
    int length = 0;
    size_t fmt_len = strlen(fmt);

    while(char ch = *fmt++)
    {
        if(n != 0 && length > n)
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
}*/

enum
{
    PRINTF_STATE_NORMAL,
    PRINTF_STATE_FLAG,
    PRINTF_STATE_WIDTH,
    PRINTF_STATE_LENGTH,
    PRINTF_STATE_SPECIFIER
};

enum
{
    PRINTF_FLAG_DEFAULT,
    PRINTF_FLAG_LEFT_JUSTIFY,
    PRINTF_FLAG_PAD_ZEROES,
    PRINTF_FLAG_SHOW_SIGN,
    PRINTF_FLAG_ALTERNATE_FORM
};

enum
{
    PRINTF_LEN_DEFAULT,
    PRINTF_LEN_SHORT,
    PRINTF_LEN_SHORT_SHORT,
    PRINTF_LEN_LONG,
    PRINTF_LEN_LONG_LONG
};

template<typename T>
size_t count_int_ch(T value, int base)
{
    if(value == 0)
    {
        return 1;
    }

    size_t count = 0;

    if(value < 0 && base == 10)
    {
        count++;
    }

    while(value)
    {
        value /= base;
        count++;
    }

    return count;
}

static int dvsnprintf(char* buff, size_t n, const char* fmt, bool dry_run, va_list args)
{
    char* buff_start = buff;
    int written_len = 0;
    int state = PRINTF_STATE_NORMAL;
    int flag = PRINTF_FLAG_DEFAULT;
    int width = 0;
    int length = PRINTF_LEN_DEFAULT;

    #define PRINTF_GETSPACE() (n - (size_t)((uintptr_t)buff - (uintptr_t)buff_start))
    #define PRINTF_TRUNCATE(__len) (PRINTF_GETSPACE() < ((size_t)(__len)) ? PRINTF_GETSPACE() : ((size_t)(__len)))
    #define PRINTF_APPENDCH(__ch) \
        if(PRINTF_GETSPACE() > 0 && !dry_run) \
        { \
            *(buff++) = (__ch); \
        } \
        written_len++;
    #define PRINTF_JUSTIFYR(__len) \
        if(flag != PRINTF_FLAG_LEFT_JUSTIFY) \
        { \
            size_t len = width >= (__len) ? width - (__len) : 0; \
            size_t pad_len = PRINTF_TRUNCATE(len); \
            if(!dry_run) memset(buff, flag == PRINTF_FLAG_PAD_ZEROES ? '0' : ' ', pad_len); \
            buff += pad_len; \
            written_len += len; \
        }

    while(*fmt)
    {
        switch(state)
        {
            case PRINTF_STATE_NORMAL:
                if(*fmt == '%')
                {
                    state = PRINTF_STATE_FLAG;
                    fmt++;

                    continue;
                }
                else
                {
                    PRINTF_APPENDCH(*fmt);
                    fmt++;
                }

                break;

            case PRINTF_STATE_FLAG:
                switch(*fmt)
                {
                    case '-':
                        flag = PRINTF_FLAG_LEFT_JUSTIFY;
                        fmt++;

                        break;
                    case '0':
                        flag = PRINTF_FLAG_PAD_ZEROES;
                        fmt++;

                        break;
                    case '+':
                        flag = PRINTF_FLAG_SHOW_SIGN;
                        fmt++;

                        break;
                    case '#':
                        flag = PRINTF_FLAG_ALTERNATE_FORM;
                        fmt++;

                        break;
                    default:
                        break;
                }

                break;
            case PRINTF_STATE_WIDTH:
            {
                if(*fmt >= '1' && *fmt <= '9')
                {
                    width = atoi(fmt);
                    fmt += count_int_ch(width, 10);
                }
                else if(*fmt == '*')
                {
                    width = va_arg(args, int);
                    fmt++;
                }

                break;
            }
            case PRINTF_STATE_LENGTH:
                switch(*fmt)
                {
                    case 'h':
                        length = PRINTF_LEN_SHORT;

                        if(*(fmt + 1) == 'h')
                        {
                            length = PRINTF_LEN_SHORT_SHORT;

                            fmt++;
                        }

                        fmt++;

                        break;

                    case 'l':
                        length = PRINTF_LEN_LONG;

                        if(*(fmt + 1) == 'l')
                        {
                            length = PRINTF_LEN_LONG_LONG;

                            fmt++;
                        }

                        fmt++;

                        break;
                    default:
                        break;
                }

                break;
            
            case PRINTF_STATE_SPECIFIER:
            {
                switch(*fmt)
                {
                    case 'c':
                    {
                        char c = (char)va_arg(args, int);

                        PRINTF_JUSTIFYR(1);
                        PRINTF_APPENDCH(c);

                        break;
                    }
                    case 's':
                    {
                        char* str = va_arg(args, char*);

                        if(str)
                        {
                            size_t str_len = strlen(str);
                            size_t trunc_len = PRINTF_TRUNCATE(str_len);

                            PRINTF_JUSTIFYR(str_len);

                            if(!dry_run)
                            {
                                strncpy(buff, str, trunc_len);
                            }

                            buff += trunc_len;
                            written_len += str_len;
                        }

                        break;
                    }
                    case '%':
                    {
                        PRINTF_JUSTIFYR(1);
                        PRINTF_APPENDCH('%');

                        break;
                    }
                    #define PRINTF_INT_SPEC(__type, __vtype, __func, __base) \
                        __type value = (__type)va_arg(args, __vtype); \
                        size_t est_size = count_int_ch(value, __base); \
                        PRINTF_JUSTIFYR(est_size); \
                        size_t trunc_len = PRINTF_TRUNCATE(est_size); \
                        char tmp[est_size + 1]; \
                        __func(value, tmp, (__base)); \
                        if(!dry_run) strncpy(buff, tmp, trunc_len); \
                        buff += trunc_len; \
                        written_len += est_size;
                    #define PRINTF_LEN_SPEC(__base, __signness) \
                        switch(length) \
                        { \
                            case PRINTF_STATE_NORMAL: \
                            { \
                                if(__signness == true) \
                                { \
                                    PRINTF_INT_SPEC(int32_t, int32_t, itoa, (__base)); \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(uint32_t, uint32_t, utoa, (__base)); \
                                } \
                                break; \
                            } \
                            case PRINTF_LEN_SHORT: \
                            { \
                                if(__signness == true) \
                                { \
                                    PRINTF_INT_SPEC(int16_t, int32_t, itoa, (__base)); \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(uint16_t, uint32_t, utoa, (__base)); \
                                } \
                                break; \
                            } \
                            case PRINTF_LEN_SHORT_SHORT: \
                            { \
                                if(__signness == true) \
                                { \
                                    PRINTF_INT_SPEC(int8_t, int32_t, itoa, (__base)); \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(uint8_t, uint32_t, utoa, (__base)); \
                                } \
                                break; \
                            } \
                            case PRINTF_LEN_LONG_LONG: \
                            case PRINTF_LEN_LONG: \
                            { \
                                if(__signness == true) \
                                { \
                                    PRINTF_INT_SPEC(int64_t, int64_t, ltoa, (__base)); \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(uint64_t, uint64_t, ultoa, (__base)); \
                                } \
                                break; \
                            } \
                            default: \
                                break; \
                        }

                    case 'i':
                    case 'd':
                    {
                        PRINTF_LEN_SPEC(10, true);

                        break;
                    }
                    case 'u':
                    {
                        PRINTF_LEN_SPEC(10, false);

                        break;
                    }
                    case 'o':
                    {
                        PRINTF_LEN_SPEC(8, false);

                        break;
                    }
                    case 'x':
                    {
                        PRINTF_LEN_SPEC(16, false);

                        break;
                    }
                    case 'p':
                    {
                        uintptr_t value = (uintptr_t)va_arg(args, void*);
                        size_t est_size = count_int_ch(value, 16);

                        if(flag != PRINTF_FLAG_PAD_ZEROES)
                        {
                            PRINTF_JUSTIFYR(est_size + 2);
                        }

                        PRINTF_APPENDCH('0');
                        PRINTF_APPENDCH('x');

                        if(flag == PRINTF_FLAG_PAD_ZEROES)
                        {
                            PRINTF_JUSTIFYR(est_size);
                        }

                        size_t trunc_len = PRINTF_TRUNCATE(est_size);
                        char tmp[est_size + 1];

                        ultoa(value, tmp, 16);

                        if(!dry_run)
                        {
                            strncpy(buff, tmp, trunc_len);
                        }

                        buff += trunc_len;
                        written_len += est_size;

                        break;
                    }
                    default:
                        break;
                }

                fmt++;
                break;
            }

            default:
                break;
        }

        if(state != PRINTF_STATE_NORMAL && state < PRINTF_STATE_SPECIFIER)
        {
            state++;
        }
        else
        {
            state = PRINTF_STATE_NORMAL;
            flag = PRINTF_FLAG_DEFAULT;
            width = 0;
            length = PRINTF_LEN_DEFAULT;
        }
    }

    if(!dry_run) *buff = '\0';

    #undef PRINTF_APPENDCH
    #undef PRINTF_JUSTIFYR
    #undef PRINTF_TRUNCATE
    #undef PRINTF_INT_SPEC
    #undef PRINTF_LEN_SPEC

    return written_len;
}

int vsnprintf(char* buff, size_t n, const char* fmt, va_list args)
{
    return dvsnprintf(buff, n, fmt, false, args);
}

int vsprintf(char* buff, const char* fmt, va_list args)
{
    return vsnprintf(buff, 0, fmt, args);
}

int snprintf(char* buff, size_t n, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int length = vsnprintf(buff, n, fmt, args);

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
    va_list new_list;
    va_copy(new_list, args);
    size_t size = dvsnprintf(nullptr, 10, fmt, true, new_list);

    char buff[size];
    int length = vsnprintf(buff, size, fmt, args);

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

int vsscanf(const char* buffer, const char* format, va_list args)
{
    size_t buff_size = strlen(buffer);
    size_t fmt_size = strlen(format);
    int n = 0;
    int buff_index = 0;

    for(int i = 0; i < fmt_size && i < buff_size; i++)
    {
        if(format[i] == '%')
        {
            char c = format[i];
            n++;
            i++;
            
            if(c == 'c')
            {
                char* output = va_arg(args, char*);
                
                *output = c;
            }
            else if(c == 's')
            {
                char* output = va_arg(args, char*);
            
                while(buff_index < buff_size)
                {
                    if(buffer[buff_index] == ' ')
                    {
                        break;
                    }

                    *output++ = buffer[buff_index];
                
                    buff_index++;
                }
            }
            else if(c == 'd')
            {
                int* output = va_arg(args, int*);
            
                *output = strtol(&buffer[buff_index], NULL, 10);
            }
            else if(c == 'x')
            {
                int* output = va_arg(args, int*);

                *output = strtol(&buffer[buff_index], NULL, 16);
            }
            else if(c == '%')
            {
                n--;

                if(buffer[buff_index] != c)
                {
                    break;
                }
            }
        }
        else if(buffer[buff_index] != format[i])
        {
            break;
        }
        
        buff_index++;
    }
    
    return n;
}

int sscanf(const char* buffer, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    int n = vsscanf(buffer, format, args);

    va_end(args);
    
    return n;
}
