#ifndef _VS_OPTION_H_
#define _VS_OPTION_H_

#include <getopt.h>

struct option_t {
    char homedir[64];
    int ptsdump[2];
    int vesdump[2];
    int uart;
    unsigned int watch;
};

extern struct option_t g_option;

extern int option_process(int argc, char *argv[]);

extern void print_usage(const char *name);

#endif
