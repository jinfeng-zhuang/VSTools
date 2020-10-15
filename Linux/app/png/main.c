#include <stdio.h>
#include <assert.h>

#include <Utility/libpng/png.h>

struct image_attr {
    int width;
    int height;
    int color_type;
};

int main(void)
{
    struct image_attr attr;
    unsigned char *image_buffer = NULL;

    image_buffer = load_png("/system/bin/netflix/data/resources/img/gloss.png", &attr);
    assert(NULL != image_buffer);

    return 0;
}

#define ASSERT(x) {if (!(x)); goto ERROR;}

unsigned char *load_png(const char *filename, struct image_attr *attr)
{

    //-------------------------------------------------------------------------
    int ret = 0;
    FILE *fp = NULL;
    unsigned char header[4];

    png_structp png_ptr = NULL;
    png_infop png_info_ptr = NULL;

    unsigned char *image_buffer = NULL;
    int width, height, color_type;
    int size;

    png_bytep *row_ptr = NULL;
    int i, j;

    //-------------------------------------------------------------------------
    ASSERT((NULL != filename) && (NULL != attr))

    fp = fopen(filename, "rb");
    ASSERT(NULL != fp)

    ret = fread(header, 1, 4, fp);
    ASSERT(4 == ret)

    ret = png_sig_cmp(image_buffer, 0, PNG_BYTES_TO_CHECK);
    ASSERT(0 == ret)

    fseek(fp, 0, SEEK_SET);

    //-------------------------------------------------------------------------
    //
    // Attention: png_create_read_struct != png_create_write_struct
    //
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
    ASSERT(NULL != png_ptr);

    png_info_ptr = png_create_info_struct(png_ptr);
    ASSERT(NULL != png_info_ptr);

    ret = setjmp(png_jmpbuf(png_ptr));
    ASSERT(0 == ret);

    png_init_io(png_ptr, fp);

    png_read_png(png_ptr, png_info_ptr, PNG_TRANSFORM_EXPAND, 0);

    width = png_get_image_width(png_ptr, png_info_ptr);
    height = png_get_image_height(png_ptr, png_info_ptr);
    color_type = png_get_color_type(png_ptr, png_info_ptr);

    printf("width %d height %d color_type %d\n", width, height, color_type);

    if (PNG_COLOR_TYPE_RGB_ALPHA == color_type) {
        size = width * height * 4; // RGBA = 4 bytes
        image_buffer = (unsigned char *)malloc(size);
        ASSERT(NULL != image_buffer);

        row_ptr = png_get_rows(png_ptr, png_info_ptr);
        ASSERT(NULL != row_ptr);

        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                image_buffer[i * width * 4 + j * 4 + 0] = row_ptr[i][j * 4 + 0];
                image_buffer[i * width * 4 + j * 4 + 1] = row_ptr[i][j * 4 + 1];
                image_buffer[i * width * 4 + j * 4 + 2] = row_ptr[i][j * 4 + 2];
                image_buffer[i * width * 4 + j * 4 + 3] = row_ptr[i][j * 4 + 3];
            }
        }
    }

    //
    // Attention: png_create_read_struct != png_create_write_struct
    //
    png_destroy_read_struct(&png_ptr, &png_info_ptr, 0);
    fclose(fp);

    attr->width = width;
    attr->height = height;
    attr->color_type = color_type;

    return image_buffer;

ERROR:
    return NULL;
}
