#include <stdio.h>
#include <string.h>

#include <vs/cpu_comm.h>

void *Trid_Util_CPUComm_CreatSingleInfo(unsigned int id, unsigned int length)
{
    int ret = 0;
    CPUCommSingleInfo_t WantInfo;
    CPUCommShMemAction_t cSmmAct;

    memset(&WantInfo, 0, sizeof(CPUCommSingleInfo_t));
    memset(&cSmmAct, 0, sizeof(CPUCommShMemAction_t));

    WantInfo.InfoAddr = 0;
    WantInfo.InfoID = id;
    WantInfo.InfoLen = length;

    ret = ioctl(g_comm_fd, CPU_COMM_CREAT_SINGLE_INFO, &WantInfo);
    
    if ((0 == ret) && (0 != WantInfo.InfoAddr)) {
        cSmmAct.MemAddr = (void *)WantInfo.InfoAddr;
        cSmmAct.action = SMM_PHYS_TO_VIR_ADD;

        ret = ioctl(g_comm_fd, CPU_COMM_SHMM_ACTION, &cSmmAct);
        
        if (0 == ret) {
            printf("Info: PHY Addr = %x Size = %x (VIRT %x)\n",
                    (unsigned int)WantInfo.InfoAddr,
                    (unsigned int)WantInfo.InfoLen,
                    (unsigned int)cSmmAct.MemAddr);
            return cSmmAct.MemAddr;
        }
    }

    return NULL;
}
