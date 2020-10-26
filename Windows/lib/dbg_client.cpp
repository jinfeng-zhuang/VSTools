#include <stdint.h>

#include <vs/net.h>
#include <vs/log.h>

struct host_request {
    unsigned int head;
    unsigned int type;
    unsigned int addr;
    unsigned int count;
};

struct endian {
    unsigned int head;
    unsigned int endian;
};

int dbg_init(const char* ip, const short port)
{
    int ret;
    struct endian request;

    ret = net_connect(ip, port, 0);

    if (ret != 0) {
        vs_log(LOG_MASK_DBG, VS_LOG_ERROR, "%s failed\n", __func__);
        return -1;
    }

    vs_log(LOG_MASK_DBG, VS_LOG_INFO, "%s connected\n", __func__);

    request.head = 18 | ((sizeof(struct endian) - 4) << 16); // TODO
    request.endian = 1;

    ret = net_transfer((unsigned char*)& request, NULL);

    if (-1 == ret) {
        vs_log(LOG_MASK_DBG, VS_LOG_WARNING, "%s set endian failed\n", __func__);
        return -1;
    }

    vs_log(LOG_MASK_DBG, VS_LOG_INFO, "%s set endian done\n", __func__);

    return 0;
}

int dbg_deinit(void)
{
    net_disconnect();

    return 0;
}

int dbg_host_read32(unsigned int addr, unsigned int* buffer, int count)
{
    struct host_request request;
    int ret;

    if ((NULL == buffer) || (0 != (addr % 4)) || (count <= 0)) {
        vs_log(LOG_MASK_DBG, VS_LOG_WARNING, "%s param not correct\n", __func__);
        return -1;
    }

    request.head = 16 | ((sizeof(struct host_request) - 4) << 16);
    request.type = 4;
    request.addr = addr;
    request.count = count;

    ret = net_transfer((unsigned char *)& request, buffer);

    if (-1 == ret) {
        vs_log(LOG_MASK_DBG, VS_LOG_WARNING, "%s failed\n", __func__);
        return -1;
    }

    vs_log(LOG_MASK_DBG, VS_LOG_INFO, "%s done\n", __func__);

    return 0;
}

// debug server's mmap limit 64KB
int dbg_host_read8_4K(unsigned int addr, unsigned char* buffer, int count)
{
    struct host_request request;
    int ret;

    if ((NULL == buffer) || (count <= 0)) {
        vs_log(LOG_MASK_DBG, VS_LOG_WARNING, "%s param not correct\n", __func__);
        return -1;
    }

    // _DBGCMD_RD_HD_BULK_2_
    request.head = 16 | ((sizeof(struct host_request) - 4) << 16); // TODO 16 => Macro
    request.type = 1;
    request.addr = addr;
    request.count = count;

    ret = net_transfer((unsigned char*)& request, buffer);

    if (-1 == ret) {
        vs_log(LOG_MASK_DBG, VS_LOG_WARNING, "%s failed\n", __func__);
        return -1;
    }

    vs_log(LOG_MASK_DBG, VS_LOG_DEBUG, "%s done\n", __func__);

    return 0;
}

int dbg_host_read8(unsigned int addr, unsigned char* buffer, int count)
{
    int ret;
    unsigned int src_inc;
    unsigned char *dst_inc;
    unsigned int remain;
    unsigned int bytes2read;

    src_inc = addr;
    dst_inc = buffer;
    remain = count;

    vs_log(LOG_MASK_DBG, VS_LOG_INFO, "dbg_host_read8 %d...\n", count);

    while (remain > 0) {

        bytes2read = (remain > 0x1000) ? 0x1000 : remain;

        ret = dbg_host_read8_4K(src_inc, dst_inc, bytes2read);
        if (0 != ret) {
            vs_log(LOG_MASK_DBG, VS_LOG_WARNING, "dbg_host_read8 failed\n");
            goto END;
        }

        src_inc += bytes2read;
        dst_inc += bytes2read;

        remain = remain - bytes2read;
    }

    vs_log(LOG_MASK_DBG, VS_LOG_DEBUG, "%s done\n", __func__);

    return 0;

END:
    vs_log(LOG_MASK_DBG, VS_LOG_WARNING, "%s failed\n", __func__);

    return -1;
}

int dbg_avmips_read(unsigned int addr, unsigned char* buffer, unsigned int size)
{
    return 0;
}
