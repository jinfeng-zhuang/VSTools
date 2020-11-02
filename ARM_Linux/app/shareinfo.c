#include <stdio.h>

#include <pthread.h>

#include <vs/mmap.h>
#include <vs/pman.h>
#include <vs/cpu_comm.h>

#include <zstack/debug.h>

pthread_mutexattr_t ma;

int main(int argc, char *argv[])
{
    unsigned int length = 0;
    unsigned int *sharebuf = NULL;

    if (0 != mem_map_init())
        return -1;

    pman_disable();

    Trid_Util_CPUComm_Init();

    ShareMemMap();

    hexdump("@0x78000000", 0x78000000, 16);

#if 0
    cpu_comm_get_mem_map_info();

    length = 1;

    sharebuf = Trid_Util_CPUComm_CreatShareInfo(&length);

    Trid_Util_CPUComm_CreatSingleInfo(0x9, 24);

    unsigned int CurInfoID = 0;
    unsigned int CurInfoLen = 0;
    unsigned int CurPos = 0;
    unsigned int ShareBufLen = 0;

    ShareBufLen = length;

    do {
        CurInfoID = *(sharebuf + (CurPos >> 2));
        if (CurInfoID == 0)
            break;
        CurInfoLen = *(sharebuf + ((CurPos + 4) >> 2));
        CurPos += CurInfoLen + 4 + 4;  // Plus ID and Len

        printf("ID %x Len %x\n", CurInfoID, CurInfoLen);

    } while (CurPos < (ShareBufLen - 4 - 4));

    printf("errno %d\n", errno);
#endif

    pthread_mutex_t *mutex = mem_map(0x0dc00000, 4096);

#if 0
    printf("support ? %d, errno %d\n", sysconf(_POSIX_THREAD_PROCESS_SHARED), errno);

    pthread_mutexattr_init(&ma);
    pthread_mutexattr_setpshared(&ma, PTHREAD_PROCESS_SHARED);
#endif

    hexdump("before mutex init", mutex, sizeof(pthread_mutex_t));

    pthread_mutex_init(mutex, NULL);

    hexdump("before mutex lock", mutex, sizeof(pthread_mutex_t));

    pthread_mutex_lock(mutex);
    pthread_mutex_unlock(mutex);

    hexdump("after mutex unlock", mutex, sizeof(pthread_mutex_t));

    return 0;
}

