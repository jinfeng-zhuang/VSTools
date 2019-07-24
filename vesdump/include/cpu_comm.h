#ifndef VS_CPU_COMM_H
#define VS_CPU_COMM_H

#include "ioctl.h"
#include "list.h"

#define HIDTV_CPU_COMM_MAGIC ('2' | 'N' | 'D' | 'C' | 'P' | 'U')

#define CPU_COMM_PARAMCOUNT_MAX 10

typedef struct _tagCommParam {
    volatile unsigned short channelId;
    volatile unsigned short tgetCPU;
    unsigned short Index;
    volatile unsigned short Attribute;
    volatile unsigned short ParamCount;
    volatile unsigned short dbgstate;
    volatile unsigned int SessionID;
    volatile int pid;
    volatile int sourcepid;
    volatile int tgetCPU_bak;
    volatile int tgetTokenPid_bak;
    volatile void* payload;
    volatile unsigned long FunctionID;
    volatile unsigned long Param[CPU_COMM_PARAMCOUNT_MAX];
    struct list_head items;
} CommParam_t, *pCommParam_t;

typedef struct {
    int index;
    unsigned long MemPhyAddr;
    unsigned long MemVirtAddr;
    unsigned long MemSize;
    unsigned long MemKernVirtAddr;
} CPUCommShMemMap_t;

typedef struct {
    unsigned int targetCPU;
    unsigned int targetTokenPid;
    unsigned int srcCPU;
    unsigned int srcTokenPid;
} Rpc_Connection_t;

typedef struct _tagCPUFuncCallParam {
    unsigned int Count;
    unsigned int Param[CPU_COMM_PARAMCOUNT_MAX];
    Rpc_Connection_t Connection;
} Trid_CPUFuncCall_Param_t;

typedef struct _tagCPUFuncCallReturn {
    unsigned int Count;
    unsigned int Param[CPU_COMM_PARAMCOUNT_MAX];
} Trid_CPUFuncCall_Return_t;

typedef enum {
    CPU_STATE_APP_READY_SET = 0,
    CPU_STATE_APP_READY_GET,
    CPU_STATE_NOTICE_REQ_GET,
} CPUStateAction;

typedef struct _tagCPUStateInfo {
    short state;
    short cpuID;
    CPUStateAction act;
} CPUStateInfo, *pCPUStateInfo;

typedef struct _tagGetReturnInfo {
    unsigned short tgetCPU;
    unsigned int SessionID;
} GetReturnInfo, *pGetReturnInfo;

#define CPU_COMM_SEND_MASTER_CALL   _IOWR(HIDTV_CPU_COMM_MAGIC, 0x01, CommParam_t*)
#define CPU_COMM_GET_SLAVE_RESP     _IOWR(HIDTV_CPU_COMM_MAGIC, 0x04, CommParam_t*)
#define CPU_COMM_GET_MEM_MAP_INFO   _IOWR(HIDTV_CPU_COMM_MAGIC, 0x13, CPUCommShMemMap_t*)
#define CPU_COMM_CPU_STATE          _IOWR(HIDTV_CPU_COMM_MAGIC, 0x21, CPUStateInfo *)

#endif
