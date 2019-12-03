#include <stdio.h>
#include <string.h>

#include <vs/cpu_comm.h>

void *Trid_Util_CPUComm_CreatShareInfo(unsigned int *length)
{
    int ret = 0;
    CPUCommShareInfo_t WantInfo;
    CPUCommShMemAction_t cSmmAct;

    memset(&WantInfo, 0, sizeof(CPUCommShareInfo_t));
    memset(&cSmmAct, 0, sizeof(CPUCommShMemAction_t));

    WantInfo.ShInfoAddr = 0;
    WantInfo.ShInfoSize = *length;

    ret = ioctl(g_comm_fd, CPU_COMM_CREAT_SHARE_INFO, &WantInfo);
    
    *length = WantInfo.ShInfoSize;

    if ((0 == ret) && (0 != WantInfo.ShInfoAddr)) {
        cSmmAct.MemAddr = (void *)WantInfo.ShInfoAddr;
        cSmmAct.action = SMM_PHYS_TO_VIR_ADD;

        ret = ioctl(g_comm_fd, CPU_COMM_SHMM_ACTION, &cSmmAct);
        
        if (0 == ret) {
            printf("Info Area: PHY Addr = %x Size = %x (VIRT %x)\n",
                    (unsigned int)WantInfo.ShInfoAddr,
                    (unsigned int)WantInfo.ShInfoSize,
                    (unsigned int)cSmmAct.MemAddr);

            return cSmmAct.MemAddr;
        }
    }

    return NULL;
}
