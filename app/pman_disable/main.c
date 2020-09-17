#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

/******************************************************************************
 * Macros
 *****************************************************************************/

#define UMAC0_PMAN_REG_BASE 0xF5005100
#define UMAC1_PMAN_REG_BASE 0xF5008100

#define PMAN_GROUP_NUM 16

#define PMAN_SETTING_OFFSET (2*4)
#define PMAN_CTRL_OFFSET (3*4)

#define PMAN_REG_SIZE (8*4)

#define PMAN_ENABLE_ARM_READ_MASK 0xC
#define PMAN_ENABLE_RW_MASK 0x11101

/******************************************************************************
 * Global Vars
 *****************************************************************************/
 
/*
 * Shared with ReadReg32, WriteReg32 and pman_disable
 */
unsigned int pman_v_base;

static int hidtv_fd;

/******************************************************************************
 * Functions
 *****************************************************************************/

int mem_map_init(void)
{
    hidtv_fd = open("/dev/hidtvreg", O_RDWR);
    if (hidtv_fd < 0) {
        printf("hidtv module not ready\n");
        return -1;
    }

    return 0;
}

unsigned char *mem_map(unsigned int addr, unsigned int size)
{
    unsigned int realsize;

    if (hidtv_fd == -1) {
        printf("hidtv not inited\n");
        return NULL;
    }

    realsize = (addr & 0xFFFF) + size;

    if (realsize < 0x10000)
        realsize = 0x10000;

    //unsigned char *memory = mmap(NULL, realsize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, hidtv_fd, addr & 0xFFFF0000);
    unsigned char *memory = mmap(NULL, realsize, PROT_READ | PROT_WRITE, MAP_SHARED, hidtv_fd, addr & 0xFFFF0000);
    if (-1 == (unsigned int)memory) {
        printf("mmap failed %d\n", errno);
        return NULL;
    }

    return memory + (addr & 0xFFFF);
}

void ReadReg32(unsigned int addr, unsigned int *val)
{
    *val = *((volatile unsigned int *)(addr - 0xf5000000 + pman_v_base));
}

void WriteReg32(unsigned int addr, unsigned int val)
{
    *((volatile unsigned int *)(addr - 0xf5000000 + pman_v_base)) = val;
}

void pman_disable(void)
{
    unsigned int reg_val = 0;
    int i = 0;

    pman_v_base = (unsigned int)mem_map(0xf5000000, 0x10000);

    for(i = 0; i < PMAN_GROUP_NUM; i++)
    {
        ReadReg32(UMAC0_PMAN_REG_BASE + PMAN_SETTING_OFFSET + i * PMAN_REG_SIZE, &reg_val);
        if(reg_val && ((reg_val & PMAN_ENABLE_ARM_READ_MASK) != PMAN_ENABLE_ARM_READ_MASK))
        {
            reg_val |= PMAN_ENABLE_ARM_READ_MASK;
            WriteReg32(UMAC0_PMAN_REG_BASE + PMAN_SETTING_OFFSET + i * PMAN_REG_SIZE, reg_val);
        }

        ReadReg32(UMAC0_PMAN_REG_BASE + PMAN_CTRL_OFFSET + i * PMAN_REG_SIZE, &reg_val);
        if((reg_val != 0) && (reg_val != PMAN_ENABLE_RW_MASK))
        {
            reg_val |= PMAN_ENABLE_RW_MASK;
            WriteReg32(UMAC0_PMAN_REG_BASE + PMAN_CTRL_OFFSET + i * PMAN_REG_SIZE, reg_val);
        }
    }

    for(i = 0; i < PMAN_GROUP_NUM; i++)
    {
        ReadReg32(UMAC1_PMAN_REG_BASE + PMAN_SETTING_OFFSET + i * PMAN_REG_SIZE, &reg_val);
        if(reg_val && ((reg_val & PMAN_ENABLE_ARM_READ_MASK) != PMAN_ENABLE_ARM_READ_MASK))
        {
            reg_val |= PMAN_ENABLE_ARM_READ_MASK;
            WriteReg32(UMAC1_PMAN_REG_BASE + PMAN_SETTING_OFFSET + i * PMAN_REG_SIZE, reg_val);
        }

        ReadReg32(UMAC1_PMAN_REG_BASE + PMAN_CTRL_OFFSET + i * PMAN_REG_SIZE, &reg_val);
        if((reg_val != 0) && (reg_val != PMAN_ENABLE_RW_MASK))
        {
            reg_val |= PMAN_ENABLE_RW_MASK;
            WriteReg32(UMAC1_PMAN_REG_BASE + PMAN_CTRL_OFFSET + i * PMAN_REG_SIZE, reg_val);
        }
    }
    
    // enable malone access share memory
    ReadReg32(0xF50051E8, &reg_val);
    reg_val |= 0x20;
    WriteReg32(0xF50051E8, reg_val);
}

int main(void)
{
    mem_map_init();
    pman_disable();
    return 0;
}
