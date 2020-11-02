#ifndef VS_DBG_H
#define VS_DBG_H

extern int dbg_init(const char* ip, const short port);
extern int dbg_deinit(void);

extern int dbg_host_read32(unsigned int addr, unsigned int* buffer, int count);
extern int dbg_host_write32(unsigned int addr, unsigned int* buffer, int count);

extern int dbg_host_read8(unsigned int addr, unsigned char* buffer, int count);
extern int dbg_host_write8(unsigned int addr, unsigned char* buffer, int count);

extern int dbg_avmips_read32(unsigned int addr, unsigned int* buffer, int count);

#endif