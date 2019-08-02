#include <stdio.h>

#include "pman.h"
#include "cpu_comm.h"
#include "option.h"
#include "misc.h"

static void Func_SlaveInfoUpdate(Trid_CPUFuncCall_Param_t* pCallParam, Trid_CPUFuncCall_Return_t* pReturnParam)
{
    printf("%s\n", __func__);
}

int main(int argc, char *argv[])
{
    int ret;
    int opt;
    unsigned int chip_id;

    ret = option_process(argc, argv);
    if (-1 == ret) {
        return 0;
    }
    
    if (0 != mem_map_init())
        return -1;

    chip_id = get_chip_id();
    if (CHIP_ID_SX7B != chip_id) {
        printf("Supported chip: SX7B, %x\n", chip_id);
        return 0;
    }

    pman_disable();
    
    Trid_Util_CPUComm_Init();

    printf("Hello Android\n");
#if 0
    Trid_Util_CPUComm_InstallRoutine("SlaveInfoUpdate", Func_SlaveInfoUpdate, 0, 0);

    if (g_option.vesdump[0])
        func_vesdump(0, g_option.homedir);

    if (g_option.vesdump[1])
        func_vesdump(1, g_option.homedir);

    if (g_option.ptsdump[0])
        func_ptsdump(0, g_option.homedir);

    if (g_option.ptsdump[1])
        func_ptsdump(1, g_option.homedir);
#endif

    while (1)
        usleep(100*1000);
}

