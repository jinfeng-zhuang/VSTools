#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <vs/log.h>

int file_save(const char* filename, unsigned char* buffer, unsigned int length)
{
    FILE* fp;
    unsigned int ret;

    if ((NULL == filename) || (NULL == buffer) || (length == 0)) {
        vs_log(LOG_MASK_MISC, VS_LOG_WARNING, "param error\n");
        return -1;
    }

    fp = fopen(filename, "w");
    if (NULL == fp) {
        vs_log(LOG_MASK_MISC, VS_LOG_WARNING, "fopen %s failed\n", filename);
        return -1;
    }

    ret = fwrite(buffer, sizeof(unsigned char), length, fp);

    if (ret != length) {
        vs_log(LOG_MASK_MISC, VS_LOG_WARNING, "fwrite %s failed: %d != %d\n", filename, ret, length);
        return -1;
    }

    fclose(fp);

    vs_log(LOG_MASK_MISC, VS_LOG_WARNING, "%s done\n", filename);

    return 0;
}