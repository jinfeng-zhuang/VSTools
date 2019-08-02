export PATH := /opt/p111/armv7a/bin:/opt/cross-arm/linaro-64/bin:$(PATH)

#CC = armv7a-cros-linux-gnueabi-gcc
CC = aarch64-linux-gnu-gcc

INC_DIR = -I ./include

LIBS = -lpthread

all: avtool

clean:
	rm avtool

avtool: main.c func_vesdump.c \
		Trid_Util_CPUComm_InstallRoutine.c \
		get_chip_id.c \
		option_process.c \
		func_ptsdump.c pman.c mem_map.c comm_get_addr.c Trid_Util_CPUComm_Call.c Trid_Util_CPUComm_Init.c Trid_Util_CRC32.c cpu_comm_is_ready.c
	$(CC) -o $@ $^ $(LIB_DIR) $(LIBS) $(INC_DIR) -D CONFIG_ENDIAN_LITTLE
