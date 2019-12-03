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

#if 1
extern int g_comm_fd;
void ShareMemMap(void)
{
    unsigned int addr_base_virt = 0;

    if (g_comm_fd == -1) {
        printf("hidtv not inited\n");
        return;
    }

    addr_base_virt = (unsigned int)mmap(0x78000000, 0x1700000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, g_comm_fd, 0x0e900000);
    if (0x78000000 != addr_base_virt) {
        printf("mmap return %x errno %d\n", addr_base_virt, errno);
        return;
    }

    printf("mmap 0xe9000000 to 0x78000000\n");
}
#endif

unsigned int ReadWord(unsigned int address)
{
    unsigned int value;
    unsigned int page_size = 0;
    unsigned int addr_base = 0;
    unsigned int addr_offset = 0;
    unsigned int addr_base_virt = 0;

    if (hidtv_fd == -1) {
        printf("hidtv not inited\n");
        return 0;
    }

    page_size = getpagesize();

    addr_base = address & (~(page_size - 1));
    addr_offset = address & (page_size - 1);

    addr_base_virt = (unsigned int)mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, hidtv_fd, addr_base);
    if (-1 == addr_base_virt) {
        printf("mmap failed %d\n", errno);
        return 0;
    }

    value = *((volatile unsigned int *)(addr_base_virt + addr_offset));

    munmap((void *)addr_base_virt, page_size);

    return value;
}

void WriteWord(unsigned int address, unsigned int value)
{
    unsigned int page_size = 0;
    unsigned int addr_base = 0;
    unsigned int addr_offset = 0;
    unsigned int addr_base_virt = 0;

    if (hidtv_fd == -1) {
        printf("hidtv not inited\n");
        return;
    }

    page_size = getpagesize();

    addr_base = address & (~(page_size - 1));
    addr_offset = address & (page_size - 1);

    addr_base_virt = (unsigned int)mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, hidtv_fd, addr_base);
    if (-1 == addr_base_virt) {
        printf("mmap failed %d\n", errno);
        return;
    }

    *((volatile unsigned int *)(addr_base_virt + addr_offset)) = value;

    munmap((void *)addr_base_virt, page_size);
}

unsigned char *mem_map(unsigned int addr, unsigned int size)
{
    unsigned int realsize;

    if (hidtv_fd == -1) {
        printf("hidtv not inited\n");
        return NULL;
    }

    realsize = (addr & 0xFFFF) + size;

    if (realsize < 0x10000)
        realsize = 0x10000;

    //unsigned char *memory = mmap(NULL, realsize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, hidtv_fd, addr & 0xFFFF0000);
    unsigned char *memory = mmap(NULL, realsize, PROT_READ | PROT_WRITE, MAP_SHARED, hidtv_fd, addr & 0xFFFF0000);
    if (-1 == (unsigned int)memory) {
        printf("mmap failed %d\n", errno);
        return NULL;
    }

    return memory + (addr & 0xFFFF);
}

