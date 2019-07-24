#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "vesdump.h"

#define SAMPLE_US   (1000*1000) // about 400KB

unsigned int channel;

typedef unsigned long long trid_uint64;

static char buffer[1024];

int main(int argc, char *argv[])
{
    int ret;
    unsigned int ddr_addr;
    unsigned int virt_addr;

    ret = 0;

    if (argc != 2) {
        printf("Usage:\n");
        printf("\t%s [CHANNEL]\n", argv[0]);
        return 0;
    }

    channel = atoi(argv[1]);

    if (0 != mem_map_init())
        return -1;

    pman_disable();
    
    Trid_Util_CPUComm_Init(0);
    
    if (channel == 1) {
        ddr_addr = comm_get_addr(2 | 1<<6);
    }
    else if (channel == 0) {
        ddr_addr = comm_get_addr(2);
    }
    else {
        return 0;
    }

    printf("ddr_addr = %x\n", ddr_addr);

    virt_addr = (unsigned int)mem_map(ddr_addr & 0xFFFF0000, 0x10000) + (ddr_addr & 0x0000FFFF);

    while (1) {
        usleep(100*1000);
    }
    
    return 0;
}

