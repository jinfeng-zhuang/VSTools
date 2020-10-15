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

#define HAL_BUF_SIZE   128

#define SIZE_1MB    (1<<20)
#define SIZE_1KB    (1<<10)

struct pts_desc {
    unsigned int wp;
    unsigned int rp;
    unsigned int start;
    unsigned int end;
};

struct local_desc {
    unsigned int start;
    unsigned int end;
    unsigned int wp;
    unsigned int rp;
};

struct avmips_desc {
    unsigned int start;
    unsigned int end;
    unsigned int wp;
    unsigned int rp;
};

struct demux_desc {
    unsigned int wp;
    unsigned int rp;
    unsigned int start;
    unsigned int end;
};

int g_dtv_flag;
unsigned int *g_es_descriptor;
unsigned int *g_pts_descriptor;
unsigned char *g_es_buffer;
unsigned char *g_pts_buffer;
unsigned int g_sample_total;
unsigned int g_sample_limit;
unsigned char hal_buf[HAL_BUF_SIZE];
sem_t sem_pts;
sem_t sem_es;

void descriptor_get(struct local_desc *desc, struct local_desc *pts_desc)
{
    if (desc != NULL) {
        if (g_dtv_flag) {
            desc->start = ((struct demux_desc *)g_es_descriptor)->start;
            desc->end   = ((struct demux_desc *)g_es_descriptor)->end;
            desc->wp    = ((struct demux_desc *)g_es_descriptor)->wp & 0x3FFFFFFF; // Bit[31:30] used for other purpose
            desc->rp    = ((struct demux_desc *)g_es_descriptor)->rp & 0x3FFFFFFF;
        }
        else {
            desc->start = ((struct avmips_desc *)g_es_descriptor)->start;
            desc->end   = ((struct avmips_desc *)g_es_descriptor)->end + 1; // [0,0x3FFF) -> [0, 0x4000)
            desc->wp    = ((struct avmips_desc *)g_es_descriptor)->wp;
            desc->rp    = ((struct avmips_desc *)g_es_descriptor)->rp;
        }
    }

    if (pts_desc != NULL) {
        pts_desc->start = ((struct pts_desc *)g_pts_descriptor)->start;
        pts_desc->end   = ((struct pts_desc *)g_pts_descriptor)->end;
        pts_desc->wp    = ((struct pts_desc *)g_pts_descriptor)->wp;
        pts_desc->rp    = ((struct pts_desc *)g_pts_descriptor)->rp;
    }
}

int main(int argc, char *argv[])
{
    int ret;
    struct local_desc desc;
    struct local_desc pts_desc;
    pthread_t pid;
    unsigned int es_desc_addr;
    unsigned int used;
    
    ret = 0;

    if (argc != 2) {
        printf("Usage:\n");
        printf("\t%s [CHANNEL]\n", argv[0]);
        return 0;
    }

    g_sample_limit = atoi(argv[1]);

    if (0 != mem_map_init())
        return -1;

    pman_disable();
    
    Trid_Util_CPUComm_Init(0);
    
    if (g_sample_limit == 1) {
        es_desc_addr = comm_get_addr(11 | 1<<6);
    }
    else if (g_sample_limit == 0) {
        es_desc_addr = comm_get_addr(11);
    }
    else {
        return 0;
    }

    if (((es_desc_addr & 0xFFFF0000) == 0x15090000) || ((es_desc_addr & 0xFFFF0000) == 0x10010000)) {
        g_dtv_flag = 1;
        es_desc_addr |= 0xF5080000;
    }

    printf("es_desc %x\n", es_desc_addr);

    g_es_descriptor = (unsigned int)mem_map(es_desc_addr & 0xFFFF0000, 0x10000) + (es_desc_addr & 0x0000FFFF);
    
    g_pts_descriptor = (unsigned int)mem_map(0xF5093F54 & 0xFFFF0000, 0x10000) + (0xF5093F54 & 0x0000FFFF);

    while (1) {
        descriptor_get(&desc, &pts_desc);

        if (desc.wp >= desc.rp)
            used = desc.wp - desc.rp;
        else
            used = desc.end - desc.rp + desc.wp - desc.start;

        printf("Desc: %x %x %x %x\n", desc.start, desc.end, desc.wp, desc.rp);
        printf("used %d, %f\n", used, ((float)used / (desc.end - desc.start)));

        usleep(100*1000);
    }

    return 0;
}

