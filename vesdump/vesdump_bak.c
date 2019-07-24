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

enum {
    TYPE_BYTE,
    TYPE_HALFWORD,
    TYPE_WORD
};

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

static unsigned int get_addr(int index)
{
    int ret;
    Trid_CPUFuncCall_Param_t CallParam;
    Trid_CPUFuncCall_Return_t CallReturn;

    CallParam.Connection.targetCPU = CPU_AV;
    CallParam.Connection.targetTokenPid = NUCLEUS_PID;
    CallParam.Count = 4;
    CallParam.Param[0] = 0; // 0-read, 1-write, 2-write mask
    CallParam.Param[1] = TYPE_WORD;
    CallParam.Param[2] = 0xBADBAD00 | index; //BADBAD00 + 0B: GET_VIDEO_DEBUG_INFO_VESDESCADDR
    CallParam.Param[3] = 0;

    while (1) {
        memset(&CallReturn, 0, sizeof(Trid_CPUFuncCall_Return_t));
        ret = Trid_Util_CPUComm_CallSlave("RegTest", &CallParam, &CallReturn);
        if (ret == SYS_NOERROR) {
            if (CallReturn.Param[1] != 0) {
                break;
            }
        }
        usleep(SAMPLE_US);
        printf("ret %d %x waiting for desc ready...\n", ret, CallReturn.Param[1]);
    }

    return CallReturn.Param[1];
}

int take_record(FILE *fp, const unsigned char *buffer, const struct local_desc *desc_prev, const struct local_desc *desc_cur, const char *filename)
{
    unsigned int sample_size;
    unsigned int data_write;

    if (NULL == fp)
        return -1;
    
    if (desc_cur->wp >= desc_prev->wp) {
        sample_size = desc_cur->wp - desc_prev->wp;
        
        data_write = fwrite(&buffer[desc_prev->wp - desc_prev->start], sizeof(unsigned char), sample_size, fp);
    }
    else {
        sample_size = desc_prev->end - desc_prev->wp + desc_cur->wp - desc_cur->start;
        
        data_write = fwrite(&buffer[desc_prev->wp - desc_prev->start], sizeof(unsigned char), desc_prev->end - desc_prev->wp, fp);
        data_write += fwrite(buffer, sizeof(unsigned char), desc_cur->wp - desc_cur->start, fp);
    }
    
    if (sample_size >> 10 > 0) {
        printf("%d MB + %d KB (%s)\n", g_sample_total>>20, sample_size >> 10, filename);
    }
    else {
        printf("%d MB + %d B (%s)\n", g_sample_total>>20, sample_size & 0x3FF, filename);
    }

    g_sample_total += data_write;
    
    fflush(fp);
    
    if (data_write < sample_size) {
        printf("WARNING: disk space no enough %d %d\n", sample_size, data_write);
        return -1;
    }
    else {
        return 0;
    }
}

void *pts_dump_thread(void *args)
{
    char filename[32];
    FILE *fp;
    struct local_desc desc_prev;
    struct local_desc desc_cur;
    int ret;
    int filecnt;
    sem_t sem;
    struct timespec timeout;

    fp = NULL;
    memset(&desc_prev, 0, sizeof(struct local_desc));
    memset(&desc_cur, 0, sizeof(struct local_desc));
    ret = 0;
    filecnt = 0;

    while (1) {

        clock_gettime(CLOCK_REALTIME, &timeout);
        //timeout.tv_nsec += SAMPLE_US * 1000; // TODO
        timeout.tv_sec++;
        
        if (0 == sem_timedwait(&sem_pts, &timeout)) {

            // FILE Setup
            if (NULL != fp) {
                printf("Record %s (%d B) done.\n", filename, ftell(fp));
                fclose(fp);
                fp = NULL;
            }

            sprintf(filename, "dump-%03d.pts", filecnt);

            fp = fopen(filename, "wb");
            if (NULL == fp) {
                printf("Can't open %s\n", filename);
                break;
            }
            else {
                printf("Record %s started\n", filename);
                filecnt++;
            }

            // DESCRIPTOR setup
            descriptor_get(NULL, &desc_prev);
            desc_prev.start = desc_prev.start;
            desc_prev.end   = desc_prev.end;
            desc_prev.wp    = desc_prev.start; // TODO demux don't reset ?
            desc_prev.rp    = desc_prev.start;
        }

        descriptor_get(NULL, &desc_cur);

        ret = take_record(fp, g_pts_buffer, &desc_prev, &desc_cur, filename);
        if (-1 == ret) {
            break;
        }

        memcpy(&desc_prev, &desc_cur, sizeof(struct local_desc));
    }

    if (NULL != fp) {
        printf("Record %s (%d B) done.\n", filename, ftell(fp));
        fclose(fp);
    }
    
    fp = NULL;
    memset(&desc_prev, 0, sizeof(struct local_desc));
    memset(&desc_cur, 0, sizeof(struct local_desc));

    return NULL;
}

void *es_dump_thread(void *args)
{
    char filename[32];
    FILE *fp;
    struct local_desc desc_prev;
    struct local_desc desc_cur;
    int ret;
    int filecnt;
    sem_t sem;
    struct timespec timeout;

    fp = NULL;
    memset(&desc_prev, 0, sizeof(struct local_desc));
    memset(&desc_cur, 0, sizeof(struct local_desc));
    ret = 0;
    filecnt = 0;

    while (1) {

        clock_gettime(CLOCK_REALTIME, &timeout);
        //timeout.tv_nsec += SAMPLE_US * 1000;
        timeout.tv_sec++;
        
        if (0 == sem_timedwait(&sem_es, &timeout)) {

            // FILE Setup
            if (NULL != fp) {
                printf("Record %s (%d MB) done.\n", filename, ftell(fp)>>20);
                fclose(fp);
                fp = NULL;
            }

            sprintf(filename, "dump-%03d.es", filecnt);

            fp = fopen(filename, "wb");
            if (NULL == fp) {
                printf("Can't open %s\n", filename);
                break;
            }
            else {
                printf("Record %s started\n", filename);
                filecnt++;
            }

            // DESCRIPTOR setup
            descriptor_get(&desc_prev, NULL);
            desc_prev.start = desc_prev.start;
            desc_prev.end   = desc_prev.end;
            desc_prev.wp    = desc_prev.start; // TODO demux don't reset ?
            desc_prev.rp    = desc_prev.start;
        }

        descriptor_get(&desc_cur, NULL);

        ret = take_record(fp, g_es_buffer, &desc_prev, &desc_cur, filename);
        if (-1 == ret) {
            break;
        }

        memcpy(&desc_prev, &desc_cur, sizeof(struct local_desc));
    }

    if (NULL != fp) {
        printf("Record %s (%d MB) done.\n", filename, ftell(fp)>>20);
        fclose(fp);
    }
    
    fp = NULL;
    memset(&desc_prev, 0, sizeof(struct local_desc));
    memset(&desc_cur, 0, sizeof(struct local_desc));

    return NULL;
}

int main(int argc, char *argv[])
{
    int ret;
    struct local_desc desc;
    struct local_desc pts_desc;
    pthread_t pid;
    unsigned int es_desc_addr;
    
    ret = 0;

    if (argc != 2) {
        printf("Usage:\n");
        printf("\t%s [(MB)]\n", argv[0]);
        return 0;
    }

    g_sample_limit = atoi(argv[1]) << 20;

    if (0 != mem_map_init())
        return -1;

    pman_disable();
    
    Trid_Util_CPUComm_Init(0);
    
    es_desc_addr = get_addr(11);
    if (((es_desc_addr & 0xFFFF0000) == 0x15090000) || ((es_desc_addr & 0xFFFF0000) == 0x10010000)) {
        g_dtv_flag = 1;
        es_desc_addr |= 0xF5080000;
    }

    g_es_descriptor = (unsigned int)mem_map(es_desc_addr & 0xFFFF0000, 0x10000) + (es_desc_addr & 0x0000FFFF);
    
    g_pts_descriptor = (unsigned int)mem_map(0xF5093F54 & 0xFFFF0000, 0x10000) + (0xF5093F54 & 0x0000FFFF);

    descriptor_get(&desc, &pts_desc);

    printf("Desc: %x %x %x %x\n", desc.start, desc.end, desc.wp, desc.rp);
    printf("PTS Desc: %x %x %x %x\n", pts_desc.start, pts_desc.end, pts_desc.wp, pts_desc.rp);
    
    // patch, demux return DDR addr, avmips return PHY addr(-0x30000000)?
    if (!g_dtv_flag)
        g_es_buffer = (unsigned char *)(desc.start - 0x30000000);
    else
        g_es_buffer = (unsigned char *)(desc.start);

    g_es_buffer = mem_map((unsigned int)g_es_buffer, desc.end - desc.start);

    g_pts_buffer = mem_map((unsigned int)pts_desc.start, pts_desc.end - pts_desc.start);

    sem_init(&sem_pts, 0, 0);
    sem_init(&sem_es, 0, 0);

    sem_post(&sem_pts);
    sem_post(&sem_es);

    pthread_create(&pid, NULL, es_dump_thread, NULL);
    pthread_create(&pid, NULL, pts_dump_thread, NULL);

    while (1)
    {
        usleep(100*1000);
        if (hal_recv(hal_buf, HAL_BUF_SIZE, SAMPLE_US) > 0) {
            printf("channel change\n");
            sem_post(&sem_pts);
            sem_post(&sem_es);
        }
    }

    return 0;
}

