#ifndef VS_LOG_H
#define VS_LOG_H

enum {
    VS_LOG_ERROR,       // system can't run
    VS_LOG_WARNING,     //
    VS_LOG_INFO,        // normal info, user need to know, below it is error info, uppon is debug info
    VS_LOG_DEBUG,       // developer, module, level
    VS_LOG_VERBOSE,     // developer, function, level, __FUNCTION__, __LINE__
};

extern unsigned int log_module;

extern int log_level;

extern void vs_log_init(unsigned int mudule, int level);
extern int vs_log(unsigned int module, int lvl, const char *fmt, ...);

//=============================================================================
enum {
    LOG_MASK_NET = 1 << 0,
    LOG_MASK_DBG = 1 << 1,
    LOG_MASK_AVMIPS = 1 << 2,
    LOG_MASK_MEMORY = 1 << 3,
    LOG_MASK_REG = 1 << 4,
    LOG_MASK_MISC = 1 << 5,
    LOG_MASK_AV = 1 << 6
};
//=============================================================================

#endif
