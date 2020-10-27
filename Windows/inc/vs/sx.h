#ifndef SX_H
#define SX_H

enum {
    CHIP_UNKNOWN,
    CHIP_SX7A,
    CHIP_SX7B,
    CHIP_SX8A,
    CHIP_SX8B,
    CHIP_Union1A,
    CHIP_Union1B,
    CHIP_Union2A,
    CHIP_Union2B
};

extern int chip_id(unsigned int value);
extern const char *chip_name(unsigned int value);

#endif