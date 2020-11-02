PROJECT = simpledec
SOURCE  = simpledec.c

#==============================================================================

export PATH := /opt/p111/armv7a/bin:$(PATH)

CC = armv7a-cros-linux-gnueabi-gcc

LIB_DIR = -L ../../Install/Core/Lib

INC_DIR = 	-I ../../Inc									\
			-I ../../Install/Core/Inc/HAL/AVCore/			\
			-I ../../Install/Core/Inc/HAL/Kernel_Driver/	\
			-I ../../Install/Core/Inc/HAL/TGE/				\
			-I ../../Install/Core/Inc/Utility/TridUtil/		\
			-I ../../Install/Core/Inc/HAL/Inc/

LIBS := 	-lUtility										\
			-lCPUComm										\
			-lhalvideo										\
			-lDemux											\
			-lhalavstream									\
			-lhalavsync										\
			-lhalcom										\
			-lpthread										\
			-lrt											\
			-lstdc++

all: $(PROJECT)

clean:
	rm $(PROJECT)

$(PROJECT): $(SOURCE)
	@$(CC) -o $@ $^ $(LIB_DIR) $(LIBS) $(INC_DIR) -D CONFIG_ENDIAN_LITTLE
