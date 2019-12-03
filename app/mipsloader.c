#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "api.h"

#define AVMIPS_CODE_BASE    (0x0D400000)

#define PIN_SHARE_UART      (0xF500EE1D)

#define AVMIPS_RESET_VECTOR (0x4002D400)

int main(void)
{
    FILE *fp = NULL;
    unsigned char *buffer = NULL;
    unsigned int file_size;
    unsigned int bytes_read;
    unsigned char *image_mem;

    fp = fopen("avdecoder.bin", "rb");
    assert(NULL != fp);

    fseek(fp, 0, SEEK_END);

    file_size = ftell(fp);

    buffer = (unsigned char *)malloc(file_size);
    assert(NULL != buffer);

    fseek(fp, 0, SEEK_SET);

    bytes_read = fread(buffer, sizeof(char), file_size, fp);
    assert(bytes_read == file_size);

    TRACE;

    mem_map_init();

    TRACE;

    pman_disable();

    image_mem = mem_map(AVMIPS_CODE_BASE, file_size);
    assert(NULL != image_mem);

    //memcpy(image_mem, buffer, file_size);

    //reg_write32(0xF00140DC, AVMIPS_RESET_VECTOR);
    //reg_write8(0xF500EE80, 0x2F);

    reg_write8(0xF500ee1d, 0x11);

    return 0;
}

