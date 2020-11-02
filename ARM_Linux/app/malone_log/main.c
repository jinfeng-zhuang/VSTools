#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

/******************************************************************************
 * Macros
 *****************************************************************************/
 
 #define MEMMAP_SIZE    (1<<20)
 #define USLEEP_TIME    (100)
 #define HEARTH_TICK    (1) // 1 second

/******************************************************************************
 * Types
 *****************************************************************************/
 
struct _log_database {
    unsigned int magic;
    unsigned int size;
    unsigned int wr;
    unsigned int rd;
    unsigned int ignored;
    unsigned int debug;
    unsigned char buffer[1]; // buffer[1] >> buffer[512KB]
};

/******************************************************************************
 * Global Vars
 *****************************************************************************/
 
 int stop_flag = 0;

/******************************************************************************
 * Functions
 *****************************************************************************/

unsigned char *mem_map(unsigned int addr, unsigned int size)
{
    static int hidtv_fd = -1;
    
    unsigned int realsize;
    
    if (-1 == hidtv_fd) {
        hidtv_fd = open("/dev/hidtvreg", O_RDWR);
        if (hidtv_fd < 0) {
            printf("hidtv module not ready\n");
            return NULL;
        }
    }

    realsize = (addr & 0xFFFF) + size;

    if (realsize < 0x10000)
        realsize = 0x10000;

    unsigned char *memory = mmap(NULL, realsize, PROT_READ | PROT_WRITE, MAP_SHARED, hidtv_fd, addr & 0xFFFF0000);
    if (-1 == (unsigned int)memory) {
        printf("mmap failed %d\n", errno);
        return NULL;
    }

    return memory + (addr & 0xFFFF);
}

void write2file(char *filename, unsigned char *buffer, unsigned int length, int flag)
{
    static FILE *fp = NULL;
    
    if ((NULL == filename) || (NULL == buffer) || (0 == length))
        return;
    
    if (NULL == fp) {
        fp = fopen(filename, "wb");
    }
    
    if (NULL != fp) {
        fwrite(buffer, 1, length, fp);
    }
    
    if ((flag & 0x1) && (NULL != fp)) {
        fclose(fp);
        fp = NULL;
    }
}

static void signal_handler(int signum)
{
    stop_flag = 1;
}

int main(int argc, char *argv[])
{
    unsigned char *virt = NULL;
    struct _log_database *db = NULL;
    int mask = 0;
    unsigned int log_db_addr = 0;
    char *filename = NULL;
    int stop_count_down = 0;
    unsigned char end_word[128];
    unsigned int cur_ignored;

/*-----------------------------------------------------------------------------
 * Parse Parameters
 */
     if (argc != 3) {
         printf("Usage: %s <ddr_addr> <filename>\n", argv[0]);
         return -1;
     }

    log_db_addr = strtol(argv[1], NULL, 16);
    filename = argv[2];
/*---------------------------------------------------------------------------*/

    signal(SIGINT, signal_handler);
    
    db = (struct _log_database *)mem_map(log_db_addr, MEMMAP_SIZE);
    if (db == NULL)
        return;
    
    // wait for data filling
    while (db->wr == 0)
        usleep(10*1000);
    
    // mask should align to ?
    mask = db->size - 1;
    
    // reset, all start from here
    //db->ignored = 0;
    //db->wr = 0;
    //db->rd = 0;
    cur_ignored = db->ignored;
    
    while (1)
    {
        if ((db->rd & mask) > (db->wr & mask)) {
            write2file(filename, &db->buffer[db->rd & mask], db->size - (db->rd & mask), 0);
            write2file(filename, db->buffer, db->wr & mask, 0);
        }
        else {
            write2file(filename, &db->buffer[db->rd & mask], db->wr - db->rd, 0);
        }
        
        db->rd = db->wr;
        
        if (stop_flag) {
            snprintf(end_word, sizeof(end_word), "END: wr %d rd %d ignored %d\n", db->wr, db->rd, db->ignored - cur_ignored);
            write2file(filename, end_word, strlen(end_word), 1);
            break;
        }
        
        // hearth tick
        if (stop_count_down++ >= (HEARTH_TICK * (1000 * 1000 / USLEEP_TIME))) {
            stop_count_down = 0;
            printf("LOGDB: wr = %x, rd = %x, ignored = %x, debug = %x\n", db->wr, db->rd, db->ignored, db->debug);
        }

        usleep(USLEEP_TIME);
    }
    
    return 0;
}
