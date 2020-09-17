TARGET = simpledec

SOURCE  += simpledec.c

#==============================================================================

export PATH := /opt/p111/armv7a/bin:$(PATH)

CC = armv7a-cros-linux-gnueabi-gcc

CFLAGS = 	-D CONFIG_ENDIAN_LITTLE							\
															\
			-I ../../Inc									\
			-I ../../Install/Core/Inc/HAL/AVCore/			\
			-I ../../Install/Core/Inc/HAL/Kernel_Driver/	\
			-I ../../Install/Core/Inc/HAL/TGE/				\
			-I ../../Install/Core/Inc/Utility/TridUtil/		\
			-I ../../Install/Core/Inc/HAL/Inc/				\
															\
			-L ../../Install/Core/Lib						\
															\
			-lUtility										\
			-lCPUComm										\
			-lhalvideo										\
			-lDemux											\
			-lhalavstream									\
			-lhalavsync										\
			-lhalcom										\
			-lpthread										\
			-lrt											\
			-lstdc++

#==============================================================================

all: $(TARGET)

clean:
	rm $(TARGET)

$(TARGET): $(SOURCE)
	@$(CC) -o $@ $^ $(CFLAGS)
