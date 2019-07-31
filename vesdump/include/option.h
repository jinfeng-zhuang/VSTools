#ifndef _VS_OPTION_H_
#define _VS_OPTION_H_

#include <getopt.h>

struct option_t {
    char homedir[64];
    int ptsdump[2];
    int vesdump[2];
};

extern struct option_t g_option;

extern void option_process(int argc, char *argv[]);

extern void print_usage(const char *name);

#endif
