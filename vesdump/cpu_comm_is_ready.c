#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "include/cpu_comm.h"

int cpu_comm_is_ready(int fd)
{
    int ret;
    CPUStateInfo info;

    if (-1 == fd)
        goto NOT_READY;

    info.cpuID = CPU_MAX_NONE;
    info.act   = CPU_STATE_APP_READY_GET;

    ret = ioctl_b(fd, CPU_COMM_CPU_STATE, &info);
    if (0 != ret)
        goto NOT_READY;

    if (info.state != CPU_STATE_READY)
        goto NOT_READY;

    return 1;

NOT_READY:
    return 0;
}

