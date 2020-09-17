#ifndef VS_IOCTL_H
#define VS_IOCTL_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define ERESTARTSYS 512

static inline int ioctl_b(int fd, int cmd, void *param)
{
    while (ioctl(fd, cmd, param)) {
        if (errno == ERESTARTSYS) {
            printf("ioctl failed: %d\n", errno);
            return -1;
        }
        else {
            printf("ioctl error %d\n", errno);
        }
        usleep(10*1000);
    }

    return 0;
}

#endif
