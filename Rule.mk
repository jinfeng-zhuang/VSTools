#
# 2019.11.28, Jinfeng Zhuang
#

TARGET = word
APP = word.c

##########################################################################
# SYSTEM with API integrated
##########################################################################

SOURCE = mem_map.c pman.c g_comm_fd.c
SOURCE += $(APP)

OBJS   = $(patsubst %.c,%.o, $(SOURCE))

export PATH := /opt/p111/armv7a/bin:/opt/cross-arm/linaro-64/bin:$(PATH)

CC      = armv7a-cros-linux-gnueabi-gcc
CFLAGS  = -I./include -Wall

.PHONY:all 
.PHONY:clean 

all : $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) -o $@ $^

%.o:%.c
	@$(CC) -o $@ -c $< $(CFLAGS)

%.d:%.c
	@set -e; rm -f $@; $(CC) -MM $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(OBJS:.o=.d)

clean:
	@rm -f $(TARGET) *.o *.d *.d.*

