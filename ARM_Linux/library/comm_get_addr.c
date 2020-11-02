#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "cpu_comm.h"

#define SAMPLE_US   (1000*1000) // about 400KB

enum {
    TYPE_BYTE,
    TYPE_HALFWORD,
    TYPE_WORD
};

unsigned int comm_get_addr(int index)
{
    int ret;
    Trid_CPUFuncCall_Param_t CallParam;
    Trid_CPUFuncCall_Return_t CallReturn;

    CallParam.Connection.targetCPU = CPU_AV;
    CallParam.Connection.targetTokenPid = NUCLEUS_PID;
    CallParam.Count = 4;
    CallParam.Param[0] = 0; // 0-read, 1-write, 2-write mask
    CallParam.Param[1] = TYPE_WORD;
    CallParam.Param[2] = 0xBADBAD00 | index; //BADBAD00 + 0B: GET_VIDEO_DEBUG_INFO_VESDESCADDR
    CallParam.Param[3] = 0;

    while (1) {
        memset(&CallReturn, 0, sizeof(Trid_CPUFuncCall_Return_t));
        ret = Trid_Util_CPUComm_Call("RegTest", &CallParam, &CallReturn);
        if (0 == ret) {
            if (CallReturn.Param[1] != 0) {
                break;
            }
            else
                printf("%s return param %d\n", __func__, CallReturn.Param[1]);
        }
        else
            printf("%s return %d\n", __func__, ret);

        usleep(SAMPLE_US);
        printf("CPUComm_Call return %d, address = %x\n", ret, CallReturn.Param[1]);
    }

    return CallReturn.Param[1];
}

