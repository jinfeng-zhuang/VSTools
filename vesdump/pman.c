extern unsigned char *mem_map(unsigned int addr, unsigned int size);

unsigned int pman_v_base;

#define UMAC0_PMAN_REG_BASE 0xF5005100
#define UMAC1_PMAN_REG_BASE 0xF5008100

#define PMAN_GROUP_NUM 16

#define PMAN_SETTING_OFFSET (2*4)
#define PMAN_CTRL_OFFSET (3*4)

#define PMAN_REG_SIZE (8*4)

#define PMAN_ENABLE_ARM_READ_MASK 0xC
#define PMAN_ENABLE_RW_MASK 0x11101

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
    int ii = 0;

    pman_v_base = (unsigned int)mem_map(0xf5000000, 0x10000);

    for(ii = 0; ii < PMAN_GROUP_NUM; ii++)
    {
        ReadReg32(UMAC0_PMAN_REG_BASE + PMAN_SETTING_OFFSET + ii * PMAN_REG_SIZE, &reg_val);
        if(reg_val && ((reg_val & PMAN_ENABLE_ARM_READ_MASK) != PMAN_ENABLE_ARM_READ_MASK))
        {
            reg_val |= PMAN_ENABLE_ARM_READ_MASK;
            WriteReg32(UMAC0_PMAN_REG_BASE + PMAN_SETTING_OFFSET + ii * PMAN_REG_SIZE, reg_val);
        }

        ReadReg32(UMAC0_PMAN_REG_BASE + PMAN_CTRL_OFFSET + ii * PMAN_REG_SIZE, &reg_val);
        if((reg_val != 0) && (reg_val != PMAN_ENABLE_RW_MASK))
        {
            reg_val |= PMAN_ENABLE_RW_MASK;
            WriteReg32(UMAC0_PMAN_REG_BASE + PMAN_CTRL_OFFSET + ii * PMAN_REG_SIZE, reg_val);
        }
    }

    for(ii = 0; ii < PMAN_GROUP_NUM; ii++)
    {
        ReadReg32(UMAC1_PMAN_REG_BASE + PMAN_SETTING_OFFSET + ii * PMAN_REG_SIZE, &reg_val);
        if(reg_val && ((reg_val & PMAN_ENABLE_ARM_READ_MASK) != PMAN_ENABLE_ARM_READ_MASK))
        {
            reg_val |= PMAN_ENABLE_ARM_READ_MASK;
            WriteReg32(UMAC1_PMAN_REG_BASE + PMAN_SETTING_OFFSET + ii * PMAN_REG_SIZE, reg_val);
        }

        ReadReg32(UMAC1_PMAN_REG_BASE + PMAN_CTRL_OFFSET + ii * PMAN_REG_SIZE, &reg_val);
        if((reg_val != 0) && (reg_val != PMAN_ENABLE_RW_MASK))
        {
            reg_val |= PMAN_ENABLE_RW_MASK;
            WriteReg32(UMAC1_PMAN_REG_BASE + PMAN_CTRL_OFFSET + ii * PMAN_REG_SIZE, reg_val);
        }
    }
}
