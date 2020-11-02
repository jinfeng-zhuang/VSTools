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

/*******************************************************************************
* MpegFormat defition
*******************************************************************************/
typedef enum {
    ASPECT_None = 0,
    ASPECT_Square = 1,
    ASPECT_4_3,
    ASPECT_16_9,
    ASPECT_2P21_1,
    ASPECT_14_9,
    ASPECT_Total,
} AspectCode_t;

typedef enum {
    FRAMERATE_OTHER = 0,
    FRAMERATE_23_976_HZ = 1,
    FRAMERATE_24_HZ = 2,
    FRAMERATE_25_HZ = 3,
    FRAMERATE_29_97_HZ = 4,
    FRAMERATE_30_HZ = 5,
    FRAMERATE_50_HZ = 6,
    FRAMERATE_59_94_HZ = 7,
    FRAMERATE_60_HZ = 8,
    FRAMERATE_8_HZ = 9,
    FRAMERATE_12_HZ = 10,
    FRAMERATE_15_HZ = 11,
    FRAMERATE_100_HZ = 12,
    FRAMERATE_119_HZ = 13,
    FRAMERATE_120_HZ = 14,
    FRAMERATE_MAX,
} FrameRate_t;

typedef enum {
    PROGSEQ_INTERLACE = 0,
    PROGSEQ_PROGRESSIVE = 1,
} ProgSeq_e;

/**
* Interlace first field definition
*/
typedef enum {
    BOTTOM_FIELD_FIRST = 1,
    TOP_FIELD_FIRST = 0,
} InterlaceFirstField_e;

/**
* Fast play type
*/
typedef enum {
    FASTPLY_KEY_FRAME_ONLY = 0,
    FASTPLY_ALL_FRAME,
} FastPlayType_e;

/*******************************************************************************
* Active format  definition
*******************************************************************************/
typedef enum _ACTIVEFORMAT {
    AFD_16_9_BOX = 2,
    AFD_14_9_BOX = 3,
    AFD_16_9_BOX_CENTRE = 4,
    AFD_AUTO = 8,         // 1000
    AFD_4_3 = 9,          // 1001
    AFD_16_9 = 10,        // 1010
    AFD_14_9 = 11,        // 1011
    AFD_FUTURE = 12,      // 1100
    AFD_4_3_SHOOT = 13,   // 1101
    AFD_16_9_SHOOT = 14,  // 1110
    AFD_SHOOT = 15        // 1111
} ACTIVEFORMAT;

/*
Important: this structure must be the same as the on in Master!!!!!!!!!
*/
typedef struct {
    int cropOffsetTop;
    int cropOffsetBottom;
    int cropOffsetLeft;
    int cropOffsetRight;
} CROP_INFO_s;

typedef struct {
    int picdisplayOffsetTop;
    int picdisplayOffsetBottom;
    int picdisplayOffsetLeft;
    int picdisplayOffsetRight;
} PIC_DISPLAY_INFO_s;

typedef enum {
    CLOCK_6_75MHZ = 0,    // 6,75MHz
    CLOCK_13_5MHZ = 1,    // 13.5MHz
    CLOCK_27MHZ = 2,      // 27MHz
    CLOCK_37_125MHZ = 3,  // 74.25/2MHz
    CLOCK_74_25MHZ = 4,   // 74.25MHz
    CLOCK_148_5MHZ = 5,   // 148.5 MHz
} ClockFreq_e;

typedef enum {
    BPP8 = 0,
    BPP10,
} PixelDepth_e;

typedef enum {
    SINGLE_BUF_MODE = 0,    // 2 fields are stored in one single buffer.
    SEPARATE_BUF_MODE = 1,  // 2 fields are stored in separate buffer.
} FieldsStoreMode_e;

// aaron. adding for PCL Fusion
typedef struct {
    unsigned int debugbufferaddress;
    unsigned int debugbuffersize;
    unsigned int crashcause;  // optional - if you would like to signal us something for logging in MIPS.
} MVEDInfo_t;

typedef struct {
    unsigned int bPictureQualityAvailable;
    unsigned int ulPictureQuality; /**< average QP during one GOP, it should be 1~51 for H.264 and 1~31 for other formats */

    unsigned int bGopBitRateAvailable;
    unsigned int ulGopBitRate; /**< average bits per frame during one GOP */
} FrameQMeterInfo_t;

typedef enum {
    PART_RANGE_MODE = 0,
    FULL_RANGE_MODE = 1,
} FullRange_e;

/**
* MPEG format structure
*/
typedef struct _tagMpegFormat {
    // part 1, initial form
    unsigned int wHsize;                  // hde size
    unsigned int wVsize;                  // vde size
    unsigned int wDispHsize;              // display horizontal size
    unsigned int wDispVsize;              // display vertival size
    AspectCode_t AspectCode;              // aspect code
    FrameRate_t FrameRate;                // frame rate
    ProgSeq_e ProgressiveSeq : 1;         // progressive sequence
    InterlaceFirstField_e FirstField : 1; /**< interlace first field*/
    ClockFreq_e clockFreq : 4;
    unsigned int usStride : 16; /**< stride, max 64K */
    PixelDepth_e PixelDepth : 2;
    FieldsStoreMode_e FieldBufMode : 1;
    FrameRate_t FrmRateConv : 5;
    FullRange_e FullRageMode : 1;
    unsigned int Dummy : 1; /**< dummy one*/
    ACTIVEFORMAT AFD;
    unsigned int Par;  // 32bits for Par:width/height

    // part 2, added on
    unsigned int wHdeStart;
    unsigned int wVdeStart;
    unsigned int wHTotal;
    unsigned int wVTotal;
    unsigned char colorPrimary;
    unsigned char transferChar;
    unsigned char matrixCoef;
    unsigned char chromaLocBot;
    unsigned char chromaLocTop;
    CROP_INFO_s cropInfo;
    PIC_DISPLAY_INFO_s picDispInfo;
} MpegFormat_t;

const char *template =
"\
wHsize %d\n\
wVsize %d\n\
wDispHsize %d\n\
wDispVsize %d\n\
AspectCode %x\n\
FrameRate %d\n\
ProgressiveSeq %x\n\
FirstField %x\n\
clockFreq %x\n\
usStride %d\n\
PixelDepth %x\n\
FieldBufMode %x\n\
FrmRateConv %x\n\
FullRageMode %x\n\
Dummy %x\n\
AFD %x\n\
Par %x\n\
wHdeStart %x\n\
wVdeStart %x\n\
wHTotal %x\n\
wVTotal %x\n\
colorPrimary %x\n\
transferChar %x\n\
matrixCoef %x\n\
chromaLocBot %x\n\
chromaLocTop %x\n\
cropInfo %x %x %x %x\n\
picDispInfo %x %x %x %x\n\
";

static char buffer[1024];

int main(int argc, char *argv[])
{
    int ret;
    unsigned int ddr_addr;
    unsigned int virt_addr;
    MpegFormat_t *format; 

    ret = 0;
    format = NULL;

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
        ddr_addr = comm_get_addr(2 | 1<<6);
    }
    else if (channel == 0) {
        ddr_addr = comm_get_addr(2);
    }
    else {
        return 0;
    }

    printf("ddr_addr = %x\n", ddr_addr);

    virt_addr = (unsigned int)mem_map(ddr_addr & 0xFFFF0000, 0x10000) + (ddr_addr & 0x0000FFFF);

    format = (MpegFormat_t *)virt_addr;

    while (1) {
        printf("\nFormat %d x %d, Framerate %d\n------------------------------\n",
                format->wHsize,
                format->wVsize,
                format->FrameRate);

        printf(template,
                format->wHsize,
                format->wVsize,
                format->wDispHsize,
                format->wDispVsize,
                format->AspectCode,
                format->FrameRate,
                format->ProgressiveSeq,
                format->FirstField,
                format->clockFreq,
                format->usStride,
                format->PixelDepth,
                format->FieldBufMode,
                format->FrmRateConv,
                format->FullRageMode,
                format->Dummy,
                format->AFD,
                format->Par,
                format->wHdeStart,
                format->wVdeStart,
                format->wHTotal,
                format->wVTotal,
                format->colorPrimary,
                format->transferChar,
                format->matrixCoef,
                format->chromaLocBot,
                format->chromaLocTop,
                format->cropInfo.cropOffsetTop,
                format->cropInfo.cropOffsetBottom,
                format->cropInfo.cropOffsetLeft,
                format->cropInfo.cropOffsetRight,
                format->picDispInfo.picdisplayOffsetTop,
                format->picDispInfo.picdisplayOffsetBottom,
                format->picDispInfo.picdisplayOffsetLeft,
                format->picDispInfo.picdisplayOffsetRight
              );

        usleep(100*1000);
    }
    
    return 0;
}

