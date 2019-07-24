#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "include/cpu_comm.h"

static const char *devname = "/dev/cpu_comm";

int g_comm_fd = -1;

static CPUCommShMemMap_t ShMemMap[2];

int Trid_Util_CPUComm_Init(void)
{
    unsigned int virtaddr;

    if (-1 != g_comm_fd)
        goto END;

    g_comm_fd = open(devname, O_RDWR | O_TRUNC, 0660);
    if (-1 == g_comm_fd) {
        printf("can't open %s\n", devname);
        goto IOERROR;
    }

    ioctl_b(g_comm_fd, CPU_COMM_GET_MEM_MAP_INFO, &ShMemMap[0]);
    ioctl_b(g_comm_fd, CPU_COMM_GET_MEM_MAP_INFO, &ShMemMap[1]);

    if ((0 == ShMemMap[0].MemPhyAddr) || (0 == ShMemMap[1].MemPhyAddr))
        goto IOERROR;

    printf("ShMemMap[0]: phy %x virt %x size %x\n", ShMemMap[0].MemPhyAddr, ShMemMap[0].MemVirtAddr, ShMemMap[0].MemSize);
    printf("ShMemMap[1]: phy %x virt %x size %x\n", ShMemMap[1].MemPhyAddr, ShMemMap[1].MemVirtAddr, ShMemMap[1].MemSize);

    virtaddr = (unsigned int)mmap((void *)ShMemMap[0].MemVirtAddr, ShMemMap[0].MemSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, g_comm_fd, ShMemMap[0].MemPhyAddr);
    if (virtaddr != ShMemMap[0].MemVirtAddr)
        goto IOERROR;

    virtaddr = (unsigned int)mmap((void *)ShMemMap[1].MemVirtAddr, ShMemMap[1].MemSize, PROT_READ,              MAP_SHARED | MAP_FIXED, g_comm_fd, ShMemMap[1].MemPhyAddr);
    if (virtaddr != ShMemMap[0].MemVirtAddr)
        goto IOERROR;

END:
    return 0;

IOERROR:
    if (g_comm_fd >= 0)
        close(g_comm_fd);

    return -1;
}

