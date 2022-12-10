static inline int isalpha(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline int isspace(int c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f' || c == '\r';
}

static inline int isdigit(int c)
{
    return c >= '0' && c <= '9';
}

static inline int isupper(int c)
{
    return c >= 'A' && c <= 'Z';
}

static inline int islower(int c)
{
    return c >= 'a' && c <= 'z';
}

static inline int isprint(int c)
{
    return c >= 0x20 && c <= 0x7e;
}

static inline int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}

static inline int iscntrl(int c)
{
    return c == 0x7f || (c >= 0 && c <= 0x1f);
}

static inline int isxdigit(int c)
{
    return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

static inline int tolower(int c)
{
    return isupper(c) ? c - 'A' + 'a' : c;
}

static inline int toupper(int c)
{
    return islower(c) ? c - 'a' + 'A' : c;
}
