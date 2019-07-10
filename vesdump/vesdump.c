#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "vesdump.h"

enum {
    TYPE_BYTE,
    TYPE_HALFWORD,
    TYPE_WORD
};

#define SAMPLE_US   (1000*1000) // about 400KB

#define SAMPLE_LIMIT  (50<<20)

#define HAL_BUF_SIZE   128

static unsigned char hal_buf[HAL_BUF_SIZE];

unsigned int desc_v_base;
unsigned char *es_v_base;

struct desc {
    unsigned int start;
    unsigned int end;
    unsigned int wr;
    unsigned int rd;
};

#if 0

struct local_desc {
    unsigned int start;
    unsigned int end;
    unsigned int wr;
    unsigned int rd;
};

struct avmips_desc {
    unsigned int start;
    unsigned int end;
    unsigned int wr;
    unsigned int rd;
};

struct demux_desc {
    unsigned int wr;
    unsigned int rd;
    unsigned int start;
    unsigned int end;
};

void capture_desc(struct local_desc *desc, void *addr, int demux_flag)
{
    if (demux_flag) {
        desc->start = (struct demux_desc *)addr->start;
        desc->end = (struct demux_desc *)addr->end;
        desc->wr = (struct demux_desc *)addr->wr;
        desc->rd = (struct demux_desc *)addr->rd;
    }
    else {
        desc->start = (struct avmips_desc *)addr->start;
        desc->end = (struct avmips_desc *)addr->end;
        desc->wr = (struct avmips_desc *)addr->wr;
        desc->rd = (struct avmips_desc *)addr->rd;
    }
}
#endif

#define SIZE_1MB    (1<<20)
#define SIZE_1KB    (1<<10)

int main(int argc, char *argv[])
{
    int ret;
    int i;
    char *dir;
    int filecnt = 0;
    char filename[32];

    Trid_CPUFuncCall_Param_t CallParam;
    Trid_CPUFuncCall_Return_t CallReturn;

    FILE *fp = NULL;

    struct desc *desc;
    unsigned int wr_prev;
    unsigned int wr_cur;
    unsigned int sample_size;
    unsigned int offset;

    unsigned int file_size;

    unsigned int base;
    int demux_flag = 0;

    if (argc != 2) {
        printf("Usage: %s [directory]\n", argv[0]);
        return 0;
    }

    dir = argv[1];

    if (0 != mem_map_init())
        return -1;

    pman_disable();

    printf("pman disabled\n");
    
    Trid_Util_CPUComm_Init(0);
    
    CallParam.Connection.targetCPU = CPU_AV;
    CallParam.Connection.targetTokenPid = NUCLEUS_PID;
    CallParam.Count = 4;
    CallParam.Param[0] = 0; // 0-read, 1-write, 2-write mask
    CallParam.Param[1] = TYPE_WORD;
    CallParam.Param[2] = 0xBADBAD0B; //BADBAD00 + 0B: GET_VIDEO_DEBUG_INFO_VESDESCADDR
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
        printf("waiting for desc ready...\n");
    }

    if (((CallReturn.Param[1] & 0xFFFF0000) == 0x15090000) || ((CallReturn.Param[1] & 0xFFFF0000) == 0x10010000)) {
        demux_flag = 1;
        CallReturn.Param[1] |= 0xF5080000;
    }

    printf("Desc addr: %x\n", CallReturn.Param[1]);

    desc_v_base = (unsigned int)mem_map(CallReturn.Param[1] & 0xFFFF0000, 0x10000) + (CallReturn.Param[1] & 0x0000FFFF);

    desc = (struct desc *)desc_v_base;

    struct desc local_desc;

    if (demux_flag) {
        local_desc.wr = desc->start & 0x3FFFFFFF;
        local_desc.rd = desc->end & 0x3FFFFFFF;
        local_desc.start = desc->wr;
        local_desc.end = desc->rd;
    }
    else {
        local_desc.wr = desc->wr;
        local_desc.rd = desc->rd;
        local_desc.start = desc->start;
        local_desc.end = desc->end;
    }

    printf("Desc: %x %x %x %x\n", local_desc.start, local_desc.end, local_desc.wr, local_desc.rd);

    // patch, for DEMUX?
    if (!demux_flag)
        local_desc.end += 1;

    wr_prev = local_desc.start;

    // patch, demux return DDR addr, avmips return PHY addr(-0x30000000)?
    if (!demux_flag)
        es_v_base = (unsigned char *)(local_desc.start - 0x30000000);
    else
        es_v_base = (unsigned char *)(local_desc.start);

    es_v_base = mem_map((unsigned int)es_v_base, local_desc.end - local_desc.start);

    if (demux_flag) {
        while ((desc->start & 0x3FFFFFFF) == wr_prev)
            usleep(SAMPLE_US);
    }

    unsigned int sample_total = 0;
    unsigned int data_write;

    while (1) {
        usleep(SAMPLE_US);

        if (hal_recv(hal_buf, HAL_BUF_SIZE, SAMPLE_US) > 0) {
            // close old file
            if (NULL != fp) {
                printf("Record %s (%d MB) done.\n", filename, ftell(fp)>>20);
                fclose(fp);
                fp = NULL;
            }

            // create new file every time
            sprintf(filename, "%s/dump-%03d.es", dir, filecnt);

            fp = fopen(filename, "wb");
            if (NULL == fp) {
                printf("Can't open %s\n", filename);
                break;
            }
            else {
                printf("Record %s started\n", filename);
                filecnt++;
            }

            wr_prev = local_desc.start;
        }

        // trigger at first loop
        if (NULL == fp) {
            continue;
        }

        if (demux_flag)
            wr_cur = desc->start & 0x3FFFFFFF;
        else
            wr_cur = desc->wr;

        // !!! local start
        offset = wr_prev - local_desc.start;

        if (wr_cur >= wr_prev) {
            sample_size = wr_cur - wr_prev;
            printf("%d MB + %d KB (%s)\n", sample_total>>20, sample_size >> 10, filename);
            data_write = fwrite(&es_v_base[offset], sizeof(unsigned char), sample_size, fp);
            fflush(fp);
            if (data_write != sample_size)
                printf("WARNING: write not complete\n");
        }
        else {
            sample_size = local_desc.end - wr_prev + wr_cur - local_desc.start;
            printf("(Wrap) %d MB + %d KB (%s)\n", sample_total>>20, sample_size >> 10, filename);
            data_write = fwrite(&es_v_base[offset], sizeof(unsigned char), local_desc.end - wr_prev, fp);
            data_write += fwrite(es_v_base, sizeof(unsigned char), wr_cur - local_desc.start, fp);
            fflush(fp);
            if (data_write != sample_size)
                printf("(L)WARNING: write not complete\n");
        }

        sample_total += sample_size;

        if (sample_total >= SAMPLE_LIMIT) {
            printf("sample limit to %d MB\n", SAMPLE_LIMIT);
            break;
        }

        wr_prev = wr_cur;

    }

    if (NULL != fp) {
        printf("Record %s (%d MB) done.\n", filename, ftell(fp)>>20);
        fclose(fp);
        fp = NULL;
    }

    return 0;
}

