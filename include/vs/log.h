#ifndef VS_LOG_H
#define VS_LOG_H

typedef struct {
    int bIsEnable;
    int trace_level;
    int total_num;
    int rd_idx;
    int wr_idx;
    char message[512][256];
    int b3rdInit;
} triSlavePrint_t;

#endif
