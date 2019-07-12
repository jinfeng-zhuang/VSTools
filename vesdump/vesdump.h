#ifndef _MAIN_H_
#define _MAIN_H_

enum {
    CPU_PLF,
    CPU_AV
};

#define NUCLEUS_PID 0

#define SYS_NOERROR 0

typedef unsigned int trid_uint32;
typedef char trid_char;
typedef char trid_bool;
typedef int RETURN_TYPE;


#define CPU_COMM_PARAMCOUNT_MAX 10

typedef struct {
    trid_uint32 targetCPU;      /**< the target CPU of the call*/
    trid_uint32 targetTokenPid; /**< the target token pid*/
    trid_uint32 srcCPU;         /**< the source CPU of the call*/
    trid_uint32 srcTokenPid;    /**< the source token pid*/
} Rpc_Connection_t;

typedef struct _tagCPUFuncCallParam {
    trid_uint32 Count;                          /**< actual filling param count in array, in unit of WORD */
    trid_uint32 Param[CPU_COMM_PARAMCOUNT_MAX]; /**< param array */
    Rpc_Connection_t Connection;                /**< cpu connection info*/
} Trid_CPUFuncCall_Param_t;

typedef struct _tagCPUFuncCallReturn {
    trid_uint32 Count;                          /**< actual filling param count in array, in unit of WORD */
    trid_uint32 Param[CPU_COMM_PARAMCOUNT_MAX]; /**< param array */
} Trid_CPUFuncCall_Return_t;


RETURN_TYPE Trid_Util_CPUComm_Init(trid_bool reserved);

#define Trid_Util_CPUComm_CallSlave Trid_Util_CPUComm_Call
RETURN_TYPE Trid_Util_CPUComm_Call(const trid_char* FuncName, Trid_CPUFuncCall_Param_t* pCallParam, Trid_CPUFuncCall_Return_t* pCallReturn);

extern int mem_map_init(void);
extern unsigned char *mem_map(unsigned int addr, unsigned int size);

extern void pman_disable(void);

typedef unsigned long HW_DWORD;

typedef struct {
    volatile HW_DWORD Reserved0;                  /* 0x00 reset vector 16 bits, irq vector 16 bits */
    volatile HW_DWORD Reserved1;                  /* 0x04 break point vector 16 bits, Reserved 16 bits */
    volatile HW_DWORD Reserved2;                  /* 0x08 Reserved for security */
    volatile HW_DWORD Reserved3;                  /* 0x0c */
    volatile HW_DWORD Reserved4;                  /* 0x10 */
    volatile HW_DWORD Reserved5;                  /* 0x14 */
    volatile HW_DWORD ParserCtrl;                 /* 0x18 Parser Control */
    volatile HW_DWORD VersionNumber;              /* 0x1c Version Number */
    volatile HW_DWORD ParserStatus;               /* 0x20 Parser Status */
    volatile HW_DWORD Capabilities;               /* 0x24 */
    volatile HW_DWORD InternalErrorCode;          /* 0x28 TSP unit internal error code */
    volatile HW_DWORD MsgFilter;                  /* 0x2c Message 0 - 31, messages not sent to host */
    volatile HW_DWORD Reserved6;                  /* 0x30  */
    volatile HW_DWORD CmdInterfaceDescriptor;     /* 0x34 Command Interface descriptor */
    volatile HW_DWORD MsgInterfaceDescriptor;     /* 0x38 Message Inteferface descriptor */
    volatile HW_DWORD PidTabDescriptor;           /* 0x3c PID table descriptor */
    volatile HW_DWORD KeyTabDescriptor;           /* 0x40 Key Table descriptor */
    volatile HW_DWORD PsiBufTabDescriptor;        /* 0x44 PSI buffer descriptor */
    volatile HW_DWORD XprtBufTabDescriptor;       /* 0x48 Transport Table buffer descriptor */
    volatile HW_DWORD EventLogBufTabDescriptor;   /* 0x4c Event Log Buffer descriptor */
    volatile HW_DWORD FilterTabDescriptor;        /* 0x50 Filter Table Descriptor */
    volatile HW_DWORD CaTabDescriptor;            /* 0x54 2 types of Desc if using NDS or Multi2 */
    volatile HW_DWORD EsBufferDescriptor;         /* 0x58 ES buffer descriptor */
    volatile HW_DWORD PTSDescriptor;              /* 0x5c  PTS buffer Descriptor*/
    volatile HW_DWORD PvrTabDescriptor;           /* 0x60 PVR Table Descriptor */
    volatile HW_DWORD XprtStreamTimeout;          /* 0x64 Transport Stream Timeout */
    volatile HW_DWORD DescramblerControl;         /* 0x68 Descrambler control */
    volatile HW_DWORD IdleCount;                  /* 0x6c Idle cycle count */
    volatile HW_DWORD StreamTabDescriptor;        /* 0x70 Pecos only */
    volatile HW_DWORD FilteringTabDescriptor;     /* 0x74 Filtering Table Descriptor */
    volatile HW_DWORD IndexingTabDescriptor;      /* 0x78 Indexing Table Descriptor */
    volatile HW_DWORD AuxPidTabDescriptor;        /* 0x7c Auxiliary Table Descriptor */
    volatile HW_DWORD Reserved32;                 /* 0x80  */
    volatile HW_DWORD PacketNumber;               /* 0x84 Packet Number */
    volatile HW_DWORD WaterTabDescriptor;         /* 0x88 Watermarking Table Descriptor*/
    volatile HW_DWORD WaterEventBufTabDescriptor; /* 0x8c Watermarking Event Buffer Table Descriptor*/
    volatile HW_DWORD AudioDataTabDescriptor;     /* 0x90 Audio Data Table Descriptor*/
    volatile HW_DWORD STCIndexDescriptor;         /* 0x94 STC Index descriptor */
    volatile HW_DWORD PCRtabDescriptor;           /* 0x98 PCR Table Descriptor */
    volatile HW_DWORD TestVectorDescriptor;       /* 0x9c Test Vector Address */
    volatile HW_DWORD CodeSignature;              /* 0xa0 Code Signature */
    volatile HW_DWORD SecTabDescriptor;           /* 0xa4 Security Table Descriptor */

} DMX_DESCRIPTOR;

typedef struct {
    volatile HW_DWORD uWritePtr;
    volatile HW_DWORD uReadPtr;
    volatile HW_DWORD uBufStartPtr;
    volatile HW_DWORD uBufEndPtr;

} DMX_PTS_BUF_DESCRIPTOR; 

#endif
