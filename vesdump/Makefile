# http://sh-swbuild-01:8080/jenkins/view/UNION1/job/UNION1-SDK_DailyBuild/lastSuccessfulBuild/artifact/DCP_SYS/misc/Install.tgz

INSTALL = Install_UN1
export PATH := /opt/p111/armv7a/bin:$(PATH)

CC = armv7a-cros-linux-gnueabi-gcc

INC_DIR = -I ./Inc					\
	  -I ./$(INSTALL)/Core/Inc/HAL/AVCore/		\
	  -I ./$(INSTALL)/Core/Inc/HAL/Kernel_Driver/	\
	  -I ./$(INSTALL)/Core/Inc/HAL/TGE/		\
	  -I ./$(INSTALL)/Core/Inc/Utility/TridUtil/	\
	  -I ./$(INSTALL)/Core/Inc/HAL/Inc/

all: printmsg

clean:
	rm vesdump

printmsg: printmsg.c pman.c mem_map.c udpsvr.c comm_get_addr.c Trid_Util_CPUComm_Call.c Trid_Util_CPUComm_Init.c Trid_Util_CRC32.c cpu_comm_is_ready.c
	$(CC) -o $@ $^ $(LIB_DIR) $(LIBS) $(INC_DIR) -D CONFIG_ENDIAN_LITTLE
