#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#define snprintf _snprintf
#else
#include <unistd.h>
#include <time.h>
#define Sleep(ms) usleep(ms*1000)
#endif

#include <vs/net.h>
#include <vs/log.h>
#include <vs/dbg.h>
#include <vs/misc.h>
#include <vs/sx.h>
#include <vs/avmips.h>

unsigned char msg[256];

const char *usage = 
"avlog - Print AVMIPS's log through DebugServer, Build @ %s\n"
"Usage:\n"
"  avlog <ip> <setting>\n"
"Example:\n"
"  avlog 10.86.79.93 \"device:2,default:1,app_video:2,video_decode:1[core:host:mpeg:fmvd:malone],HEVC:1[drvr]\"\n"
"Attention:\n"
"  Space is invalid in <setting>\n";

int main(int argc, char* argv[])
{
    int ret;
    unsigned int chip_id;
    unsigned int version_addr;
    char version[64];
    unsigned int log_setting_addr;
    unsigned int log_addr;
    unsigned int wr;
    unsigned int rd;
    unsigned int msg_addr;
    struct log_desc local_desc;
    unsigned int i;
    char *log_setting;
    char *ip;
    FILE* fp = NULL;
    char output_filename[64];
    
#ifdef _WIN32
    SYSTEMTIME time;
#else
    time_t nsec;
    struct tm *tm;
#endif

    vs_log_init(LOG_MASK_AV | LOG_MASK_DBG | LOG_MASK_MISC, VS_LOG_INFO);

    if (argc != 3) {
        vs_log(LOG_MASK_AV, VS_LOG_WARNING, usage, __DATE__);
        return -1;
    }

    ip = argv[1];
    log_setting = argv[2];

    // TODO param check

#ifdef _WIN32
    GetLocalTime(&time);
    
    snprintf(output_filename, sizeof(output_filename), "%d-%02d-%02d_%02d-%02d-%02d.log",
        time.wYear,
        time.wMonth,
        time.wDay,
        time.wHour,
        time.wMinute,
        time.wSecond
    );
#else
    time(&nsec);

    tm = localtime(&nsec);

    snprintf(output_filename, sizeof(output_filename), "%d-%02d-%02d_%02d-%02d-%02d.log",
        tm->tm_year,
        tm->tm_mon,
        tm->tm_mday,
        tm->tm_hour,
        tm->tm_min,
        tm->tm_sec
    );
#endif

    ret = dbg_init(ip, (unsigned short)65528);
    if (0 != ret) {
        goto END;
    }

    // Get Chip ID
    ret = dbg_host_read32(0xF500E000, &chip_id, 1);
    if (0 != ret) {
        goto END;
    }

    vs_log(LOG_MASK_AV, VS_LOG_INFO, "Chip: %s\n", chip_name(chip_id));

    // Get Log Address
    ret = dbg_avmips_read32(GET_VIDEO_DEBUG_INFO_PRINTMSGADDR, &log_addr, 1);
    if (0 != ret) {
        goto END;
    }

    vs_log(LOG_MASK_AV, VS_LOG_MODULE, "Current Log Address: 0x%08x\n", log_addr);

    ret = dbg_avmips_read32(GET_AVMIPS_VERSION, &version_addr, 1);
    if (0 != ret) {
        goto END;
    }

    vs_log(LOG_MASK_AV, VS_LOG_MODULE, "Current version Address: 0x%08x\n", version_addr);

    if (version_addr) {
        ret = dbg_host_read8(version_addr, (unsigned char *)version, sizeof(version));
        if (0 != ret) {
            goto END;
        }

        vs_log(LOG_MASK_AV, VS_LOG_INFO, "AVMIPS Version: %s\n", version);
    }
    else {
        vs_log(LOG_MASK_AV, VS_LOG_INFO, "AVMIPS Version: not supported\n");
    }

    // Set Log Settings
    ret = dbg_avmips_read32(GET_VIDEO_DEBUG_INFO_PRINTSET, &log_setting_addr, 1);
    if (0 != ret) {
        goto END;
    }

    vs_log(LOG_MASK_AV, VS_LOG_MODULE, "Current Setting Address: 0x%08x\n", log_setting_addr);

    ret = dbg_host_write8(log_setting_addr, (unsigned char *)log_setting, strlen(log_setting));
    if (0 != ret) {
        goto END;
    }

    ret = dbg_avmips_read32(GET_VIDEO_DEBUG_INFO_PRINTPARSE, &log_setting_addr, 1);
    if (0 != ret) {
        goto END;
    }

    vs_log(LOG_MASK_AV, VS_LOG_MODULE, "Parse Done\n");
    
    

    ret = dbg_host_read8(log_addr, (unsigned char *)&local_desc, OFFSET(struct log_desc, msg));
    if (0 != ret) {
        vs_log(LOG_MASK_AV, VS_LOG_WARNING, "Read local_desc failed\n");
        goto END;
    }

    vs_log(LOG_MASK_AV, VS_LOG_INFO, "LOG: enable %d, level %d, total %d, rd %d, wr %d\n",
        local_desc.enable,
        local_desc.level,
        local_desc.total,
        local_desc.rd,
        local_desc.wr
        );

    rd = local_desc.wr;

    fp = fopen(output_filename, "w");
    if (NULL == fp) {
        vs_log(LOG_MASK_AV, VS_LOG_WARNING, "Open/Create %s failed\n", output_filename);
        goto END;
    }

    while (1) {
        ret = dbg_host_read32(log_addr + OFFSET(struct log_desc, wr), &wr, 1);
        if (0 != ret) {
            vs_log(LOG_MASK_AV, VS_LOG_WARNING, "Read WR failed\n");
            goto END;
        }

        vs_log(LOG_MASK_AV, VS_LOG_MODULE, "wr = %d\n", wr);

        for (i = rd; i < wr; i++) {
            msg_addr = log_addr + OFFSET(struct log_desc, msg[i & (local_desc.total - 1)]);

            ret = dbg_host_read8(msg_addr, msg, sizeof(msg));
            if (0 != ret) {
                vs_log(LOG_MASK_AV, VS_LOG_WARNING, "Read msg failed\n");
                goto END;
            }

            msg[sizeof(msg) - 1] = '\0';

            vs_log(LOG_MASK_AV, VS_LOG_INFO, "%d%c) %s", wr - rd, (wr - rd > 512 ? '!' : ' '), msg);

            fprintf(fp, "%d%c) %s", wr - rd, (wr - rd > 512 ? '!' : ' '), msg);
        }

        rd = wr;

        Sleep(10);
    }

END:

    vs_log(LOG_MASK_AV, VS_LOG_INFO, "END\n");

    if (fp) {
        fclose(fp);
    }

    dbg_deinit();

    return 0;
}
