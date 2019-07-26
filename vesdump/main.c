#include <stdio.h>
#include <getopt.h>

#include "crc.h"
#include "log.h"
#include "pman.h"
#include "cpu_comm.h"
#include "mmap.h"

enum {
    OPT_CHANNEL,
    OPT_AVSYNC,
    OPT_ESDESC,
    OPT_UART,
    OPT_VESDUMP,
    OPT_PTSDUMP,
    OPT_FORMAT,
    OPT_FRAMEQ,
    OPT_LOG,
    OPT_FILE,
};

enum {
    FUNC_NONE,
    FUNC_PTSDUMP,
};

static struct option long_options[] = {
    {"channel", required_argument, NULL, OPT_CHANNEL},
    {"file", required_argument, NULL, OPT_FILE},
    {"avsync", no_argument, NULL, OPT_AVSYNC},
    {"esdesc", no_argument, NULL, OPT_ESDESC},
    {"ptsdump", no_argument, NULL, OPT_PTSDUMP},
    {"uart", required_argument, NULL, OPT_UART}, // [enable | disable]
    {"log", required_argument, NULL, OPT_LOG},
    {NULL, 0, NULL, 0}
};

int main(int argc, char *argv[])
{
    int ret;
    unsigned int ddr_addr;
    unsigned int virt_addr;
    int i;
    int opt;
    int channel;
    char filename[64];
    int func;

    func = -1;
    // default channel 0
    channel = -1;
    memset(filename, 0, 64);

    if (0 != mem_map_init())
        return -1;

    pman_disable();
    
    Trid_Util_CPUComm_Init();
    
    while (1) {
        opt = getopt_long_only(argc, argv, "", long_options, NULL);
        if (-1 == opt)
            break;
        
        switch (opt) {
        case OPT_CHANNEL:
            channel = atoi(optarg);
            break;
        case OPT_FILE:
            strncpy(filename, optarg, 64);
            break;
        case OPT_ESDESC:
            if (channel != -1)
                func_esdesc(channel);
            break;
        case OPT_PTSDUMP:
            func = FUNC_PTSDUMP;
        default:
            break;
        }
    }
    
    if ((strlen(filename) > 0) && (channel != -1))
        func_ptsdump(channel, filename);
}

