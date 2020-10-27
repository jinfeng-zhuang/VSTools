#ifndef MISC_H
#define MISC_H

#define OFFSET(T, f) ((int)(&(((T*)0)->f)))

extern int hexdump(unsigned char* buffer, unsigned int length);

extern int file_save(const char* filename, unsigned char* buffer, unsigned int length);
extern int file_append(const char* filename, unsigned char* buffer, unsigned int length);

#endif