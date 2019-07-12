#include <stdio.h>

#define BUFLEN    (1<<20)

struct pts_entry {
    unsigned int pts;
    unsigned int dts;
    unsigned int flag;
    unsigned int bytecount;
};

unsigned char buffer[BUFLEN];
unsigned int length;

int main(int argc, char *argv[])
{
    int i;
    FILE *fp;
    char *filename;
    unsigned int filesize;
    unsigned char *filecontent;
    struct pts_entry *pts;
    unsigned int byteread;

    if (2 != argc)
        return 0;

    filename = argv[1];

    fp = fopen(filename, "rb");
    if (NULL == fp)
        return 0;

    length = fread(buffer, sizeof(unsigned char), BUFLEN, fp);

    printf("load %d bytes\n", length);

    pts = (struct pts_entry *)buffer;

    for (i = 0; i < length / sizeof(struct pts_entry); i++) {
        printf("%x %x %x %x\n", pts[i].pts, pts[i].dts, pts[i].flag, pts[i].bytecount);
    }

END:
    if (fp)
        fclose(fp);

    return 0;
}

