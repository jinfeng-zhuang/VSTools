#pragma warning(disable:4996)

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vs/log.h>

int hexdump(unsigned char* buffer, unsigned int length)
{
    uint32_t i, j;
    uint32_t line_num;
    uint32_t last_bytes;
    char *hexdump_buffer;

    line_num = length / 16;
    last_bytes = length % 16;
    
    hexdump_buffer = (char *)malloc(length * 10);
    if (NULL == hexdump_buffer) {
        return -1;
    }

    hexdump_buffer[0] = '\0';

    for (i = 0; i < line_num; i++) {
        sprintf(&hexdump_buffer[strlen(hexdump_buffer)], "%08x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            i * 16,
            buffer[i * 16 + 0],
            buffer[i * 16 + 1],
            buffer[i * 16 + 2],
            buffer[i * 16 + 3],
            buffer[i * 16 + 4],
            buffer[i * 16 + 5],
            buffer[i * 16 + 6],
            buffer[i * 16 + 7],
            buffer[i * 16 + 8],
            buffer[i * 16 + 9],
            buffer[i * 16 + 10],
            buffer[i * 16 + 11],
            buffer[i * 16 + 12],
            buffer[i * 16 + 13],
            buffer[i * 16 + 14],
            buffer[i * 16 + 15]);
    }

    if (last_bytes) {
        sprintf(&hexdump_buffer[strlen(hexdump_buffer)], "%08x: ", i * 16);
        for (j = 0; j < last_bytes; j++) {
            sprintf(&hexdump_buffer[strlen(hexdump_buffer)], "%02x ", buffer[i * 16 + j]);
        }
        sprintf(&hexdump_buffer[strlen(hexdump_buffer)], "\n");
    }

    vs_log(LOG_MASK_MISC, VS_LOG_VERBOSE, "%s\n", hexdump_buffer);
    
    return 0;
}
