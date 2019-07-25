#ifndef VS_MMAP_H
#define VS_MMAP_H

extern int mem_map_init(void);
extern unsigned char *mem_map(unsigned int addr, unsigned int size);

#endif
