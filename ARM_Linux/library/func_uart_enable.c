#include "mmap.h"

int func_uart_enable(void)
{
    unsigned int virtaddr;

    virtaddr = mem_map(0xF500EE1D, PAGE_SIZE);
    if (-1 == virtaddr)
        return -1;

    printf("%x\n", *((volatile unsigned char *)virtaddr));

    *((volatile unsigned char *)virtaddr) = 0x11;

    return 0;
}

