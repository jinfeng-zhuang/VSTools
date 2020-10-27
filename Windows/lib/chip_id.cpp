#include <vs/sx.h>
#include <vs/misc.h>

static const char *chip_name_array[] = {
    "UNKNOWN",
    "SX7A",
    "SX7B",
    "SX8A",
    "SX8B",
    "Union1-A",
    "Union1-B",
    "Union2-A",
    "Union2-B"
};

int chip_id(unsigned int value)
{
    int id;

    if (BITGET(value, 0, 16) == 0x9700)
        id = CHIP_SX7A;
    else if (BITGET(value, 0, 16) == 0x9710)
        id = CHIP_SX7B;
    else if (BITGET(value, 0, 16) == 0x9800)
        id = CHIP_SX8A;
    else if (BITGET(value, 0, 16) == 0x9810)
        id = CHIP_SX8B;
    else if ((BITGET(value, 20, 8) == 0x60) && (BITGET(value, 28, 4) == 0))
        id = CHIP_Union1A;
    else if ((BITGET(value, 20, 8) == 0x60) && (BITGET(value, 28, 4) == 1))
        id = CHIP_Union1B;
    else if ((BITGET(value, 20, 8) == 0x71) && (BITGET(value, 28, 4) == 0))
        id = CHIP_Union2A;
    else if ((BITGET(value, 20, 8) == 0x71) && (BITGET(value, 28, 4) == 1))
        id = CHIP_Union2B;
    else
        id = CHIP_UNKNOWN;

    return id;
}

const char *chip_name(unsigned int value)
{
    int id;

    id = chip_id(value);

    return chip_name_array[id];
}
