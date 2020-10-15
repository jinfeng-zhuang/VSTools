#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern int save_bgr_to_bmp(const char* pFileName, unsigned char* pRgbaData, int nWidth, int nHeight);
extern unsigned char *load_file(const char* filename, unsigned int *size);

#define ROTATE 0 // 0, 90, 180, 270

#if 0==ROTATE
#define WIDTH   (2008)
#define HEIGHT  (4000)
#define STRIDE  (2048)
#elif 1==ROTATE
#define WIDTH   (2160)
#define HEIGHT  (3840)
#define STRIDE  (6144)
#elif 2==ROTATE
#define WIDTH   (1008)
#define HEIGHT  (2000)
#define STRIDE  (1024)
#else
#define WIDTH   (10)
#define HEIGHT  (10)
#define STRIDE  (6144)
#endif

int main(int argc, char *argv)
{
    unsigned char *buffer;
    unsigned int length;
    unsigned char *rgb_buffer;
    int i;
    int j;
    
#if 2==ROTATE
    buffer = load_file("rotate_180.mem", &length);
#elif 0==ROTATE
    buffer = load_file("rotate_0.mem", &length);
#else
#error "ROTATE not support"
#endif
    assert(NULL != buffer);

    if (length < HEIGHT * STRIDE) {
        printf("file length not match\n");
        return -1;
    }
    
    rgb_buffer = (unsigned char *)malloc(3 * WIDTH * HEIGHT);
    assert(NULL != rgb_buffer);
    
    for (i = 0; i < HEIGHT; i++) {
        for (j=0; j < WIDTH; j++) {
#if 0
            rgb_buffer[(WIDTH * i + j) * 3 + 0] = (int)(i * (255.0 / HEIGHT));
            rgb_buffer[(WIDTH * i + j) * 3 + 1] = (int)(i * (255.0 / HEIGHT));
            rgb_buffer[(WIDTH * i + j) * 3 + 2] = (int)(i * (255.0 / HEIGHT));
#else
            rgb_buffer[(WIDTH * i + j) * 3 + 0] = buffer[STRIDE * i + j];
            rgb_buffer[(WIDTH * i + j) * 3 + 1] = buffer[STRIDE * i + j];
            rgb_buffer[(WIDTH * i + j) * 3 + 2] = buffer[STRIDE * i + j];
#endif
        }
    }
   
#if 0==ROTATE
    save_bgr_to_bmp("rotate.bmp", rgb_buffer, WIDTH, HEIGHT);
#elif 2==ROTATE
    save_bgr_to_bmp("rotate_180.bmp", rgb_buffer, WIDTH, HEIGHT);
#else
#error "ROTATE not support"
#endif

    return 0;
}
