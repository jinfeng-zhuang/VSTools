#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "include/cpu_comm.h"

extern int g_comm_fd;

#define COMM_FUNC_NAME_LEN_MAX 64

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

    memset(response, 0, sizeof(Trid_CPUFuncCall_Return_t));

    memset(func_name, 0, COMM_FUNC_NAME_LEN_MAX);
    snprintf(func_name, COMM_FUNC_NAME_LEN_MAX, "%s_%1lx_%3.3lx", name, request->Connection.targetCPU, request->Connection.targetTokenPid & 0xFFF);

    func_id = Trid_Util_CRC32(0x123456, func_name, strlen(func_name));

    while (!cpu_comm_is_ready(g_comm_fd)) {
        printf("cpu comm not ready\n");
        usleep(10*1000);
    }

    memset(&callparam, 0, sizeof(CommParam_t));
    callparam.Attribute = 0;
    callparam.FunctionID = func_id;
    callparam.pid = getpid();
    callparam.sourcepid = getpid(); // TODO: any difference of pid ?
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

