#include <stdio.h>
#include <getopt.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include <sys/mman.h>
#include <fcntl.h>

#include <vs/mmap.h>
#include <vs/pman.h>

#include <zstack/debug.h>

int main(int argc, char *argv[])
{
    int len = sizeof(pthread_mutex_t);
    int fd = 0;
    unsigned char *share_buf = NULL;

    if (0 != mem_map_init()) {
        TRACE("\n");
        goto error;
    }

    pman_disable();

    TRACE("mutex len %d\n", len);

    fd = open("TEST", O_RDWR);
    if (-1 == fd) {
        TRACE("errno %d\n", errno);
        return -1;
    }

    share_buf = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    TRACE("share_buf %p\n", share_buf);

    hexdump("@share_buf", share_buf, 32);

    pthread_mutex_t *mutex = share_buf;

    TRACE("\n");
    pthread_mutex_init(mutex, NULL);
    TRACE("\n");
    pthread_mutex_lock(mutex);
    TRACE("\n");
    pthread_mutex_unlock(mutex);
    TRACE("\n");

    return 0;
}

