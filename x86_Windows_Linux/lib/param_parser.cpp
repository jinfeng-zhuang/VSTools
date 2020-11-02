#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <getopt.h>
#include <vs/misc.h>

#define SHORT_PARAM_STRING_SIZE 128

#ifdef _WIN32
#define snprintf _snprintf
#endif

static char short_param_string[SHORT_PARAM_STRING_SIZE];

static int find_key(struct param *arr, int count, const char key)
{
    int i;

    for (i = 0; i < count; i++) {
        if (key == arr[i].key) {
            return i;
        }
    }

    return -1;
}

int param_parse(int argc, char *argv[], struct param *arr, int count)
{
    int i;
    int opt;

    for (i = 0; i < count; i++) {
        snprintf(short_param_string, SHORT_PARAM_STRING_SIZE - strlen(short_param_string), "%c", arr[i].key);
        if (arr[i].size > 0) {
            snprintf(short_param_string, SHORT_PARAM_STRING_SIZE - strlen(short_param_string), "%c", ':');
        }
    }

    while ((opt = getopt(argc, argv, short_param_string)) != -1)  {
        if (-1 != find_key(arr, count, opt)) {
            switch (arr[i].type) {
                case PARAM_FLAG:
                    *(unsigned char *)arr[i].value = 1;
                    break;
                case PARAM_INT:
                    *(unsigned int *)arr[i].value = atoi(optarg);
                    break;
                default:
                    strncpy((char *)arr[i].value, optarg, arr[i].size);
                    break;
            }
        }
        else {
            return -1;
        }
    }

    return 0;
}
