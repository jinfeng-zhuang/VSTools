#include <stdio.h>
#include <unistd.h>

#include <vs/pman.h>
#include <vs/mmap.h>

void print_usage(char *name)
{
    printf("%s <address>\n", name);
    printf("%s <address> <value>\n", name);
    printf("Attention: register start from 0xF0000000\n");
}

int main(int argc, char *argv[])
{
    unsigned int address;
    unsigned int value;

    if (0 != mem_map_init())
        return -1;

    //pman_disable();

    if (2 == argc) {
        sscanf(argv[1], "%x", &address);
        value = ReadWord(address);
        printf("0x%08x\n", value);
    }
    else if (3 == argc) {
        sscanf(argv[1], "%x", &address);
        sscanf(argv[2], "%x", &value);
        WriteWord(address, value);
    }
    else {
        print_usage(argv[0]);
    }

    return 0;
}

