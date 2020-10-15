#ifndef VS_MMAP_H
#define VS_MMAP_H

#define SIZE_1KB (1<<10)

#define PAGE_SIZE (SIZE_1KB * 4)

extern int mem_map_init(void);
extern unsigned char *mem_map(unsigned int addr, unsigned int size);

extern unsigned int ReadWord(unsigned int address);
extern void WriteWord(unsigned int address, unsigned int value);

#endif
