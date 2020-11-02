#include <stdio.h>

#include <vs/pman.h>
#include <vs/cpu_comm.h>
#include <vs/option.h>
#include <vs/misc.h>

int main(int argc, char *argv[])
{
    int ret;
    int opt;

    if (0 != mem_map_init())
        return -1;

    //pman_disable();

    printf("pman disabled\n");
    
    Trid_Util_CPUComm_Init();

    printf("cpu comm inited\n");

    func_vesdump(0, "/mnt");

    while (1)
        usleep(100*1000);

END:
    return 0;
}

