#include <stdio.h>

#include <vs/mmap.h>
#include <vs/pman.h>
#include <vs/cpu_comm.h>

static void Func_SlaveInfoUpdate(Trid_CPUFuncCall_Param_t* pCallParam, Trid_CPUFuncCall_Return_t* pReturnParam)
{
    printf("%s\n", __func__);
}


Trid_CPUFuncCall_Param_t CallParam;
Trid_CPUFuncCall_Return_t CallReturn;

enum {
    NOTIFY_VIDEO_EOS = 0x500,
};

int main(int argc, char *argv[])
{
    if (0 != mem_map_init())
        return -1;

    pman_disable();

    Trid_Util_CPUComm_Init();

    Trid_Util_CPUComm_InstallRoutine("SlaveInfoUpdate", Func_SlaveInfoUpdate, 0, 0);

    CallParam.Connection.targetCPU = CPU_AV;
    CallParam.Connection.targetTokenPid = NUCLEUS_PID;

    CallParam.Count = 3;
    CallParam.Param[0] = 0; // channel
    CallParam.Param[1] = NOTIFY_VIDEO_EOS;
    CallParam.Param[2] = 1; // enable

    int ret = Trid_Util_CPUComm_Call("AVCore_VideoSetEventFilter", &CallParam, &CallReturn);
    printf("ret = %d, callreturn %x\n", ret, CallReturn.Param[1]);

    while (1)
        usleep(100*1000);
}

