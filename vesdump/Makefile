INSTALL = Install_SX7B
export PATH := /opt/p111/armv7a/bin:$(PATH)

CC = armv7a-cros-linux-gnueabi-gcc

INC_DIR = -I ./Inc					\
	  -I ./$(INSTALL)/Core/Inc/HAL/AVCore/		\
	  -I ./$(INSTALL)/Core/Inc/HAL/Kernel_Driver/	\
	  -I ./$(INSTALL)/Core/Inc/HAL/TGE/		\
	  -I ./$(INSTALL)/Core/Inc/Utility/TridUtil/	\
	  -I ./$(INSTALL)/Core/Inc/HAL/Inc/

LIB_DIR = -L ./$(INSTALL)/Core/Lib

LIBS := -lUtility	\
	-lCPUComm	\
	-lhalvideo	\
	-lhalaudio	\
	-lhalavstream	\
	-lhalavsync	\
	-lDemux		\
	-lhalsound	\
	-lDisplay	\
	-lHalSPI	\
	-lTridXMLParser	\
	-lhalcom	\
	-lhalcp		\
	-lpthread	\
	-lm		\
	-lrt		\
	-lstdc++

all: vesdump

clean:
	rm vesdump

vesdump: vesdump.c pman.c mem_map.c udpsvr.c
	$(CC) -o $@ $^ $(LIB_DIR) $(LIBS) $(INC_DIR) -D CONFIG_ENDIAN_LITTLE
