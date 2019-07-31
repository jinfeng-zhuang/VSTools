#ifndef VS_MISC_H
#define VS_MISC_H

enum chip_id_e {
    CHIP_ID_SX7B = 0x00009710
};

extern enum chip_id_e get_chip_id(void);

#endif
