
int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s [filename]\n", argv[0]);
        return 0;
    }

    TriHidtvRegMap();

    Thal_Common_Init();

    Thal_AVSync_Init();

    Thal_AVStream_Init();

    Thal_Video_Init();

    Thal_Demux_Init(&demux_config);

    Thal_AVSync_SetSyncMode(channel, _FILEPLAYBACK_MODE_);
    Thal_AVSync_TimerPause(channel);
    Thal_Video_InitDev(channel, VIDEO_CORE_HEVC);
    Thal_Video_Open(channel);
    Thal_AVStream_SetPTSOrder(channel, TRID_STREAM_NORMAL);
    Thal_Video_Connect(channel, _VIDEO_SRC_MEMORY_, DMX_INVALID_HANDLE);
    
    // create av stream
    Thal_AVStream_VideoNew(channel);

    usleep(ptPlayer->DataFeedTime * 1000);
    size = 512KB;
    GetTimeStamp();
    Thal_AVStream_Append(avstream, mem, size, pts);
    Trid_Slave_UpdateMfvdWp();

    // Thal_Video_SetBBDMode

    Thal_Video_SetBufWM();

    // start put es data

    Thal_AVSync_TimerSetTime(channel, 0);
    Thal_AVSync_TimerSetSpeed(channel, 1024);
    Thal_Video_SetTrickMode(channel, VIDEO_PLAY_SMOOTH);
    Thal_Video_SetDecMode;
    Thal_Video_SetFreeRunStatus;
    Thal_Video_Start;
    Thal_Video_SetDisplayMode;

    //DecodeOneJPEG;
    DecodeOneFrame;
}

