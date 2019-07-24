#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "include/cpu_comm.h"

extern int g_comm_fd;

#define COMM_FUNC_NAME_LEN_MAX 64

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

int cpu_comm_is_ready(int fd)
{
    int ret;
    CPUStateInfo info;

    if (-1 == fd)
        goto NOT_READY;

    info.cpuID = CPU_MAX_NONE;
    info.act   = CPU_STATE_APP_READY_GET;

    ret = ioctl_b(fd, CPU_COMM_CPU_STATE, &info);
    if (0 != ret)
        goto NOT_READY;

    if (info.state != CPU_STATE_READY)
        goto NOT_READY;

    return 1;

NOT_READY:
    return 0;
}

int Trid_Util_CPUComm_Call(const char *name, Trid_CPUFuncCall_Param_t *request, Trid_CPUFuncCall_Return_t *response)
{
    int ret;
    char func_name[COMM_FUNC_NAME_LEN_MAX];
    unsigned int func_id;
    CommParam_t callparam;
    GetReturnInfo *return_info;
    int i;

    if ((NULL == name) || (NULL == request) || (NULL == response))
        goto IOERROR;

    sprintf(func_name, "%s_%1lx_%3.3lx", name, request->Connection.targetCPU, request->Connection.targetTokenPid & 0xFFF);

    func_id = Trid_Util_CRC32(0x123456, func_name, strlen(func_name));

    while (!cpu_comm_is_ready(g_comm_fd))
        usleep(10*1000);

    callparam.Attribute = 0;
    callparam.FunctionID = func_id;
    callparam.pid = getpid();
    callparam.sourcepid = getpid();
    callparam.ParamCount = request->Count;
    for (i = 0; i< callparam.ParamCount; i++)
        callparam.Param[i] = request->Param[i];

    ret = ioctl_b(g_comm_fd, CPU_COMM_SEND_MASTER_CALL, &callparam);
    if (0 != ret)
        goto IOERROR;

    return_info = (GetReturnInfo *)&callparam;
    return_info->tgetCPU = callparam.tgetCPU;
    return_info->SessionID = callparam.SessionID;

    ret = ioctl_b(g_comm_fd, CPU_COMM_GET_SLAVE_RESP, return_info);
    if (0 != ret)
        goto IOERROR;

    response->Count = callparam.ParamCount;
    for (i = 0; i < callparam.ParamCount; i++)
        response->Param[i] = callparam.Param[i];

    return 0;

IOERROR:
    return -1;
}

