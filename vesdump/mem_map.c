#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

static int hidtv_fd;

int mem_map_init(void)
{
    hidtv_fd = open("/dev/hidtvreg", O_RDWR);
    if (hidtv_fd < 0) {
        printf("hidtv module not ready\n");
        return -1;
    }

    return 0;
}

unsigned char *mem_map(unsigned int addr, unsigned int size)
{
    if (hidtv_fd == -1) {
        printf("hidtv not inited\n");
        return NULL;
    }

    if (addr & 0xFFFF) {
        printf("mem map MUST align to 0x10000\n");
        return NULL;
    }

    if (size < 0x10000)
        size = 0x10000;

    unsigned char *memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, hidtv_fd, addr);
    if (-1 == (unsigned int)memory) {
        printf("mmap failed %d\n", errno);
        return NULL;
    }

    return memory;
}

