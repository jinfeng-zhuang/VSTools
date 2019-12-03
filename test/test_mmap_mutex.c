#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <vs/mmap.h>
#include <vs/pman.h>

#include <zstack/debug.h>

int main(int argc, char *argv[])
{
    unsigned char *buffer = NULL;
    pthread_mutex_t *mutex = NULL;
    int option = 0;

    if (2 != argc) {
        printf("%s [0 | 1]\n", argv[0]);
        return 0;
    }

    option = atoi(argv[1]);

    option = option ? 1 : 0;

    printf("sizeof %d\n", sizeof(pthread_mutex_t));

    if (0 != mem_map_init()) {
        TRACE("\n");
        goto error;
    }

    pman_disable();

    if (0 == option) {
        buffer = (unsigned char *)malloc(4096);
        if (NULL == buffer) {
            TRACE("\n");
            goto error;
        }
    }
    else {
        buffer = mem_map(0x0dc00000, 4096);
        if ((void*)-1 == buffer) {
            TRACE("\n");
            goto error;
        }
    }

    mutex = (pthread_mutex_t *)buffer;

    hexdump("before mutex init", mutex, sizeof(pthread_mutex_t));

    pthread_mutex_init(mutex, NULL);

    hexdump("before mutex lock", mutex, sizeof(pthread_mutex_t));

    pthread_mutex_lock(mutex);
    hexdump("mutex locking:", mutex, sizeof(pthread_mutex_t));
    pthread_mutex_unlock(mutex);

    hexdump("after mutex unlock", mutex, sizeof(pthread_mutex_t));

    return 0;

error:
    printf("error\n");

    return -1;
}

