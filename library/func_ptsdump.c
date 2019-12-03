#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define SAMPLE_US   (1000*1000) // about 400KB

struct local_desc {
    unsigned int start;
    unsigned int end;
    unsigned int wp;
    unsigned int rp;
};

struct pts_desc {
    unsigned int wp; 
    unsigned int rp; 
    unsigned int start;
    unsigned int end;
};

//=================================================================================================
// Variable
//=================================================================================================

static int g_dtv_flag;
static unsigned int *g_pts_descriptor;
static unsigned char *g_pts_buffer;
static unsigned int g_sample_total;
static sem_t sem_es;
static char directory[64];

//=================================================================================================
// Function
//=================================================================================================

static void descriptor_get(struct local_desc *desc, struct local_desc *pts_desc)
{
    if (pts_desc != NULL) {
        pts_desc->start = ((struct pts_desc *)g_pts_descriptor)->start;
        pts_desc->end   = ((struct pts_desc *)g_pts_descriptor)->end;
        pts_desc->wp    = ((struct pts_desc *)g_pts_descriptor)->wp;
        pts_desc->rp    = ((struct pts_desc *)g_pts_descriptor)->rp;
    }
}

static int take_record(FILE *fp, const unsigned char *buffer, const struct local_desc *desc_prev, const struct local_desc *desc_cur, const char *filename)
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

static void *pts_dump_thread(void *args)
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

            sprintf(filename, "%s/dump-%03d.pts", directory, filecnt);

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
        printf("Record %s (%d MB) done.\n", filename, ftell(fp)>>20);
        fclose(fp);
    }
    
    return NULL;
}

int func_ptsdump(int channel, char *homedir)
{
    int ret;
    struct local_desc desc;
    struct local_desc pts_desc;
    pthread_t pid;
    unsigned int es_desc_addr;
    
    ret = 0;

    if (NULL == homedir)
        return -1;

    if ((channel != 0) && (channel != 1))
        return -1;

    strncpy(directory, homedir, sizeof(directory));

    es_desc_addr = comm_get_addr(19 | channel << 6);

    g_pts_descriptor = (unsigned int)mem_map(es_desc_addr & 0xFFFF0000, 0x10000) + (es_desc_addr & 0x0000FFFF);
    
    descriptor_get(NULL, &desc);

    printf("Desc: %x %x %x %x\n", desc.start, desc.end, desc.wp, desc.rp);
    
    g_pts_buffer = mem_map((unsigned int)desc.start, desc.end - desc.start);

    sem_init(&sem_es, 0, 0);

    sem_post(&sem_es);

    pthread_create(&pid, NULL, pts_dump_thread, NULL);

    return 0;
}

