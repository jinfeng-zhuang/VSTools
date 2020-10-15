#include <stdio.h>

#define BITS_PER_PIXCEL 24

#pragma pack(1)
typedef struct
{
    unsigned short  bfType;
    unsigned int    bfSize;
    unsigned short  bfReserved1;
    unsigned short  bfReserved2;
    unsigned int    bfOffBits;
} BMP_FILE_HEADER;

typedef struct {
    unsigned int    biSize;
    int             biWidth;
    int             biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    int             biXPelsPerMeter;
    int             biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
} BMP_INFO_HEADER;
#pragma pack()

int save_bgr_to_bmp(const char* pFileName, unsigned char* pRgbaData, int nWidth, int nHeight)
{
    BMP_FILE_HEADER bmpHeader;
    BMP_INFO_HEADER bmpInfo;

    FILE* fp = NULL;

    int i = 0, j = 0;

    //4字节对齐。每一行必须是4的倍数。
    int bytesPerLine = (nWidth * BITS_PER_PIXCEL + 31) / 32 * 4;
    int pixcelBytes = bytesPerLine * nHeight;

    bmpHeader.bfType = 0x4D42;
    bmpHeader.bfReserved1 = 0;
    bmpHeader.bfReserved2 = 0;
    bmpHeader.bfOffBits = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER);
    bmpHeader.bfSize = bmpHeader.bfOffBits + pixcelBytes;

    bmpInfo.biSize = sizeof(BMP_INFO_HEADER);
    bmpInfo.biWidth = nWidth;

    /** 这样图片才不会倒置 */
    bmpInfo.biHeight = -nHeight;
    bmpInfo.biPlanes = 1;
    bmpInfo.biBitCount = BITS_PER_PIXCEL;
    bmpInfo.biCompression = 0;
    bmpInfo.biSizeImage = pixcelBytes;
    bmpInfo.biXPelsPerMeter = 100;
    bmpInfo.biYPelsPerMeter = 100;
    bmpInfo.biClrUsed = 0;
    bmpInfo.biClrImportant = 0;

    fp = fopen(pFileName, "wb+");
    if (!fp)
    {
        return -1;
    }

    fwrite(&bmpHeader, sizeof(BMP_FILE_HEADER), 1, fp);
    fwrite(&bmpInfo, sizeof(BMP_INFO_HEADER), 1, fp);

    fwrite(pRgbaData, 1, pixcelBytes, fp);

    fclose(fp);

    return 0;
}