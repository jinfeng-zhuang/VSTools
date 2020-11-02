#include <vs/misc.h>

const char *mpegformat_framerate_name_array[] = {
    "OTHER",
    "23.976Hz",
    "24Hz",
    "25Hz",
    "29.97Hz",
    "30Hz",
    "50Hz",
    "59.94Hz",
    "60Hz",
    "8Hz",
    "12Hz",
    "15Hz",
    "100Hz",
    "119Hz",
    "120Hz",
    "48Hz",
    "47.952Hz"
};

const char *mpegformat_framerate_name(int rate)
{
    if (rate > ARRAY_SIZE(mpegformat_framerate_name_array))
        return mpegformat_framerate_name_array[0];
    else
        return mpegformat_framerate_name_array[rate];
}
