#include <stdio.h>
#include <string.h>

#include <vs/cpu_comm.h>

void cpu_comm_get_mem_map_info(void)
{
    CPUCommShMemMap_t share;

    memset(&share, 0, sizeof(CPUCommShMemMap_t));

    share.index = 0;

    ioctl(g_comm_fd, CPU_COMM_GET_MEM_MAP_INFO, &share);
   
    printf( "CPU_COMM_GET_MEM_MAP_INFO:\n"
            "\tMemVirtAddr:     %08X\n"
            "\tMemPhyAddr:      %08X\n"
            "\tMemSize:         %08X\n"
            "\tMemKernVirtAddr: %08X\n",
            (unsigned int)share.MemVirtAddr,
            (unsigned int)share.MemPhyAddr,
            (unsigned int)share.MemSize,
            (unsigned int)share.MemKernVirtAddr
            );
}
