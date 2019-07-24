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

unsigned int channel;

typedef unsigned long long trid_uint64;

typedef struct _tagTriVideoShareInfo {
    // part1: video decoder info
    trid_uint64 VideoPTS;
    trid_uint64 VideoPtsStcDelta;
    trid_uint64 StcPcrDelta;
    trid_uint32 bVideoSyncOn;
    trid_uint32 VideoErrorMB;
    trid_uint32 bVideoLocked;
    trid_uint32 VideoCount;

    // part2: video profile&level
    trid_uint32 VideoStandard;
    trid_uint32 VideoProfile;
    trid_uint32 VideoLevel;

    // part3: video statistics info
    trid_uint32 SkippedFrams;
    trid_uint32 DecodedFrames;
    trid_uint64 DecodedBytes;
    trid_uint32 BytesInESBuffer;
    trid_uint32 FrmsInRDY2DispQ;
} triVideoSharedInfo_t;

#define CODEC_NUMBER    16

// refer to VideoStreamType_e
const char *codec_name[CODEC_NUMBER] = {
    "H264",
    "VC1",
    "MPEG",
    "MPEG4",
    "DIVX3",
    "RV",
    "AVS",
    "JPEG",
    "VP6",
    "SPARK",
    "VP8",
    "M264",
    "MJPEG_A",
    "MJPEG_B",
    "H265",
    "VP9"
};

const char *get_codec_name(int index)
{
    if (index < CODEC_NUMBER)
        return codec_name[index];
    else
        return NULL;
}

const char *template =
"VideoPTS %llx\n\
VideoPtsStcDelta %llx\n\
StcPcrDelta %llx\n\
bVideoSyncOn %x\n\
VideoErrorMB %x\n\
bVideoLocked %x\n\
VideoCount %x\n\
VideoStandard %x\n\
VideoProfile %x\n\
VideoLevel %x\n\
SkippedFrams %x\n\
DecodedFrames %x\n\
DecodedBytes %llx\n\
BytesInESBuffer %x\n\
FrmsInRDY2DispQ %x\n";

static char buffer[1024];

int main(int argc, char *argv[])
{
    int ret;
    unsigned int ddr_addr;
    unsigned int virt_addr;
    triVideoSharedInfo_t *info;
    
    ret = 0;
    info = NULL;

    if (argc != 2) {
        printf("Usage:\n");
        printf("\t%s [CHANNEL]\n", argv[0]);
        return 0;
    }

    channel = atoi(argv[1]);

    if (0 != mem_map_init())
        return -1;

    pman_disable();
    
    Trid_Util_CPUComm_Init(0);
    
    if (channel == 1) {
        ddr_addr = comm_get_addr(1 | 1<<6);
    }
    else if (channel == 0) {
        ddr_addr = comm_get_addr(1);
    }
    else {
        return 0;
    }

    printf("ddr_addr = %x\n", ddr_addr);

    virt_addr = (unsigned int)mem_map(ddr_addr & 0xFFFF0000, 0x10000) + (ddr_addr & 0x0000FFFF);

    info = (triVideoSharedInfo_t *)virt_addr;

    while (1) {
        printf("\nCodec %s\n------------------------------\n", get_codec_name(info->VideoStandard));

        printf(template,
                info->VideoPTS,
                info->VideoPtsStcDelta,
                info->StcPcrDelta,
                info->bVideoSyncOn,
                info->VideoErrorMB,
                info->bVideoLocked,
                info->VideoCount,
                info->VideoStandard,
                info->VideoProfile,
                info->VideoLevel,
                info->SkippedFrams,
                info->DecodedFrames,
                info->DecodedBytes,
                info->BytesInESBuffer,
                info->FrmsInRDY2DispQ
              );

        usleep(100*1000);
    }
    
    return 0;
}

