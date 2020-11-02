unsigned char inb(unsigned short int port)
{
    unsigned char rv;
    __asm__ __volatile__("inb %1, %0"
                         : "=a"(rv)
                         : "dN"(port));
    return rv;
}

void outb(unsigned short int port, unsigned char data)
{
    __asm__ __volatile__("outb %1, %0"
                         :
                         : "dN"(port), "a"(data));
}

void outw(unsigned short int port, unsigned short int data)
{
    asm volatile("outw %%ax, %%dx"
                 :
                 : "d"(port), "a"(data));
}
