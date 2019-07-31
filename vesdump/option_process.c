#include <stdio.h>
#include <string.h>

#include "option.h"

enum {
    OPT_HELP,
    OPT_VESDUMP,
    OPT_PTSDUMP,
    OPT_HOMEDIR,
    OPT_MAX
};

struct option_t g_option;

static struct option long_options[] = {
    {"help",    no_argument,        NULL, OPT_HELP},
    {"vesdump", required_argument,  NULL, OPT_VESDUMP},
    {"ptsdump", required_argument,  NULL, OPT_PTSDUMP},
    {"homedir", required_argument,  NULL, OPT_HOMEDIR},
    {NULL,      0,                  NULL, 0}
};

void print_usage(const char *name)
{
    printf("Usage: %s --vesdump [0|1] --ptsdump [0|1] --homedir \"/data\"\n", name);
}

void option_process(int argc, char *argv[])
{
    int opt;
    int value;

     while (1) {
        opt = getopt_long_only(argc, argv, "", long_options, NULL);
        if (-1 == opt)
            break;

        if (('?' == opt) || (OPT_HELP == opt)) {
            goto USAGE;
        }

        printf("opt %d optarg %s\n", opt, optarg);

        if (OPT_VESDUMP == opt) {
            value = atoi(optarg);
            if ((value != 0) && (1 != value))
                goto USAGE;
            g_option.vesdump[value] = 1;
        }

        if (OPT_PTSDUMP == opt) {
            value = atoi(optarg);
            if ((value != 0) && (1 != value))
                goto USAGE;
            g_option.ptsdump[value] = 1;
        }

        if (OPT_HOMEDIR == opt) {
            strncpy(g_option.homedir, optarg, sizeof(g_option.homedir));
        }
    }

    return;

USAGE:
    print_usage(argv[0]);
}

