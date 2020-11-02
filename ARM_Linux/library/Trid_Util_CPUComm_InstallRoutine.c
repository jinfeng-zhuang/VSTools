#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <vs/cpu_comm.h>

extern int g_comm_fd;

int Trid_Util_CPUComm_InstallRoutine(const char* FuncName, CPURoutine_t pRoutine, int TokenPid, int channel)
{
    int ret;
    char name[COMM_FUNC_NAME_LEN_MAX];
    RoutineItem item;

    // 0 - current CPU listen this event
    // 0 - ignore pid
    snprintf(name, COMM_FUNC_NAME_LEN_MAX, "%s_%1x_%3.3x", FuncName, 0, 0);

    strncpy(item.FuncName, name, COMM_FUNC_NAME_LEN_MAX);

    item.pRoutine = (funcCPUComm)pRoutine;
    item.pid = getpid();
    item.ChanID = channel;
    item.FuncID = Trid_Util_CRC32(0x123456, name, strlen(name));
    item.NewBGToken = 0;

    ret = ioctl(g_comm_fd, CPU_COMM_INSTALL_ROUNTINE, &item);
    if (0 != ret)
        return -1;

    return 0;
}

