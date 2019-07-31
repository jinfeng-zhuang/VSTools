#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

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

unsigned int *g_pts_descriptor;
unsigned char *g_pts_buffer;
unsigned int g_sample_total;
static char directory[64];

void pts_descriptor_get(struct local_desc *desc, struct local_desc *pts_desc)
{
    if (pts_desc != NULL) {
        pts_desc->start = ((struct pts_desc *)g_pts_descriptor)->start;
        pts_desc->end   = ((struct pts_desc *)g_pts_descriptor)->end;
        pts_desc->wp    = ((struct pts_desc *)g_pts_descriptor)->wp;
        pts_desc->rp    = ((struct pts_desc *)g_pts_descriptor)->rp;
    }
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
    char *filename;
    FILE *fp;
    struct local_desc desc_prev;
    struct local_desc desc_cur;
    int ret;
    
    if (NULL == args)
        return NULL;

    filename = "dump.pts";
    fp = NULL;
    memset(&desc_prev, 0, sizeof(struct local_desc));
    memset(&desc_cur, 0, sizeof(struct local_desc));
    ret = 0;
    
    // FILE Setup
    fp = fopen(filename, "wb");
    if (NULL == fp) {
        printf("Can't open %s\n", filename);
        return NULL;
    }

    // DESCRIPTOR Init
    pts_descriptor_get(NULL, &desc_prev);
    desc_prev.start = desc_prev.start;
    desc_prev.end   = desc_prev.end;
    desc_prev.wp    = desc_prev.start; // TODO demux don't reset ?
    desc_prev.rp    = desc_prev.start;

    while (1) {
            
        usleep(SAMPLE_US);

        pts_descriptor_get(NULL, &desc_cur);

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

    return NULL;
}

int func_ptsdump(int channel, const char *homedir)
{
    pthread_t pid;
    unsigned int ddr_addr;
    struct local_desc pts_desc;
    
    if (NULL == homedir) {
        strncpy(directory, "/tmp", sizeof(directory));
    }   
    else {
        strncpy(directory, homedir, sizeof(directory));
    } 
    
    if ((channel != 0) && (channel != 1))
        return -1;
    
    // SX7B don't have pts buffer, as malone use PES
    if (0 == channel)
        ddr_addr = comm_get_addr(19 | 0 << 6);
    else if (1 == channel)
        ddr_addr = comm_get_addr(19 | 1 << 6); // use constant can block error spread
    else
        return -1;
    
    printf("pts descriptor @ 0x%08x\n", ddr_addr);
    
    g_pts_descriptor = (unsigned int)mem_map(ddr_addr & 0xFFFF0000, 0x10000) + (ddr_addr & 0x0000FFFF);

    pts_descriptor_get(NULL, &pts_desc);
    
    // get buffer
    g_pts_buffer = mem_map((unsigned int)pts_desc.start, pts_desc.end - pts_desc.start);

    pthread_create(&pid, NULL, pts_dump_thread, NULL);

    while (1)
    {
        pts_descriptor_get(NULL, &pts_desc);    
        printf("PTS: %x %x %x %x\n", pts_desc.start, pts_desc.end, pts_desc.wp, pts_desc.rp);
        usleep(100*1000);
    }

    return 0;
}

