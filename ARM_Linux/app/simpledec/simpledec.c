#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "trid_util.h"
#include "thal_common_api.h"
#include "thal_video_api.h"
#include "thal_avstream_api.h"
#include "thal_avsync_api.h"

#define TRACE printf("[TRACE] %s %d\n", __func__, __LINE__)

#define SIZE_1KB (1<<10)
#define SIZE_1MB (1<<20)

int main(int argc, char *argv[])
{
    int i;
    ChannelType_e ChanType = _MAIN_CHANNEL_;
    Thal_AVStream_ptr AVStream;
    unsigned char *buffer;
    FILE *fp;
    unsigned int bytes_read;
    unsigned char *mem, *mem_rewind;
    trid_uint32 mem_rewind_size;
    unsigned int tmp;
    char *param_filename;
    char *param_stream_type;
    VideoStreamType_e stream_type;
    
    printf("simpledec build at %s - %s\n\n", __DATE__, __TIME__);

    //================================================================
    
    if (argc != 3) {
        printf("\tUsage: simpledec <stream type> <filename>\n");
        printf("\tSupported Format: H264, VIDEO_MJPEG_A\n");
        goto FAIL_PARAM;
    }
    
    param_filename = argv[2];
    param_stream_type = argv[1];
    
    if (0 == strcmp(param_stream_type, "H264"))
        stream_type = VIDEO_H264;
    else if (0 == strcmp(param_stream_type, "VIDEO_MJPEG_A"))
        stream_type = VIDEO_MJPEG_A;
    else {
        printf("Unknown format!\n");
        goto FAIL_PARAM;
    }

    //================================================================
    fp = fopen(param_filename, "rb");
    if (NULL == fp) {
        printf("file not found\n");
        return -1;
    }
    
    buffer = (unsigned char *)malloc(12<<20);
    if (NULL == buffer) {
        printf("malloc failed\n");
        return -1;
    }

    bytes_read = fread(buffer, 1, 12<<20, fp);
    printf("bytes_read = %x\n", bytes_read);
    //================================================================
    
    trid_debug_init("TEST");
    
    TriHidtvRegMap();

    if (SYS_NOERROR != Thal_Common_Init()) {
        printf("[FAIL] Thal_Common_Init: check if comm driver loaded\n");
        goto FAIL_HIDTV;
    }
    
    if (SYS_NOERROR != Thal_AVSync_Init()) {
        printf("[FAIL] Thal_AVSync_Init\n");
        goto FAIL_HIDTV;
    }

    // Create log catagory: AVSTREAM
    if (SYS_NOERROR != Thal_AVStream_Init()) {
        printf("[FAIL] Thal_AVStream_Init\n");
        goto FAIL_AVSTREAM;
    }
    
    if (SYS_NOERROR != Thal_Video_Init()) {
        printf("Thal_Video_Init failed\n");
        goto FAIL_VIDEO_INIT;
    }

    //================================================================
    
    Thal_AVSync_SetSyncMode(ChanType, _FILEPLAYBACK_TIMER_);
    Thal_AVSync_TimerPause(ChanType);
        
    Thal_Video_InitDev(ChanType, VIDEO_CORE_MALONE);
    
    if (SYS_NOERROR != Thal_Video_Open(ChanType)) {
        printf("Thal_Video_Open failed\n");
        goto FAIL_OPEN;
    }
    
    Thal_AVStream_SetPTSOrder(ChanType, 0); // stream mode replace 'Thal_Video_SetStreamMode'

    if (SYS_NOERROR != Thal_Video_Connect(ChanType, _VIDEO_SRC_MEMORY_, -1)) {
        printf("Thal_Video_Connect failed\n");
        //goto FAIL_OPEN;
    }

    AVStream = Thal_AVStream_VideoNew(ChanType);
    if (NULL == AVStream) {
        printf("[FAIL] Thal_AVStream_VideoNew\n");
        goto FAIL_AVSTREAM;
    }

    Thal_Video_SetBufWM(ChanType, 0x2000, 0x500000);

    for (i = 0; i < 11; i++) {
        mem = Thal_AVStream_Getbuffer(AVStream, SIZE_1MB, &mem_rewind, &mem_rewind_size, -1);

        if (mem) {
            memcpy(mem, &buffer[i * SIZE_1MB], SIZE_1MB);

            Thal_AVStream_Append(AVStream, mem, SIZE_1MB, 0);
        }
        
        Trid_Util_Delay(1);
    }
    
    //Thal_Video_NoitfyEOS(ChanType, trid_false);

    Thal_AVSync_TimerSetTime(ChanType, 0);

    Thal_AVSync_TimerSetSpeed(ChanType, 1024);

    Thal_Video_SetTrickMode(ChanType, VIDEO_PLAY_SMOOTH);

    Thal_Video_SetFreeRunStatus(ChanType, trid_false); // SyncOn = false

    if (SYS_NOERROR != Thal_Video_Start(ChanType, stream_type, 0)) {
        printf("Thal_Video_Start failed\n");
        goto FAIL_START;
    }

    printf("decoder started\n");

    while (1) {
        if (getchar() == 27) { // Escape
            break;
        }
        Trid_Util_Delay(100); // 1s
    }

    printf("decoder stop\n");

FAIL_START:
    Thal_Video_Stop(ChanType);

FAIL_OPEN:
    Thal_Video_Close(ChanType);

FAIL_VIDEO_INIT:
    Thal_Video_Exit();

FAIL_AVSTREAM:
    if (AVStream)
        Thal_AVStream_Free(AVStream);

    //Thal_Common_DeInit(); // can't deinit

FAIL_HIDTV:
    //TriHidtvRegUnmap();

    if (buffer)
        free(buffer);

    if (fp)
        fclose(fp);

FAIL_PARAM:
    return 0;
}