#ifndef MISC_H
#define MISC_H

#include <stddef.h>

///////////////////////////////////////////////////////////////////////////////

//#ifdef _WIN32
#define OFFSET(T, f) (((unsigned int)(&(((T*)0)->f))))
//#else
//#define OFFSET(T,f) offsetof(T, f)
//#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define BITGET(RegValue, StartBit, Bits) (((RegValue) >> (StartBit)) & ((0x1 << (Bits)) - 1))

///////////////////////////////////////////////////////////////////////////////

enum param_type {
    PARAM_FLAG,
    PARAM_STRING,
    PARAM_INT,
    PARAM_FLOAT,
};

struct param {
    const char key;
    const int size;
    int type;
    void *value;
};

///////////////////////////////////////////////////////////////////////////////

extern int hexdump(void* buffer, unsigned int length);

extern int file_save(const char* filename, unsigned char* buffer, unsigned int length);
extern int file_append(const char* filename, unsigned char* buffer, unsigned int length);

extern void print_usage(void);
extern int param_parse(int argc, char *argv[], struct param *arr, int count);

#endif