#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#include "crc.h"
#include "log.h"

#define SAMPLE_US   (1000*1000) // about 400KB

unsigned int channel;

typedef unsigned long long trid_uint64;

static char buffer[1024];

int main(int argc, char *argv[])
{
    int ret;
    unsigned int ddr_addr;
    unsigned int virt_addr;
    triSlavePrint_t *log;
    unsigned int rd_idx;
    unsigned int wr_idx;

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
        ddr_addr = comm_get_addr(8 | 1<<6);
    }
    else if (channel == 0) {
        ddr_addr = comm_get_addr(8);
    }
    else {
        return 0;
    }

    printf("ddr_addr = %x\n", ddr_addr);

    // Attention, should > 128KB, otherwise cause bus error
    virt_addr = (unsigned int)mem_map(ddr_addr & 0xFFFF0000, 0x100000) + (ddr_addr & 0x0000FFFF);

    log = (triSlavePrint_t *)virt_addr;

    rd_idx = log->wr_idx & (log->total_num - 1);

    while (1) {

        wr_idx = log->wr_idx & (log->total_num - 1);
        rd_idx = rd_idx & (log->total_num - 1);

        if (rd_idx == wr_idx) {
            usleep(100*1000);
            continue;
        }

        log->message[rd_idx][255] = '\0';
        printf("%s", log->message[rd_idx]);
        rd_idx++;
    }
    
    return 0;
}

