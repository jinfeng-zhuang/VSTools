#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <vs/net.h>
#include <vs/log.h>
#include <vs/dbg.h>
#include <vs/misc.h>

#define DISP_VER_SX7 1
#define DISP_VER_SX8 2

#define MPEGDISP_VERSION DISP_VER_SX7

#define SX_REG_BASE (0x10000000)

#define MPEGDISP_REG_BASE   (SX_REG_BASE + 0x13000)

#define MPEGDISP_Y_C_EVEN (MPEGDISP_REG_BASE + 0x30)
#define MPEGDISP_Y_P1_EVEN (MPEGDISP_REG_BASE + 0x34)
#define MPEGDISP_Y_P2_EVEN (MPEGDISP_REG_BASE + 0x38)
#define MPEGDISP_Y_P3_EVEN (MPEGDISP_REG_BASE + 0x3c)
#define MPEGDISP_C_C_EVEN (MPEGDISP_REG_BASE + 0x40)
#define MPEGDISP_C_P1_EVEN (MPEGDISP_REG_BASE + 0x44)
#define MPEGDISP_C_P2_EVEN (MPEGDISP_REG_BASE + 0x48)
#define MPEGDISP_C_P3_EVEN (MPEGDISP_REG_BASE + 0x4c)

#if (MPEGDISP_VERSION == DISP_VER_SX7)
#define MPEGDISP_CH1_C0_FRMINFO (0x19603DC0)
#define MPEGDISP_CH1_P1_FRMINFO (0x19603DC4)
#define MPEGDISP_CH1_P2_FRMINFO (0x19603DC8)
#define MPEGDISP_CH1_P3_FRMINFO (0x19603DCC)
#define MPEGDISP_CH2_C0_FRMINFO (0x19603DD0)
#define MPEGDISP_CH2_P1_FRMINFO (0x19603DD4)
#define MPEGDISP_CH2_P2_FRMINFO (0x19603DD8)
#define MPEGDISP_CH2_P3_FRMINFO (0x19603DDC)

#define MPEGDISP_CH1_SEAMLESS_FRMINFO (0x19603DE0)
#define MPEGDISP_CH2_SEAMLESS_FRMINFO (0x19603DE4)

#elif(MPEGDISP_VERSION == DISP_VER_SX8)

#define MPEGDISP_CH1_C0_FRMINFO (0x196809D8)
#define MPEGDISP_CH1_P1_FRMINFO (0x196809DC)
#define MPEGDISP_CH1_P2_FRMINFO (0x196809E0)
#define MPEGDISP_CH1_P3_FRMINFO (0x196809E4)
#define MPEGDISP_CH2_C0_FRMINFO (0x196809E8)
#define MPEGDISP_CH2_P1_FRMINFO (0x196809EC)
#define MPEGDISP_CH2_P2_FRMINFO (0x196809F0)
#define MPEGDISP_CH2_P3_FRMINFO (0x196809F4)

#define MPEGDISP_CH1_SEAMLESS_FRMINFO (0x196809F8)
#define MPEGDISP_CH2_SEAMLESS_FRMINFO (0x196809FC)
#endif

struct MpegFormat {
    uint32_t wHsize;
    uint32_t wVsize;
    uint32_t wDispHsize;
    uint32_t wDispVsize;
    uint32_t AspectCode;
    uint32_t FrameRate;
    uint32_t ProgressiveSeq : 1;
    uint32_t FirstField : 1;
    uint32_t clockFreq : 4;
    uint32_t usStride : 16;
    uint32_t PixelDepth : 2;
    uint32_t FieldBufMode : 1;
    uint32_t FrmRateConv : 5;
    uint32_t FullRageMode : 1;
    uint32_t chromaFormat : 1;
    uint32_t AFD;
    uint32_t Par;
};

struct VideoFrameInfo {
    uint32_t start_code;
    uint32_t version;
    struct MpegFormat format;
};

int main(int argc, char* argv[])
{
    int ret;
    unsigned int chip_id;
    unsigned int cur_frame_info_addr;
    unsigned int bitdepth;
    unsigned int cur_luma_addr;
    unsigned int cur_luma_size;
    unsigned int cur_chroma_addr;
    unsigned int cur_chroma_size;
    unsigned char *luma_data;
    unsigned char *chroma_data;
    struct VideoFrameInfo frame_info;
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    char filename[64];
    char *ip;

    if (argc != 2) {
        printf("Usage: %s <ip>\n", argv[0]);
        return -1;
    }

    ip = argv[1];

    vs_log_init(LOG_MASK_AV, VS_LOG_INFO);

    vs_log(LOG_MASK_AV, VS_LOG_INFO, "%s\n", argv[0]);
    
    ret = dbg_init(ip, (unsigned short)65528);
    if (0 == ret) {

        // Get Chip ID
        ret = dbg_host_read32(0xF500E000, &chip_id, 1);
        if (0 == ret) {
            vs_log(LOG_MASK_AV, VS_LOG_INFO, "Chip ID: 0x%08x\n", chip_id);
        }

        if (chip_id != 0x9710) {
            vs_log(LOG_MASK_AV, VS_LOG_WARNING, "Not a SX7B chip, not supported\n");
            goto END;
        }

        // Get Frame Info Register
        ret = dbg_host_read32(MPEGDISP_CH1_C0_FRMINFO | 0xF0000000, &cur_frame_info_addr, 1);
        if (0 == ret) {
            vs_log(LOG_MASK_AV, VS_LOG_DEBUG, "Current Frame Info Address: 0x%08x\n", cur_frame_info_addr);
        }

        if (0 == cur_frame_info_addr) {
            vs_log(LOG_MASK_AV, VS_LOG_WARNING, "Current Frame Info Address is NULL\n");
            goto END;
        }

        // Get Frame Info
        ret = dbg_host_read8(cur_frame_info_addr, (unsigned char *)&frame_info, sizeof(frame_info));
        if (0 == ret) {
            vs_log(LOG_MASK_AV, VS_LOG_INFO, "Current Frame Info: %dx%d%c @ %d, %s, Stride %d\n",
                frame_info.format.wHsize,
                frame_info.format.wVsize,
                frame_info.format.ProgressiveSeq ? 'p' : 'i',
                frame_info.format.FrameRate,
                frame_info.format.PixelDepth ? "10bit" : "8bit",
                frame_info.format.usStride
                );
        }

        width = frame_info.format.wHsize;
        height = frame_info.format.wVsize;
        stride = frame_info.format.usStride;
        bitdepth = frame_info.format.PixelDepth == 0 ? 8 : 10;

        // Get Luma Addr
        ret = dbg_host_read32(MPEGDISP_Y_C_EVEN | 0xF0000000, &cur_luma_addr, 1);
        if (0 == ret) {
            vs_log(LOG_MASK_AV, VS_LOG_INFO, "Current Luma Address: 0x%08x\n", cur_luma_addr);
        }

        if (bitdepth == 10) {
            cur_luma_size = stride * height * 10 / 8;
        }
        else {
            cur_luma_size = stride * height;
        }

        // Get Crhoma Addr
        ret = dbg_host_read32(MPEGDISP_C_C_EVEN | 0xF0000000, &cur_chroma_addr, 1);
        if (0 == ret) {
            vs_log(LOG_MASK_AV, VS_LOG_INFO, "Current Chroma Address: 0x%08x\n", cur_chroma_addr);
        }

        if (bitdepth == 10) {
            cur_chroma_size = stride * height * 10 / 8 / 2;
        }
        else {
            cur_chroma_size = stride * height;
        }

        // Get Y Memory
        // TODO: PMAN not protected area
        // TODO: Should use dbg_host_read8
        _snprintf(filename, sizeof(filename), "%dx%d_%d.yuv", width, height, stride);

        luma_data = (unsigned char*)malloc(cur_luma_size);
        if (NULL == luma_data) {
            vs_log(LOG_MASK_AV, VS_LOG_WARNING, "malloc luma failed\n");
            goto END;
        }

        ret = dbg_host_read8(cur_luma_addr, luma_data, cur_luma_size);

        file_save(filename, luma_data, cur_luma_size);

        // Get UV Memory
        _snprintf(filename, sizeof(filename), "%dx%d_%d.yuv", width, height, stride);

        chroma_data = (unsigned char*)malloc(cur_chroma_size);
        if (NULL == chroma_data) {
            vs_log(LOG_MASK_AV, VS_LOG_WARNING, "malloc chroma failed\n");
            goto END;
        }

        ret = dbg_host_read8(cur_chroma_addr, chroma_data, cur_chroma_size);

        file_append(filename, chroma_data, cur_chroma_size);
    }

END:

    dbg_deinit();

    return 0;
}
