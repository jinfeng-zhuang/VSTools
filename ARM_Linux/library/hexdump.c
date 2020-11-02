#include <stdio.h>

void hexdump(const char *cause, unsigned char *buf, unsigned int length)
{
    int i = 0; 

    if ((!cause) || (!buf))
        return;

    printf("\nHEXDUMP %d bytes (%s):\n%08X:", length, cause, (unsigned int)buf);

    for (i=0; i<length; i++) {
        printf("%02X ", buf[i]);
    }    

    printf("\n\n");
}
