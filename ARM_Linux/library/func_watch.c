#include "mmap.h"

int func_watch(unsigned int addr)
{
    unsigned int virtaddr;

    virtaddr = mem_map(addr, PAGE_SIZE);

    while (1) {
        printf("%08X: %08x\n", *((volatile unsigned int *)addr));
        usleep(100*1000);
    }
}

