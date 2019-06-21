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

#endif
