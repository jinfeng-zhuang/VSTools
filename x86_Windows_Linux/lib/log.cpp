#define _CRT_SECURE_NO_WARNINGS

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <vs/log.h>

unsigned int log_module;
int log_level = VS_LOG_INFO;

char log_buffer[1 << 20];

void vs_log_init(unsigned int mudule, int level)
{
    log_module = mudule;
    log_level = level;
}

int vs_log(unsigned int module, int lvl, const char *fmt, ...)
{
    if ((0 == (module & log_module)) || (lvl > log_level)) {
        return 0;
    }

    log_buffer[0] = '\0';
    
    va_list args;
    va_start(args, fmt);
    
#if _WIN32
    _vsnprintf(log_buffer, sizeof(log_buffer), fmt, args);
#else
    vsnprintf(log_buffer, sizeof(log_buffer), fmt, args);
#endif

    va_end(args);

    printf("%s", log_buffer);
    fflush(stdout);

    if (lvl == VS_LOG_ERROR)
        exit(0xFF);

    return 0;
}
