#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <Windows.h>

#include <vs/net.h>
#include <vs/log.h>
#include <vs/dbg.h>
#include <vs/misc.h>
#include <vs/sx.h>
#include <vs/avmips.h>

#include <getopt.h>

int test;

int *test_a[] = {&test};

const char *usage = 
"avformat - Print AVMIPS & DISPMIPS interface, build @ %s\n"
"usage: avformat <ip> [options]...\n"
"\n"
"-i ip      ip address\n"
"-f frame   dump current frame decoded info\n"
"-m mpeg    mpeg format info\n"
"-e edr     EDR info\n"
"-h hdr     HDR info\n"
"-c codec   codec, profile, level\n"
"-o output  output format, like compress, tile, raster mode\n"
"-d dump    dump current frame to yuv file\n"
"-s simple  print only different info\n"
"-l loop    show mpeg format periodically\n";

const char *frame_struct_name[] = {
    "TopField",
    "BottomField",
    "Two Field",
    "Frame"
};

const char *codec_name[] = {
    "H264",
    "VC1",
    "MPEG",
    "MPEG4",
    "DIVX3",
    "RV",
    "AVS",
    "JPEG",
    "VP6",
    "SPARK",
    "VP8",
    "M264",
    "MJPEG-A",
    "MJPEG-B",
    "H264",
    "VP9",
    "WMV",
    "MJPEG"
};

const char *output_format_name[] = {
    "Raster Scan Mode",
    "Tile Mode",
    "Compress 8x2",
    "Compress 16x2",
    "Compress 8x4"
};

void print_usage(void)
{
    printf(usage, __DATE__);
}

#if 0
struct avformat_param_t {
    int debug;
    char ip[16];
} avformat_param = {0};

struct param params[] = {
    {'d', 4, PARAM_INT,     &avformat_param.debug},
    {'i', 16, PARAM_STRING, avformat_param.ip},
};
#endif

int main(int argc, char* argv[])
{
    int ret;
    unsigned int id;
    unsigned int cur_frame_info_addr;
    unsigned int bitdepth;
    unsigned int cur_luma_addr;
    unsigned int cur_luma_size;
    unsigned int cur_chroma_addr;
    unsigned int cur_chroma_size;
    unsigned char *luma_data;
    unsigned char *chroma_data;
    struct VideoFrameInfo frame_info;
    struct VideoFrameInfo frame_info_prev;
    struct VideoFrameInfo frame_info_null;
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    char filename[64];
    char *ip;
    unsigned int frameinfo_addr;

    int opt = 0;
    int flag_loop = 0;
    int flag_dump = 0;
    int flag_mpeg = 0;
    int flag_profile = 0;
    int flag_edr = 0;
    int flag_hdr = 0;
    int flag_output = 0;
    int flag_frame = 0;
    int flag_ip = 0;
    int flag_simple = 0;

    if (argc == 1) {
        print_usage();
        return -1;
    }

#if 0
    if (-1 != param_parse(argc, argv, params, ARRAY_SIZE(params))) {
        printf("DONE: ip = %s, level = %d\n", avformat_param.ip, avformat_param.debug);
    }
#endif

    while ((opt = getopt(argc, argv, "sfmehcodhli:")) != -1)  {
        switch (opt) {
            case 'l':
                flag_loop = 1;
                break;
            case 'd':
                flag_dump = 1;
                break;
            case  'c':
                flag_profile = 1;
                break;
            case 'e':
                flag_edr = 1;
                break;
            case 'f':
                flag_frame = 1;
                break;
            case 'm':
                flag_mpeg = 1;
                break;
            case 'o':
                flag_output = 1;
                break;
            case 'i':
                flag_ip = 1;
                ip = optarg;
                break; 
            case 'h':
                flag_hdr = 1;
                break;
            case 's':
                flag_simple = 1;
                break;
            default:
                printf("opt = %c\n", opt);
                print_usage();
                return -1;
        }
    }

    memset(&frame_info_prev, 0, sizeof(struct VideoFrameInfo));
    memset(&frame_info_null, 0, sizeof(struct VideoFrameInfo));

    vs_log_init(LOG_MASK_AV | LOG_MASK_MISC, VS_LOG_INFO);
    
    ret = dbg_init(ip, (unsigned short)65528);
    if (0 == ret) {

        // Get Chip ID
        ret = dbg_host_read32(0xF500E000, &id, 1);
        if (0 == ret) {
            vs_log(LOG_MASK_AV, VS_LOG_INFO, "Chip: %s\n", chip_name(id));
        }

        if ((chip_id(id) != CHIP_SX7B) && (chip_id(id) != CHIP_SX7A) && (chip_id(id) != CHIP_SX8B) && (chip_id(id) != CHIP_SX8A)) {
            vs_log(LOG_MASK_AV, VS_LOG_WARNING, "Not supported\n");
            goto END;
        }

        if ((chip_id(id) == CHIP_SX7B) || (chip_id(id) == CHIP_SX7A)) {
            frameinfo_addr = MPEGDISP_CH1_C0_FRMINFO_SX7;
        }
        else {
            frameinfo_addr = MPEGDISP_CH1_C0_FRMINFO_SX8;
        }

        while (1) {
            // Get Frame Info Register
            ret = dbg_host_read32(frameinfo_addr | 0xF0000000, &cur_frame_info_addr, 1);
            if (0 == ret) {
                vs_log(LOG_MASK_AV, VS_LOG_MODULE, "Current Frame Info Address: 0x%08x\n", cur_frame_info_addr);
                break;
            }
            else {
                vs_log(LOG_MASK_AV, VS_LOG_INFO, "NULL\n");
            }

            Sleep(10);
        }

        vs_log(LOG_MASK_AV, VS_LOG_INFO, "Begin\n");

        while (1) {
            // Get Frame Info
            ret = dbg_host_read8(cur_frame_info_addr, (unsigned char *)&frame_info, sizeof(struct VideoFrameInfo));
            if (0 == ret) {

                if (flag_simple) {
                    if (0 == memcmp(&frame_info, &frame_info_prev, sizeof(struct VideoFrameInfo))) {
                        continue;
                    }
                    else {
                        memcpy(&frame_info_prev, &frame_info, sizeof(struct VideoFrameInfo));
                    }
                }

                if (flag_mpeg) {
                    vs_log(LOG_MASK_AV, VS_LOG_INFO,
                        "\tMPEG Format: %dx%d%c @ %s, %s, Stride %d, Display %dx%d\n",
                        frame_info.format.wHsize,
                        frame_info.format.wVsize,
                        frame_info.format.ProgressiveSeq ? 'p' : 'i',
                        mpegformat_framerate_name(frame_info.format.FrameRate),
                        frame_info.format.PixelDepth ? "10bit" : "8bit",
                        frame_info.format.usStride,
                        frame_info.format.wDispHsize,
                        frame_info.format.wDispVsize
                        );
                }

                if (flag_profile) {
                    vs_log(LOG_MASK_AV, VS_LOG_INFO,
                        "\tCodec %s Profile %d Level %d\n",
                        codec_name[frame_info.tProfileLvl.VideoStandard],
                        frame_info.tProfileLvl.VideoProfile,
                        frame_info.tProfileLvl.VideoLevel
                        );
                }

                if (flag_hdr) {
                    vs_log(LOG_MASK_AV, VS_LOG_INFO,
                        "\tHDRMode %d Addr 0x%x:\n",
                        frame_info.tEDRInfo.HDRModeFlag,
                        frame_info.tEDRInfo.DispColorSeiAddr
                        );
                    hexdump(&frame_info.tDispColor, sizeof(frame_info.tDispColor));
                }

                if (flag_edr) {
                    vs_log(LOG_MASK_AV, VS_LOG_INFO,
                        "\tEDRMode %d Avail %d Addr %x\n",
                        frame_info.tEDRInfo.EDRModeFlag,
                        frame_info.tEDRInfo.MetaDataAvail,
                        frame_info.tEDRInfo.MetaDataAddr
                        );
                }

                if (flag_frame) {
                    vs_log(LOG_MASK_AV, VS_LOG_INFO,
                        "\tType '%d' Ref %d Idx %d PTS 0x%08x DecTime %02d(us) TFF %d RFF %d Struct '%s'\n",
                        frame_info.tPrivData.PicType,
                        frame_info.tPrivData.RefCnt,
                        frame_info.tPrivData.FrameIdx,
                        frame_info.tPrivData.PTS,
                        frame_info.tPrivData.DecTime,
                        frame_info.tPrivData.bTopFieldFirst,
                        frame_info.tPrivData.bRepeatFirstField,
                        frame_struct_name[frame_info.tPrivData.ePictureStructure]
                        );
                }

                if (flag_output) {
                    if (frame_info.tExtInfo.eOutputFormat < ARRAY_SIZE(output_format_name))
                        vs_log(LOG_MASK_AV, VS_LOG_INFO,
                            "\tOutput format '%s' EDR LUT %x\n",
                            output_format_name[frame_info.tExtInfo.eOutputFormat],
                            frame_info.tExtInfo.tLUTAddr
                            );
                    else
                        vs_log(LOG_MASK_AV, VS_LOG_INFO,
                            "\tOutput format %d EDR LUT %x\n",
                            frame_info.tExtInfo.eOutputFormat,
                            frame_info.tExtInfo.tLUTAddr
                            );
                }
            }

            if (flag_loop) {
                Sleep(1000 / 100); // 100Hz
            }
            else {
                break;
            }
        }

        if (flag_dump) {
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

            // Get Chroma Addr
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
    }

END:

    dbg_deinit();

    return 0;
}
