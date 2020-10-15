#include <stdio.h>
#include <errno.h>

#include "mmap.h"

#define CHIP_ID_REG 0xF500e000
#define PAGE_SIZE (4<<10)

int get_chip_id(void)
{
    unsigned int chipid;

    unsigned int *memory = (unsigned int *)mem_map(CHIP_ID_REG, PAGE_SIZE);
    if (-1 == (int)memory) {
        printf("mmap failed %d\n", errno);
        return 0;
    }

    chipid = memory[0];

    return chipid;
}

