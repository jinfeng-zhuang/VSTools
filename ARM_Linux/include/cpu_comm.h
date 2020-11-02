#ifndef VS_CPU_COMM_H
#define VS_CPU_COMM_H

#include "ioctl.h"
#include "list.h"

#define NUCLEUS_PID 0

#define HIDTV_CPU_COMM_MAGIC ('2' | 'N' | 'D' | 'C' | 'P' | 'U')

#define CPU_COMM_PARAMCOUNT_MAX 10
#define COMM_FUNC_NAME_LEN_MAX  64

enum {
    CPU_STATE_RESET = -1,
    CPU_STATE_NOT_READY = 0,
    CPU_STATE_READY = 1
};

enum {
    CPU_PLF = 0,
    CPU_AV,
    CPU_DISP,
    CPU_MAX_NONE
};

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

typedef enum {
    SMM_TRANS_ADDRSS = 0,
    SMM_VIR_TO_PHYS_ADD,
    SMM_PHYS_TO_VIR_ADD,
} SMMTransAction;

typedef struct {
    void* MemAddr;
    int cpu;
    SMMTransAction action;
} CPUCommShMemAction_t;

typedef struct _tagCPUCommShareInfo {
    unsigned long ShInfoAddr;
    unsigned long ShInfoSize;
} CPUCommShareInfo_t;

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

typedef void (*funcCPUComm)(void);

typedef struct _tagRoutineItem {
    unsigned short ChanID;
    unsigned short TargetCPU;
    int pid;
    unsigned long FuncID;
    char FuncName[COMM_FUNC_NAME_LEN_MAX];
    funcCPUComm pRoutine;
    int NewBGToken;
    int nextIndex;
} RoutineItem, *pRoutineItem;

typedef struct _tagCPUCommSingleInfo {
    unsigned long InfoID;
    unsigned long InfoAddr;
    unsigned long InfoLen;
} CPUCommSingleInfo_t;

#define CPU_COMM_SHMM_ACTION        _IOWR(HIDTV_CPU_COMM_MAGIC, 0x14, CPUCommShMemAction_t*)

#define CPU_COMM_SEND_MASTER_CALL   _IOWR(HIDTV_CPU_COMM_MAGIC, 0x01, CommParam_t*)
#define CPU_COMM_GET_SLAVE_RESP     _IOWR(HIDTV_CPU_COMM_MAGIC, 0x04, CommParam_t*)
#define CPU_COMM_GET_MEM_MAP_INFO   _IOWR(HIDTV_CPU_COMM_MAGIC, 0x13, CPUCommShMemMap_t*)
#define CPU_COMM_CPU_STATE          _IOWR(HIDTV_CPU_COMM_MAGIC, 0x21, CPUStateInfo *)
#define CPU_COMM_INSTALL_ROUNTINE   _IOWR(HIDTV_CPU_COMM_MAGIC, 0x30, RoutineItem *)
#define CPU_COMM_CREAT_SHARE_INFO   _IOWR(HIDTV_CPU_COMM_MAGIC, 0x23, CPUCommShareInfo_t*)
#define CPU_COMM_CREAT_SINGLE_INFO  _IOWR(HIDTV_CPU_COMM_MAGIC, 0x24, CPUCommSingleInfo_t*)

extern int Trid_Util_CPUComm_Init(void);
extern int Trid_Util_CPUComm_Call(const char* FuncName, Trid_CPUFuncCall_Param_t* pCallParam, Trid_CPUFuncCall_Return_t* pCallReturn);
extern unsigned int comm_get_addr(int index);

typedef void (*CPURoutine_t)(Trid_CPUFuncCall_Param_t* pCallParam, Trid_CPUFuncCall_Return_t* pReturnParam);

extern int Trid_Util_CPUComm_InstallRoutine(const char* FuncName, CPURoutine_t pRoutine, int TokenPid, int channel);

extern int cpu_comm_is_ready(int fd);

extern unsigned int Trid_Util_CRC32(unsigned long val, const void* ss, int len);

extern int g_comm_fd;

extern void *Trid_Util_CPUComm_CreatShareInfo(unsigned int *length);
extern void *Trid_Util_CPUComm_CreatSingleInfo(unsigned int id, unsigned int length);

#endif
