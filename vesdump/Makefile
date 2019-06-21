export PATH := /opt/p111/armv7a/bin:$(PATH)

CC = armv7a-cros-linux-gnueabi-gcc

INC_DIR = -I ./Inc					\
	  -I ./Install/Core/Inc/HAL/AVCore/		\
	  -I ./Install/Core/Inc/HAL/Kernel_Driver/	\
	  -I ./Install/Core/Inc/HAL/TGE/		\
	  -I ./Install/Core/Inc/Utility/TridUtil/	\
	  -I ./Install/Core/Inc/HAL/Inc/

LIB_DIR = -L ./Install/Core/Lib

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
	echo ""

clean:
	rm vesdump

vesdump: vesdump.c pman.c mem_map.c
	$(CC) -o $@ $^ $(LIB_DIR) $(LIBS) $(INC_DIR) -D CONFIG_ENDIAN_LITTLE
